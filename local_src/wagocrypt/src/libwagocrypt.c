//-----------------------------------------------------------------------------
//  Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
//  PROPRIETARY RIGHTS are involved in the subject matter of this material.
//  All manufacturing, reproduction, use and sales rights pertaining to this
//  subject matter are governed by the license agreement. The recipient of this
//  software implicitly accepts the terms of the license.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
///  \file     libwagocrypt.c
///
///  \version  $Id:
///
///  \brief    Building external CoDeSys library 'WagoLibCrypt.lib'
///
///  \author   CM : WAGO Kontakttechnik GmbH & Co. KG
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include "libwagocrypt.h"


static const unsigned char default_iv[] = {
  0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
};


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_ERR_get_error : UDINT
*
* This function is a CoDeSys wrapper for
*
*	unsigned long ERR_get_error(void);
*-----------------------------------------------------------------------------*/
unsigned long WCRYPT_ERR_get_error(void)
{
    return ERR_get_error();
}



/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_aes_unwrap_key : DINT
*
* int EVP_aes_unwrap_key(EVP_CIPHER_CTX *ctx, const unsigned char *iv, unsigned char *out, const unsigned char *in, unsigned int inlen)
* ----------------------------------------------------------------------------------------------------------------------------*/
int WCRYPT_EVP_aes_unwrap_key(   EVP_CIPHER_CTX *ctx, 
                                 const unsigned char *iv,
                                 unsigned char *out,
                                 const unsigned char *in, 
                                 unsigned int inlen)
{
	unsigned char *A, B[16], *R;
	unsigned int i, j, t;
	inlen -= 8;
	if (inlen & 0x7)
		return -1;
	if (inlen < 8)
		return -1;
	A = B;
	t =  6 * (inlen >> 3);
	memcpy(A, in, 8);
	memcpy(out, in + 8, inlen);
	for (j = 0; j < 6; j++)
		{
		R = out + inlen - 8;
		for (i = 0; i < inlen; i += 8, t--, R -= 8)
			{
			A[7] ^= (unsigned char)(t & 0xff);
			if (t > 0xff)	
				{
				A[6] ^= (unsigned char)((t >> 8) & 0xff);
				A[5] ^= (unsigned char)((t >> 16) & 0xff);
				A[4] ^= (unsigned char)((t >> 24) & 0xff);
				}
			memcpy(B + 8, R, 8);
			EVP_Cipher(ctx, B, B, EVP_CIPHER_CTX_block_size(ctx));
			memcpy(R, B + 8, 8);
			}
		}
	if (!iv)
		iv = default_iv;
	if (CRYPTO_memcmp(A, iv, 8))
		{
		OPENSSL_cleanse(out, inlen);
		return 0;
		}
	return inlen;
}


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_aes_wrap_key : DINT
*
*   int EVP_aes_wrap_key(EVP_CIPHER_CTX *ctx, const unsigned char *iv, unsigned char *out, const unsigned char *in, unsigned int inlen)
* ----------------------------------------------------------------------------------------------------------------------------*/
int WCRYPT_EVP_aes_wrap_key(   EVP_CIPHER_CTX *ctx, 
                               const unsigned char *iv,
                               unsigned char *out,
                               const unsigned char *in, 
                               unsigned int inlen)
{
	unsigned char *A, B[16], *R;
	unsigned int i, j, t;
	if ((inlen & 0x7) || (inlen < 8))
		return -1;
	A = B;
	t = 1;
	memcpy(out + 8, in, inlen);
	if (!iv)
		iv = default_iv;

	memcpy(A, iv, 8);

	for (j = 0; j < 6; j++)
		{
		R = out + 8;
		for (i = 0; i < inlen; i += 8, t++, R += 8)
			{
			memcpy(B + 8, R, 8);
			EVP_Cipher(ctx, B, B, EVP_CIPHER_CTX_block_size(ctx));
			A[7] ^= (unsigned char)(t & 0xff);
			if (t > 0xff)	
				{
				A[6] ^= (unsigned char)((t >> 8) & 0xff);
				A[5] ^= (unsigned char)((t >> 16) & 0xff);
				A[4] ^= (unsigned char)((t >> 24) & 0xff);
				}
			memcpy(R, B + 8, 8);
			}
		}
	memcpy(out, A, 8);
	return inlen + 8;
}


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_CIPHER_CTX_cleanup : DINT
*
* This function is a CoDeSys wrapper for
*
*	int EVP_CIPHER_CTX_cleanup (EVP_CIPHER_CTX *c);
*------------------------------------------------------------------------------*/
int WCRYPT_EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX *c)
{
    return EVP_CIPHER_CTX_cleanup (c);
}


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_CIPHER_CTX_ctrl : DINT
*
* This function is a CoDesys wrapper for
*
*	int EVP_CIPHER_CTX_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg, void *ptr);
*-----------------------------------------------------------------------------*/
int WCRYPT_EVP_CIPHER_CTX_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg, void *ptr)
{
    return EVP_CIPHER_CTX_ctrl(ctx, type, arg, ptr);
}


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_CIPHER_CTX_free : DINT
*
* This function is a CoDeSys wrapper for
*
*	void EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *a);
*-----------------------------------------------------------------------------*/
void WCRYPT_EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *a)
{
    EVP_CIPHER_CTX_free(a);
}


/** --------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_CIPHER_CTX_init : DINT
* 
* This function is a CoDeSys wrapper for
*
*	void EVP_CIPHER_CTX_init (EVP_CIPHER_CTX *c);
*-----------------------------------------------------------------------------*/
void WCRYPT_EVP_CIPHER_CTX_init (EVP_CIPHER_CTX *c)
{
    (void)EVP_CIPHER_CTX_init(c);
}


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_CIPHER_CTX_new : POINTER TO WCRYPT_stCipherCtx
*
* This function is a CoDeSys wrapper for:
*
*	EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void);
*-----------------------------------------------------------------------------*/
EVP_CIPHER_CTX * WCRYPT_EVP_CIPHER_CTX_new(void)
{
    return EVP_CIPHER_CTX_new();
}


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_CipherInit_ex : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_CipherInit_ex( EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher, ENGINE *impl, const unsigned char *key, const unsigned char *iv, int enc);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_CipherInit_ex(  EVP_CIPHER_CTX *ctx,
						       const EVP_CIPHER *cipher,
                               ENGINE *impl,
		           			   const unsigned char *key,
						       const unsigned char *iv,
		           			   int enc)
{
    return EVP_CipherInit_ex( ctx, cipher, impl, key, iv, enc);
}



/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DecryptFinal_ex : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_DecryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *outm, int *outl);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DecryptFinal_ex( EVP_CIPHER_CTX *ctx, 
                                unsigned char *outm, 
                                int *outl)
{
    return EVP_DecryptFinal_ex(ctx, outm, outl);
}

/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DecryptInit_ex : DINT
*
* This function is a CoDeSys wrapper for
*
*		int	EVP_DecryptInit_ex( EVP_CIPHER_CTX *ctx,const EVP_CIPHER *cipher, 
*                               ENGINE *impl, const unsigned char *key, 
*                               const unsigned char *iv);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DecryptInit_ex( EVP_CIPHER_CTX *ctx,
                               const EVP_CIPHER *cipher, 
                               ENGINE *impl, 
                               const unsigned char *key, 
                               const unsigned char *iv)
{
    return EVP_DecryptInit_ex( ctx, cipher, impl, key, iv);
}


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DecryptUpdate : DINT
*
* This function is a CoDeSys wrapper for
*	
*	int	EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DecryptUpdate( EVP_CIPHER_CTX *ctx, 
                              unsigned char *out, 
                              int *outl, 
                              const unsigned char *in, 
                              int inl)
{
    return EVP_DecryptUpdate(ctx, out, outl, in, inl);
}


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DigestInit_ex : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_DigestInit_ex(EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DigestInit_ex(EVP_MD_CTX *ctx, 
                             const EVP_MD *type, 
                             ENGINE *impl)
{
    return EVP_DigestInit_ex(ctx, type, impl);
}


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DigestSignFinal : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_DigestSignFinal(EVP_MD_CTX *ctx, unsigned char *sigret, size_t *siglen);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DigestSignFinal( EVP_MD_CTX *ctx,
			                    unsigned char *sigret, 
                                size_t *siglen)
{
    return	EVP_DigestSignFinal(ctx, sigret, siglen);
}


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DigestSignInit : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_DigestSignInit(EVP_MD_CTX *ctx, EVP_PKEY_CTX **pctx,const EVP_MD *type,
*                          ENGINE *e, EVP_PKEY *pkey);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DigestSignInit(  EVP_MD_CTX *ctx, 
                                EVP_PKEY_CTX **pctx,
                                const EVP_MD *type,
                                ENGINE *e, 
                                EVP_PKEY *pkey)
{
    return EVP_DigestSignInit(ctx, pctx, type, e, pkey);
}


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DigestUpdate : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_DigestUpdate(EVP_MD_CTX *ctx,const void *d, size_t cnt);
*	int	EVP_DigestSignUpdate(EVP_MD_CTX *ctx,const void *d, size_t cnt);
*	int	EVP_DigestVerifyUpdate(EVP_MD_CTX *ctx,const void *d, size_t cnt);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DigestUpdate(  EVP_MD_CTX *ctx,
                              const void *d, 
                              size_t cnt)
{
    return EVP_DigestUpdate(ctx, d, cnt);
}


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_EncryptFinal_ex : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_EncryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_EncryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
{
    return EVP_EncryptFinal_ex(ctx, out, outl);
}


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_EncryptInit_ex : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher, ENGINE *impl, const unsigned char *key, const unsigned char *iv);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_EncryptInit_ex(  EVP_CIPHER_CTX *ctx,
                                const EVP_CIPHER *cipher, 
                                ENGINE *impl,
		                        const unsigned char *key, 
                                const unsigned char *iv)
{
    return EVP_EncryptInit_ex(ctx, cipher, impl, key, iv);
}


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_EncryptUpdate : DINT
*
* This function is a CoDesys wrapper for
*
*	int	EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx, 
                             unsigned char *out, 
                             int *outl, 
                             const unsigned char *in,
                             int inl)
{
    return EVP_EncryptUpdate(ctx, out, outl, in, inl);
}



/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_get_cipherbyname : POINTER TO WCRYPT_stCipher
* 
* This function is a CoDeSys wrapper for:
* 
*	const EVP_CIPHER *EVP_get_cipherbyname(const char *name);
*
* List of available ciphers and digest have to be filled with function: 
*        OpenSSL_add_all_algorithms()
*-----------------------------------------------------------------------------*/
const EVP_CIPHER * WCRYPT_EVP_get_cipherbyname(const char *name)
{      
    return EVP_get_cipherbyname(name);
}


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_get_digestbyname : DWORD
*
* This function is a CoDeSys wrapper for
*
*	const EVP_MD *EVP_get_digestbyname(const char *name);
*
* List of available ciphers and digest have to be filled with function: 
*        OpenSSL_add_all_algorithms()
*-----------------------------------------------------------------------------*/
const EVP_MD* WCRYPT_EVP_get_digestbyname(const char *name)
{
    return EVP_get_digestbyname(name);
}


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_MD_CTX_create : DWORD
*
* This function is a CoDEsys wrapper for
*
*	EVP_MD_CTX *EVP_MD_CTX_create(void);
*-----------------------------------------------------------------------------*/
EVP_MD_CTX* WCRYPT_EVP_MD_CTX_create(void)
{
    return EVP_MD_CTX_create();
}


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_MD_CTX_destroy : DINT
*
* This function is a CoDeSys wrapper for
*
*	void EVP_MD_CTX_destroy(EVP_MD_CTX *ctx);
*-----------------------------------------------------------------------------*/
void WCRYPT_EVP_MD_CTX_destroy(EVP_MD_CTX *ctx)
{
    return EVP_MD_CTX_destroy(ctx);
}


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_PKEY_free : DINT
*
* This function is a CoDeSys wrapper for
*
*	void EVP_PKEY_free(EVP_PKEY *pkey);
*-----------------------------------------------------------------------------*/
void WCRYPT_EVP_PKEY_free(EVP_PKEY *pkey)
{
    return EVP_PKEY_free(pkey);
}


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_PKEY_new_mac_key : DWORD
*
* This function is a CoDeSys wrapper for
*
*	EVP_PKEY *EVP_PKEY_new_mac_key(int type,ENGINE *e,const unsigned char *key,int keylen);
*-----------------------------------------------------------------------------*/
EVP_PKEY* WCRYPT_EVP_PKEY_new_mac_key( int type, 
								       ENGINE *e,	
								       const unsigned char *key, 
								       int keylen)
{
    return EVP_PKEY_new_mac_key( type, e, key, keylen);
}


/** ------------------------------------------------------------------
* FUNCTION WCRYPT_fVersion 
*----------------------------------------------------------------------*/
unsigned short WCRYPT_fVersion()
{
    return 0x0100;
}


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_OpenSSL_add_all_algs : DINT
*
* This function is CoDeSys wrapper for
*
*	void OpenSSL_add_all_algorithms(void);
*
* Behavior depend on compile flag "OPENSSL_LOAD_CONF"!
*-----------------------------------------------------------------------------*/
void WCRYPT_OpenSSL_add_all_algs(void)
{
    (void)OpenSSL_add_all_algorithms();
}

/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_OPENSSL_cleanse : DINT
*
* This function is a CoDeSys wrapper for
*
*	void OPENSSL_cleanse(void *ptr, size_t len);
*-----------------------------------------------------------------------------*/
void WCRYPT_OPENSSL_cleanse(void *ptr, size_t len)
{
    OPENSSL_cleanse(ptr, len);
}

/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_OPENSSL_free : DWORD
*
* This function is a CoDeSys wrapper to free memory ...
*
*	void OPENSSL_free(void *addr);
*------------------------------------------------------------------------------*/
void WCRYPT_OPENSSL_free( void *addr )
{
    OPENSSL_free(addr);
}

/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_OPENSSL_malloc : POINTER TO DWORD
*
* This function is a CoDeSys wrapper to allocate memory ...
*
*	void *OPENSSL_malloc(size_t num);
*------------------------------------------------------------------------------*/
void * WCRYPT_OPENSSL_malloc( size_t num )
{
    return OPENSSL_malloc(num);
}





/** ------------------------------------------------------------------
* Init function for struct 'WCRYPT_stCipher'
*----------------------------------------------------------------------*/
BOOL WCRYPT_stCipherinit(WCRYPT_stCipher* inst, BOOL bRetain)
{
	inst->nid = 0L;
	inst->block_size = 0L;
	inst->key_len = 0L;
	inst->iv_len = 0L;
	inst->flags = 0UL;
	inst->init = 0L;
	inst->do_cipher = 0L;
	inst->cleanup = 0L;
	inst->ctx_size = 0L;
	inst->set_asn1_parameters = 0L;
	inst->get_asn1_parameters = 0L;
	inst->ctrl = 0L;
	inst->app_data = 0UL;
	return bRetain;
}

/** ------------------------------------------------------------------
* Init function for struct 'WCRYPT_stCipherCtx'
*----------------------------------------------------------------------*/
BOOL WCRYPT_stCipherCtxinit(WCRYPT_stCipherCtx* inst, BOOL bRetain)
{
	int j; /* Loop counter */
	inst->pstCipher = 0UL;
	inst->pstEngine = 0UL;
	inst->encrypt = 0L;
	inst->buf_len = 0L;
	for (j = 0; j < 16; j++)
	{
			(*((inst->oiv) + j)) = 0;
	}
	for (j = 0; j < 16; j++)
	{
			(*((inst->iv) + j)) = 0;
	}
	for (j = 0; j < 32; j++)
	{
			(*((inst->buf) + j)) = 0;
	}
	inst->num = 0L;
	inst->app_data = 0UL;
	inst->key_len = 0L;
	inst->flags = 0UL;
	inst->cipher_data = 0UL;
	inst->final_used = 0L;
	inst->block_mask = 0L;
	for (j = 0; j < 32; j++)
	{
			(*((inst->final) + j)) = 0;
	}

	return bRetain;
}








