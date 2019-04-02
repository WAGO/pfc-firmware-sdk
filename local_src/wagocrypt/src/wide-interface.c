//-----------------------------------------------------------------------------
//  Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
//  PROPRIETARY RIGHTS are involved in the subject matter of this material.
//  All manufacturing, reproduction, use and sales rights pertaining to this
//  subject matter are governed by the license agreement. The recipient of this
//  software implicitly accepts the terms of the license.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
///  \file     wide-interface.c
///
///  \version  $Id:
///
///  \brief    WIDE interface for external CoDeSys 2.3 library 'WagoLibCrypt.lib'
///
///  \author   CM : WAGO Kontakttechnik GmbH & Co. KG
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <openssl/evp.h>
#include "libwagocrypt.h"


#define ADD_FUNC(func) { #func, (void (*)(void))func}
#define END_OF_TABLE  { "", NULL }

enum ErrorCodes
{
    CLD_SUCCESS = 0,
    CLD_FAILURE = 2,
};

typedef struct tFunctionTable  
{
    char  Name[32];
    void  (*Function)(void);
} tFunctionTable;


typedef struct 
{
    const tFunctionTable *(*GetFunctionTable)(void);
    int32_t (*Close)(void);
} tCoDeSysLib;


/**
 * Table containing the functions implemented by this lib.
 * The functions are matched via their name by the runtime (CoDeSys).
 * Note that the function table must be finished with an empty string
 * and a NULL function pointer.
 */
/* ------------------------------------------------------------------*/
/* ATTENTION: Add all function names  -------------------------------*/
/* ------------------------------------------------------------------*/
static const tFunctionTable s_ft[] =
{
    /* add all your functions from the generated c-file here */
    ADD_FUNC(WCRYPT_ERR_get_error),    
    ADD_FUNC(WCRYPT_EVP_aes_unwrap_key),
    ADD_FUNC(WCRYPT_EVP_aes_wrap_key),
    ADD_FUNC(WCRYPT_EVP_CIPHER_CTX_cleanup),
    ADD_FUNC(WCRYPT_EVP_CIPHER_CTX_ctrl),
    ADD_FUNC(WCRYPT_EVP_CIPHER_CTX_free),
    ADD_FUNC(WCRYPT_EVP_CIPHER_CTX_init),
    ADD_FUNC(WCRYPT_EVP_CIPHER_CTX_new),
    ADD_FUNC(WCRYPT_EVP_CipherInit_ex),
    ADD_FUNC(WCRYPT_EVP_DecryptFinal_ex),
    ADD_FUNC(WCRYPT_EVP_DecryptInit_ex),
    ADD_FUNC(WCRYPT_EVP_DecryptUpdate),
    ADD_FUNC(WCRYPT_EVP_DigestInit_ex),
    ADD_FUNC(WCRYPT_EVP_DigestSignFinal),
    ADD_FUNC(WCRYPT_EVP_DigestSignInit),
    ADD_FUNC(WCRYPT_EVP_DigestUpdate),
    ADD_FUNC(WCRYPT_EVP_EncryptFinal_ex),
    ADD_FUNC(WCRYPT_EVP_EncryptInit_ex),
    ADD_FUNC(WCRYPT_EVP_EncryptUpdate),
    ADD_FUNC(WCRYPT_EVP_get_cipherbyname),
    ADD_FUNC(WCRYPT_EVP_get_digestbyname),
    ADD_FUNC(WCRYPT_EVP_MD_CTX_create),
    ADD_FUNC(WCRYPT_EVP_MD_CTX_destroy),
    ADD_FUNC(WCRYPT_EVP_PKEY_free),
    ADD_FUNC(WCRYPT_EVP_PKEY_new_mac_key),
    ADD_FUNC(WCRYPT_fVersion),
    ADD_FUNC(WCRYPT_OpenSSL_add_all_algs),
    ADD_FUNC(WCRYPT_OPENSSL_cleanse),
    ADD_FUNC(WCRYPT_OPENSSL_free),
    ADD_FUNC(WCRYPT_OPENSSL_malloc),
    /* WIDE init function for user defined data types */
    ADD_FUNC(WCRYPT_stCipherinit),
    ADD_FUNC(WCRYPT_stCipherCtxinit),
    END_OF_TABLE,
};

/**
 * The GetFunctionTable function is called to retrieve implemented functions 
 */
static const tFunctionTable * cld_GetFunctionTable(void)
{
    printf("called %s\n", __func__);
    return s_ft;    
}

/**
 *  The Close function gets called when CoDeSys is shutting down.
 *  Any resources used by the library should be released here.
 */
static int32_t cld_Close(void)
{
	printf("called %s\n", __func__);
    /* Cleanup resources here */
    /* Make sure that threads are really finished, via pthread_join for example 
     * Otherwise you are likely to get a ‘segmentation fault’ who hard to find 
     * because the .so is unloaded immediately after the call to Close, and thus
     * the symbols are not available any more.
     */
    return CLD_SUCCESS;
}


/**
 * The Open function is called after the library has been loaded.
 * Any initialization code should be placed here, mainly setting
 * up the v-table (tCoDeSysLib).
*/
/* ------------------------------------------------------------------*/
/* ATTENTION: Don't forget to update name of lib<xy>_open() ---------*/
/* ------------------------------------------------------------------*/
int32_t libwagocrypt_open(tCoDeSysLib *me, 
                       void const * const dont_use1, 
                       void const * const dont_use2)
{
    // Remove compiler warning
    (void)dont_use1;
    (void)dont_use2;
    printf("called %s\n", __func__);
    // Setup v-table
    me->Close = cld_Close;
    me->GetFunctionTable = cld_GetFunctionTable;
    // return whether the lib was initialized successfully.
    // when CLD_FAILURE is returned the library will get unloaded immediately
    // after this call.
    // In the failure case take care to free used resources, such as memory
    // files, sockets, threads, etc) 
	return CLD_SUCCESS;
}


