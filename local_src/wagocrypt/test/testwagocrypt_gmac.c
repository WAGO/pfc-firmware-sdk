/* OpenSSL GMAC example.         */
/* Written and placed in public  */
/*   domain by Jeffrey Walton    */

/* gcc -g3 -O1 -DDEBUG=1 -Wall -Wextra -I/usr/local/ssl/macosx-x64/include/ /usr/local/ssl/macosx-x64/lib/libcrypto.a t.c -o t.exe */
/* gcc -g2 -Os -DNDEBUG=1 -Wall -Wextra -I/usr/local/ssl/macosx-x64/include/ /usr/local/ssl/macosx-x64/lib/libcrypto.a t.c -o t.exe */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define UNUSED(x) ((void)x)
#define ASSERT(x) assert(x)

typedef unsigned char byte;

/* Taken from NIST's GCM Test Vectors    */
/* http://csrc.nist.gov/groups/STM/cavp/ */

/*
 [Keylen = 128]
 [IVlen = 96]
 [PTlen = 0]
 [AADlen = 128]
 [Taglen = 128]
 
 Count = 0
 Key = 77be63708971c4e240d1cb79e8d77feb
 IV = e0e00f19fed7ba0136a797f3
 PT =
 AAD = 7a43ec1d9c0a5a78a0b16533a6213cab
 CT =
 Tag = 209fcc8d3675ed938e9c7166709dd946
*/

int main(int argc, char* argv[])
{
    UNUSED(argc), UNUSED(argv);
    
    int rc = 0, unused;
    unsigned int i;
    
    byte key[] = { 0x77, 0xbe, 0x63, 0x70, 0x89, 0x71, 0xc4, 0xe2, 0x40, 0xd1, 0xcb, 0x79, 0xe8, 0xd7, 0x7f, 0xeb };
    byte iv[] = { 0xe0, 0xe0, 0x0f, 0x19, 0xfe, 0xd7, 0xba, 0x01, 0x36, 0xa7, 0x97, 0xf3 };
    byte aad[] = { 0x7a, 0x43, 0xec, 0x1d, 0x9c, 0x0a, 0x5a, 0x78, 0xa0, 0xb1, 0x65, 0x33, 0xa6, 0x21, 0x3c, 0xab };
    
    byte tag[16] = {};
    byte exp[] = { 0x20, 0x9f, 0xcc, 0x8d, 0x36, 0x75, 0xed, 0x93, 0x8e, 0x9c, 0x71, 0x66, 0x70, 0x9d, 0xd9, 0x46 };

    EVP_CIPHER_CTX *ctx = NULL;
    
    ctx = EVP_CIPHER_CTX_new();
    ASSERT(ctx != NULL);
    if(ctx == NULL) {
        exit (1);
    }
    
    rc = EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL);
    ASSERT(rc == 1);
    if(rc != 1) {
        exit (1);
    }
    
    rc = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, sizeof(iv), NULL);
    ASSERT(rc == 1);
    if(rc != 1) {
        exit (1);
    }
    
    rc = EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv);
    ASSERT(rc == 1);
    if(rc != 1) {
        exit (1);
    }
    
    rc = EVP_EncryptUpdate(ctx, NULL, &unused, aad, sizeof(aad));
    ASSERT(rc == 1);
    if(rc != 1) {
        exit (1);
    }
    
    rc = EVP_EncryptFinal_ex(ctx, NULL, &unused);
    ASSERT(rc == 1);
    if(rc != 1) {
        exit (1);
    }
    
    rc = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, sizeof(tag), tag);
    ASSERT(rc == 1);
    if(rc != 1) {
        exit (1);
    }
    
    printf("Calculated tag:\n  ");
    for(i = 0; i < sizeof(tag); i++)
    {
        printf("%02x", tag[i]);
        
        if(i == sizeof(tag) - 1) {
            printf("\n");
        }
    }
    
    printf("Expected tag:\n  ");
    for(i = 0; i < sizeof(exp); i++)
    {
        printf("%02x", exp[i]);
        
        if(i == sizeof(exp) - 1) {
            printf("\n");
        }
    }
    
    if(ctx) {
        EVP_CIPHER_CTX_free(ctx);
    }
    
    return 0;
}
