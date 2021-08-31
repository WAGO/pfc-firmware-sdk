// Dumbed-down version of flashtool by Jon Povey <jon.povey@racelogic.co.uk>
// Uses Hamming code computation from linux kernel by Frans Meulenbroeks

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <getopt.h>

#include <mtd/mtd-user.h>

#include <stdbool.h>

//#define DEBUG

#ifdef DEBUG
#define DBG(fmt, args...) \
    fprintf(stderr, "%s: " fmt, "DEBUG", ##args)
#else
#define DBG(fmt, args...) \
    ({ if (0) fprintf(stderr, "%s: " fmt, "DEBUG", ##args); 0; })
#endif

enum error_codes {
    NO_ERRORS = 0,
    ERROR_BAD_BLOCK			= 1,
    FATAL_ERROR		= 2	// general fatal error
};

static int   dryrun = 0;

/**
 * @brief                Write a page (minimal write size on NAND), in our case 2048 bytes
 *                     plus 64 bytes metadata (i.e. ECC).
 *
 * @param mtd_fd         Destination file's file descriptor
 * @param page_buf       Pointer to payload buffer (Assumed length payload_len + oob_len)
 * @param start_offset   Start offset of current erase block (in bytes) (needed to write oob)
 * @param page           Nr of 2048 byte long chunk within current (128k long) erase block
 * @param payload_len    Payload length
 * @param oob_len        OOB (Out Of Bounds) data length: Spare area in NAND. In our setup, 
 *                     there are 64 bytes of OOB data after each 2048 bytes of payload. 
 *                     1st byte is reserved, subsequent 12 bytes are used for ECC storage 
 *                     (3 bytes per 512 bytes of payload)
 *
 * @return               0 on success, according errno on error (ioctl, write)
 */
int write_page(int mtd_fd, 
        unsigned char *page_buf,
        size_t start_offset,
        size_t page, 
        size_t payload_len, 
        size_t oob_len,
        bool invalidate_page){

    DBG("write page %d.\n", page);
    int ret;


    // calculate ECC first
    unsigned char *ecc_block;
    int step;  // how many times we do have to calculate the ECC per page

    for(ecc_block = page_buf, step = 0;
            ecc_block < (page_buf + payload_len);
            ecc_block += 512, ++step){
        // ecc pointer: 3 bytes beginning at (end of payload + 1 byte bad block marker)
        __nand_calculate_ecc(ecc_block, 512, page_buf + payload_len + 1 + (3*step));
    }

    if(invalidate_page)
    {
        // Replace checksum of the first 512 bytes with 0xffffff
        // Important: flip bits from 0 to 1 (on NAND erasing is done by setting bits to 1)
        page_buf[payload_len + 2] = 0xff;
        page_buf[payload_len + 3] = 0xff;
        page_buf[payload_len + 4] = 0xff;
    }

    if(dryrun){
        int i;

        for(i=0;i<(2048+64);++i){
            printf("%02x ", page_buf[i]);

            if(i && ((i+1)%16 == 0) )
                printf("\n");
        }

    } else {
        struct mtd_oob_buf oob;

        int ret = write(mtd_fd, page_buf, payload_len);

        if (ret != payload_len) {
            perror("Write page");
            return -errno;
        }

        oob.start = start_offset + page * payload_len; // actually start of oob's data block (absolute address!)
        oob.length = oob_len;
        oob.ptr =  page_buf + payload_len;

        DBG("OOB\n");
        if (ioctl(mtd_fd, MEMWRITEOOB, &oob) != 0) {
            perror("Write OOB");
            return -errno;
        }
    }
    return 0;
}

/**
 * @brief                Read a page (minimal write size on NAND), in our case 2048 bytes, to
 *                     a pre-allocated buffer big enough to store payload AND oob data (64 
 *                     extra bytes)
 *
 * @param image_fd       File descripter of the source file. 
 * @param page_buf       Pre-allocated buffer to read the data into (has to be payload_len + 
 *                     oob_len bytes long)
 * @param payload_len    Size of payload to read (=write size of NAND module)
 * @param oob_len        Size of OOB area
 *
 * @return               0 if no errors happened, according -errno on error (read) 
 */
int read_page(int image_fd, unsigned char *page_buf, size_t payload_len, size_t oob_len){

    DBG("read page.\n");

    int ret;

    memset(page_buf, 0xff, payload_len+oob_len);

    ret = read(image_fd, page_buf, payload_len);

    if (ret == 0) {
        fprintf(stderr, "Unexpected EOF reading input file\n");
        return -errno;
    } else if (ret < 0) {
        perror("Reading image file");
        return -errno;
    }

    return 0;
}

/**
 * @brief TODO
 *
 * @param offset
 * @param erasesize
 *
 * @return 
 */
int erase_block(int mtd_fd, int offset, size_t erasesize)
{
    struct erase_info_user ei;

    DBG("erase block at 0x%x\n", offset);

    ei.start = offset;
    ei.length = erasesize;
    if(dryrun){
        return 0;
    } else {
        return ioctl(mtd_fd, MEMERASE, &ei);
    }
}

/**
 * @brief TODO
 *
 * @param offset
 *
 * @return 
 */
int mark_block_bad(int mtd_fd, loff_t offset)
{
    fprintf(stderr, "mark block bad at 0x%llx\n", offset);

    /*
     * This ioctl may only set the BBT (not sure).
     * We should possibly try to write the manufacturer bad block markers
     * in the block itself, for the UBL.
     */
    // TODO Mark 1st and 2nd OOB page? 
    if(dryrun){
        return 0;
    } else {
        return ioctl(mtd_fd, MEMSETBADBLOCK, &offset);
    }
}

void usage(void)
{
    printf("\nflash-xloader - write xloader image to the first three NAND sectors using 1Bit Hamming ECC scheme.\n\n"
            "Usage:\n"
            "  flash-xloader [PARAMETERS] \n\n"
            "PARAMETERS:\n"
            "  -i, --image        [IMAGEFILE]   xloader image location (i.e. ./mlo)\n"
            "  -d, --destination  [MTD DEVICE]  Destination device (i.e. /dev/mtd3)\n"
            "  -D, --dryrun                     Debug mode: don't write anything\n"
            "  -c, --nr_copies    n (default 1) Write up to 4 copies to NAND (AM35x checks\n"
            "                                   additional blocks if the 1st one is marked\n"
            "                                   as bad)\n"
            "  EXPERT SETTINGS:"
            "  --invalidate                     mark image invalid to make CPU skip it (only use if you're know what you're doing)"
            "  --set-valid        n (default 0) undo change by --invalidate for block nr n"
            "\n"
          );
}

int main(int argc, char **argv){

    int ret;

    char *mtd_path = NULL;  
    char *image_path = NULL;  
    int mtd_fd, image_fd;
    int nr_copies = 1;
    bool invalidate = false;
    bool set_valid = false;
    int block_to_validate = -1;

    for (;;) {
        int option_index = 0;
        static const char *short_options = "i:d:c:V:D";
        static const struct option long_options[] = {
            {"image",		    required_argument,	0, 'i'},
            {"destination",	    required_argument,	0, 'd'},
            {"dryrun",		    no_argument,		0, 'D'},
            {"nr_copies",	    required_argument,  0, 'c'},
            {"invalidate",	    no_argument,        0, 'I'},
            {"set-valid",	    required_argument,  0, 'V'},
            {},
        };

        int c = getopt_long(argc, argv, short_options,
                long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'i':
                image_path = strdup(optarg);
                break;
            case 'd':
                mtd_path = strdup(optarg);
                break;
            case 'D':
                dryrun = 1;
                break;
            case 'c':
                nr_copies = strtol(optarg, NULL, 10);
                break;
            case 'I':
                invalidate = true;
                break;
            case 'V':
                set_valid = true;
                block_to_validate = strtol(optarg, NULL, 10);
                break;

            case '?':
                usage();
                exit(EXIT_FAILURE);
        }
    }

    // Sanity checks
    if(nr_copies < 1 || nr_copies > 4){
        fprintf(stderr, "Illegal value: nr_copies shall be between 1 and 4.\n");
        usage();
        exit(EXIT_FAILURE);
    }

    if( NULL == image_path || NULL == mtd_path){
        usage();
        exit(EXIT_FAILURE);
    }

    if( invalidate && set_valid ){
        fprintf(stderr, "Illegal value: --invalidate and --set-valid cannot be used together.\n");
        usage();
        exit(EXIT_FAILURE);
    }

    if(set_valid && (block_to_validate < 0 || block_to_validate > 3)){
        fprintf(stderr, "Illegal value: block number shall be between 0 and 3.\n");
        usage();
        exit(EXIT_FAILURE);
    }

    printf("Writing %s to %s.\n", image_path, mtd_path);

    static struct mtd_info_user mi;

    // Open mtd and image files
    if ((mtd_fd = open(mtd_path, O_RDWR)) == -1) {
        perror(mtd_path);
        exit(EXIT_FAILURE);
    }

    if (ioctl(mtd_fd, MEMGETINFO, &mi) != 0) {
        perror("MEMGETINFO");
        close(mtd_fd);
        exit(EXIT_FAILURE);
    }

    if (mi.writesize != 2048) {
        fprintf(stderr, "writesize %d not supported\n", mi.writesize);
        exit(EXIT_FAILURE);
    }

    image_fd = open(image_path, O_RDONLY);

    if (image_fd == -1) {
        perror(image_path);
        close(mtd_fd);
        exit(EXIT_FAILURE);
    }

    size_t image_size = lseek(image_fd, 0, SEEK_END);

    if (image_size > mi.size) {
        fprintf(stderr, "Image file is larger than %d bytes: %d\n", mi.size, image_size);
        close(image_fd);
        close(mtd_fd);
        exit(EXIT_FAILURE);
    }


    lseek(image_fd, 0, SEEK_SET); // Better safe than sorry: 
    // We do it once again in loop below

    // We write ECC by ourselves
    DBG("Setting mtd file mode to raw.\n");
    if(!dryrun){
        ret = ioctl(mtd_fd, MTDFILEMODE, (void *) MTD_FILE_MODE_RAW);
        if (ret != 0) {
            perror ("MTDFILEMODE");
            close (mtd_fd);
            exit (EXIT_FAILURE);
        }
    }

    // FIXME: Actually flashtool writes in 128Kb long chunks, processing 2Kb at a
    // time. Is it needed?  At first glance: no because we never write more than
    // one eraseblock. We also don't care about rest of block when flashing
    // xloader via barebox.

    // We have to write in writesize (=2048 Byte) long chunks

    int writesize = mi.writesize;
    int oobsize = mi.oobsize;

    int pages = (image_size-1) / writesize + 1;

    DBG("pages: %d\n", pages);

    unsigned char *page_buf = malloc(writesize + oobsize);

    if(NULL == page_buf){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    enum error_codes error = NO_ERRORS;

    if(set_valid)
    {
        // Write first page w/o erasing sector and invalidating:
        // this way we switch previously invalidated (0->1) bits to 0 again

        // rewind image file descriptor
        lseek(image_fd, 0, SEEK_SET);
        ret = read_page(image_fd, page_buf, writesize, oobsize);

        if(ret < 0){
            perror("read_page");
            error = FATAL_ERROR;
        }
        else
        {
            ret = write_page(mtd_fd, page_buf, block_to_validate * mi.erasesize, 0, writesize, oobsize, false);
            if(ret < 0){
            perror("write_page");
            error = FATAL_ERROR;
        }

        }

    }
    else
    {
        // Write xloader to up to the first 4 blocks

        int block;

        for(block=0; 
            block < nr_copies && error != FATAL_ERROR; // Do enter loop after bad block error 
            ++block){                                  // if marking it as such succeeded

            error = NO_ERRORS;

            bool invalidate_current_copy = invalidate;

            // rewind image file descriptor
            lseek(image_fd, 0, SEEK_SET);

            if (lseek(mtd_fd, block * mi.erasesize, SEEK_SET) != block * mi.erasesize) {
                perror("Write seek");
                error = FATAL_ERROR;
                break;
            }

            // TODO: parameter "erase"?
            ret = erase_block(mtd_fd, block * mi.erasesize, mi.erasesize);

            if(ret == -EIO){
                error = ERROR_BAD_BLOCK; 
            } else if(ret < 0){
                perror("erase_block");
            } else {

                // Read/write in 2048 byte chunks
                int page;
                for(page=0;
                        page<pages && NO_ERRORS == error;
                        ++page){

                    ret = read_page(image_fd, page_buf, writesize, oobsize);

                    if(ret < 0){
                        perror("read_page");
                        error = FATAL_ERROR;
                        break;
                    }

                    if(NO_ERRORS == error){
                        ret = write_page(mtd_fd, page_buf, block * mi.erasesize, page, writesize, oobsize, invalidate_current_copy);
                        invalidate_current_copy = false; // invalidating first page is a sufficient measure

                        if(ret == -EIO){
                            error = ERROR_BAD_BLOCK;
                            break; 
                        } else if(ret < 0){
                            perror("write_page");
                            break;
                        }
                    }

                } // write page
            }

            if(ERROR_BAD_BLOCK == error){

                // Try to mark block as bad:
                // Abort on failure
                // Continue with next block on success

                ret = mark_block_bad(mtd_fd, block * mi.erasesize);

                if(ret < 0){
                    // Block could not be marked as read and will be used by CPU to read xloader
                    perror("Fatal error while marking block as bad");
                    error = FATAL_ERROR;
                }
            }
        }

    }

    if (image_fd != -1)
    {
        close(image_fd);
    }


    if (mtd_fd != -1)
        close(mtd_fd);

    free(page_buf);

    return 0;
}

