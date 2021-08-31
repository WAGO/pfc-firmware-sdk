/* https://wiki.openssl.org/index.php/EVP_Signing_and_Verifying */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project.
 */
/* ====================================================================
 * Copyright (c) 2012 The OpenSSL Project.  All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <openssl/err.h>
#include <openssl/evp.h>
#include "../src/libwagocrypt.h"


static int sign_it(const unsigned char* msg, size_t mlen, unsigned char** sig, size_t* slen, EVP_PKEY* pkey)
{
    /* Returned to caller */
    int result = -1;
    
    if(!msg || !mlen || !sig || !pkey) {
        printf("Param error: sign_it()\n");
        return -1;
    }
    
    if(*sig)
        OPENSSL_free(*sig);
    
    *sig = NULL;
    *slen = 0;
    
    EVP_MD_CTX* ctx = NULL;
    
    do
    {
        ctx = EVP_MD_CTX_create();
        if(ctx == NULL) {
            printf("EVP_MD_CTX_create failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }

        OpenSSL_add_all_algorithms();
        const EVP_MD* md = EVP_get_digestbyname("SHA256");
        if(md == NULL) {
            printf("EVP_get_digestbyname failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        int rc = EVP_DigestInit_ex(ctx, md, NULL);
        if(rc != 1) {
            printf("EVP_DigestInit_ex failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        rc = EVP_DigestSignInit(ctx, NULL, md, NULL, pkey);
        if(rc != 1) {
            printf("EVP_DigestSignInit failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        rc = EVP_DigestSignUpdate(ctx, msg, mlen);
        if(rc != 1) {
            printf("EVP_DigestSignUpdate failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        size_t req = 0;
        rc = EVP_DigestSignFinal(ctx, NULL, &req);
        if(rc != 1) {
            printf("EVP_DigestSignFinal failed (1), error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        if(!(req > 0)) {
            printf("EVP_DigestSignFinal failed (2), error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        *sig = OPENSSL_malloc(req);
        if(*sig == NULL) {
            printf("OPENSSL_malloc failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        *slen = req;
        rc = EVP_DigestSignFinal(ctx, *sig, slen);
        if(rc != 1) {
            printf("EVP_DigestSignFinal failed (3), return code %d, error 0x%lx\n", rc, ERR_get_error());
            break; /* failed */
        }
        
        if(req != *slen) {
            printf("EVP_DigestSignFinal failed, mismatched signature sizes %d, %d\n", req, *slen);
            break; /* failed */
        }
        
        result = 0;       
        
    } while(0);
    
    if(ctx) {
        EVP_MD_CTX_destroy(ctx);
        ctx = NULL;
    }
    
    /* Convert to 0/1 result */
    return !!result;
}


static int verify_it(const unsigned char* msg, size_t mlen, const unsigned char* sig, size_t slen, EVP_PKEY* pkey)
{
    /* Returned to caller */
    int result = -1;
    
    if(!msg || !mlen || !sig || !slen || !pkey) {
        return -1;
    }

    EVP_MD_CTX* ctx = NULL;
    
    do
    {
        ctx = EVP_MD_CTX_create();
        if(ctx == NULL) {
            printf("EVP_MD_CTX_create failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        const EVP_MD* md = EVP_get_digestbyname("SHA256");
        if(md == NULL) {
            printf("EVP_get_digestbyname failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        int rc = EVP_DigestInit_ex(ctx, md, NULL);
        if(rc != 1) {
            printf("EVP_DigestInit_ex failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        rc = EVP_DigestSignInit(ctx, NULL, md, NULL, pkey);
        if(rc != 1) {
            printf("EVP_DigestSignInit failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        rc = EVP_DigestSignUpdate(ctx, msg, mlen);
        if(rc != 1) {
            printf("EVP_DigestSignUpdate failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        unsigned char buff[EVP_MAX_MD_SIZE];
        size_t size = sizeof(buff);
        
        rc = EVP_DigestSignFinal(ctx, buff, &size);
        if(rc != 1) {
            printf("EVP_DigestVerifyFinal failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        

        if(!(size > 0)) {
            printf("EVP_DigestSignFinal failed (2)\n");
            break; /* failed */
        }
        
        const size_t m = (slen < size ? slen : size);
        result = !!CRYPTO_memcmp(sig, buff, m);
        
        OPENSSL_cleanse(buff, sizeof(buff));
        
    } while(0);
    
    if(ctx) {
        EVP_MD_CTX_destroy(ctx);
        ctx = NULL;
    }
    
    /* Convert to 0/1 result */
    return !!result;
}


static void print_it(const char* label, const unsigned char* buff, size_t len)
{
    if(!buff || !len)
        return;
    
    if(label)
        printf("%s: ", label);
    size_t i;
    for(i=0; i < len; ++i)
        printf("%02X", buff[i]);
    
    printf("\n");
}



int main()
{
    const char* msg = "The quick brown fox jumps over the lazy dog";
    const char* pwd = "password";
    const unsigned char* msg_buf = (const unsigned char*)msg;
    const unsigned char* pwd_buf = (const unsigned char*)pwd;
    //http://www.freeformatter.com/hmac-generator.html#ad-output
    //HMAC-SHA1: ac20071d3cb2626fe2eeb59e75a7f8d050698620
    //HMAC-SHA256: f8a691703d515669c03fe5c16519c79ac27ad2e4dddcbad6a88f1015e2a756f7
    //HMAC-SHA512: da67c952e7fc5747a1e9c51445d456b4115bce090a0ce2220c1b634b8e9242487be753dbf6d831491bfa0306dfaf601ce7362774cadbfd3f8e69eee532312e57

    unsigned char* sig = NULL;
    size_t slen = 0;
    int ret;
    EVP_PKEY* pkey; 

    pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, pwd_buf, strlen(pwd));
    fprintf(stderr, "EVP_PKEY-obj: 0x%lx\n", (unsigned long)pkey);
    if(pkey == NULL)
    {
        return -1;
    }
    
    ret = sign_it(msg_buf, strlen(msg), &sig, &slen, pkey);
    fprintf(stderr, "Signing test result %d\n", ret);

    print_it("Signature", sig, slen);
    
    ret = verify_it(msg_buf, strlen(msg), sig, slen, pkey);
    fprintf(stderr, "Verify test result %d\n", ret);

    if(sig)
    {
        OPENSSL_free(sig);
    }
    
    if(pkey)
    {
        EVP_PKEY_free(pkey);
    }
    return 1;
}


