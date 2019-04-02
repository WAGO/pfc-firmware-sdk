/*
   UIO helper functions - header file.

   Relicenced for Wago, until we move it out into a seperate lib

*/

#ifndef UIO_HELPER_H
#define UIO_HELPER_H

#include <stddef.h>
#include <stdint.h>

#define UIO_MAX_NAME_SIZE	64
#define UIO_IPC_IDENT_SIZE	12
#define UIO_MAX_NUM		255

#define UIO_INVALID_SIZE	-1
#define UIO_INVALID_ADDR	(~0)

#define UIO_MMAP_NOT_DONE  0
#define UIO_MMAP_OK     1
#define UIO_MMAP_FAILED    2

/* This should be identical to the define in include/linux/uio_driver.h */
#define MAX_UIO_MAPS	5

struct uio_map_t {
	unsigned long addr;
	size_t size;
   int32_t mmap_result;
};

struct uio_dev_attr_t {
	char name[UIO_MAX_NAME_SIZE];
	char value[UIO_MAX_NAME_SIZE];
	struct uio_dev_attr_t *next;
};

struct uio_info_t {
	int uio_num;
	struct uio_map_t maps[MAX_UIO_MAPS];
	int event_count;
	char name[ UIO_MAX_NAME_SIZE ];
	char version[ UIO_MAX_NAME_SIZE ];
	struct uio_dev_attr_t *dev_attrs;
	struct uio_info_t* next;
};

/* function prototypes */

int uio_get_mem_size(struct uio_info_t* info, int map_num);
int uio_get_mem_addr(struct uio_info_t* info, int map_num);
int uio_get_event_count(struct uio_info_t* info);
int uio_get_name(struct uio_info_t* info);
int uio_get_version(struct uio_info_t* info);
int uio_get_all_info(struct uio_info_t* info);
int uio_get_device_attributes(struct uio_info_t* info);
int uio_open_device(struct uio_info_t* info);
void uio_free_dev_attrs(struct uio_info_t* info);
void uio_free_info(struct uio_info_t* info);
struct uio_info_t* uio_find_devices(int filter_num);
struct uio_info_t* uio_find_devices_by_name(const char *name);
int uio_find_devices_mapping(const char *name, const char *ident_GEODE);

void *uio_mmap_with_addr(void* map_addr, struct uio_info_t* info, int map_num);
void *uio_mmap(struct uio_info_t* info, int map_num);
void uio_munmap(void *p, size_t size);

#endif
