RM=rm -rf

.phony: all clean

all: testwagocrypt_aes testwagocrypt_keywrap testwagocrypt_hmac testwagocrypt_gmac

testwagocrypt_aes: testwagocrypt_aes.c
	$(CC) -Wall testwagocrypt_aes.c -I../src -L../src -lwagocrypt -lssl -lcrypto -ldl -o testwagocrypt_aes

testwagocrypt_keywrap: testwagocrypt_keywrap.c
	$(CC) -Wall testwagocrypt_keywrap.c -I../src -L../src -lwagocrypt -lssl -lcrypto -ldl -o testwagocrypt_keywrap


testwagocrypt_hmac: testwagocrypt_hmac.c
	$(CC) -Wall testwagocrypt_hmac.c -I../src -L../src -lwagocrypt -lssl -lcrypto -ldl -o testwagocrypt_hmac

testwagocrypt_gmac: testwagocrypt_gmac.c
	$(CC) -Wall testwagocrypt_gmac.c -I../src -L../src -lwagocrypt -lssl -lcrypto -ldl -o testwagocrypt_gmac

clean:
	$(RM)  testwagocrypt_aes testwagocrypt_keywrap testwagocrypt_hmac testwagocrypt_gmac

