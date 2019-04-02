CFLAGS=-ggdb2 -O0

all: flash-xloader

nand_ecc.o: nand_ecc.c
	$(CC) -c -DSTANDALONE nand_ecc.c -o nand_ecc.o $(CFLAGS)

flash-xloader: nand_ecc.o flash-xloader.o
	$(CC) -o flash-xloader nand_ecc.o flash-xloader.o

clean:
	rm -f flash-xloader *.o
