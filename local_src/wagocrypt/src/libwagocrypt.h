//-----------------------------------------------------------------------------
//  Copyright (c) WAGO GmbH & Co. KG
//
//  PROPRIETARY RIGHTS are involved in the subject matter of this material.
//  All manufacturing, reproduction, use and sales rights pertaining to this
//  subject matter are governed by the license agreement. The recipient of this
//  software implicitly accepts the terms of the license.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
///  \file     libwagocrypt.h
///
///  \version  $Id:
///
///  \brief    IEC61131 -Interface of external CoDeSys 2.3 library 'WagoLibCrypt.lib'
///
///  \author   CM : WAGO GmbH & Co. KG
//-----------------------------------------------------------------------------

#define BOOL  int8_t
#define BYTE  uint8_t
#define USINT int8_t
#define INT   int16_t
#define UINT  uint16_t
#define WORD  uint16_t
#define DINT  int32_t
#define UDINT uint32_t
#define DWORD uint32_t
#define REAL  float
#define LREAL double



typedef struct
{
	DINT nid;
	DINT block_size;
	DINT key_len;
	DINT iv_len;
	DWORD flags;
	DINT init;
	DINT do_cipher;
	DINT cleanup;
	DINT ctx_size;
	DINT set_asn1_parameters;
	DINT get_asn1_parameters;
	DINT ctrl;
	DWORD app_data;
}WCRYPT_stCipher;

BOOL WCRYPT_stCipherinit(WCRYPT_stCipher* inst, BOOL bRetain);

typedef struct
{
	WCRYPT_stCipher *pstCipher;
	DWORD pstEngine;
	DINT encrypt;
	DINT buf_len;
	BYTE oiv[16];
	BYTE iv[16];
	BYTE buf[32];
	DINT num;
	DWORD app_data;
	DINT key_len;
	DWORD flags;
	DWORD cipher_data;
	DINT final_used;
	DINT block_mask;
	BYTE final[32];
}WCRYPT_stCipherCtx;

BOOL WCRYPT_stCipherCtxinit(WCRYPT_stCipherCtx* inst, BOOL bRetain);




/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_ERR_get_error : UDINT
*
* This function is a CoDeSys wrapper for
*
*	unsigned long ERR_get_error(void);
*-----------------------------------------------------------------------------*/
unsigned long WCRYPT_ERR_get_error(void);




/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_aes_unwrap_key : DINT
*
* int EVP_aes_unwrap_key(EVP_CIPHER_CTX *ctx, const unsigned char *iv, unsigned char *out, const unsigned char *in, unsigned int inlen)
* ----------------------------------------------------------------------------------------------------------------------------*/
int WCRYPT_EVP_aes_unwrap_key(EVP_CIPHER_CTX *ctx, 
                              const unsigned char *iv,
	                          unsigned char *out,
	                          const unsigned char *in, 
                              unsigned int inlen);

/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_aes_wrap_key : DINT
*
*   int EVP_aes_wrap_key(EVP_CIPHER_CTX *ctx, const unsigned char *iv, unsigned char *out, const unsigned char *in, unsigned int inlen)
* ----------------------------------------------------------------------------------------------------------------------------*/
int WCRYPT_EVP_aes_wrap_key( EVP_CIPHER_CTX *ctx, 
                             const unsigned char *iv,
                             unsigned char *out,
                             const unsigned char *in, 
                             unsigned int inlen);


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_CIPHER_CTX_cleanup : DINT
*
* This function is a CoDeSys wrapper for
*
*	int EVP_CIPHER_CTX_cleanup (EVP_CIPHER_CTX *c);
*------------------------------------------------------------------------------*/
int WCRYPT_EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX *c);


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_CIPHER_CTX_ctrl : DINT
*
* This function is a CoDesys wrapper for
*
*	int EVP_CIPHER_CTX_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg, void *ptr);
*-----------------------------------------------------------------------------*/
int WCRYPT_EVP_CIPHER_CTX_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg, void *ptr);


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_CIPHER_CTX_free : DINT
*
* This function is a CoDeSys wrapper for
*
*	void EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *a);
*-----------------------------------------------------------------------------*/
void WCRYPT_EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *a);


/** --------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_CIPHER_CTX_init : DINT
* 
* This function is a CoDeSys wrapper for
*
*	void EVP_CIPHER_CTX_init (EVP_CIPHER_CTX *c);
*-----------------------------------------------------------------------------*/
void WCRYPT_EVP_CIPHER_CTX_init(EVP_CIPHER_CTX *c);


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_CIPHER_CTX_new : POINTER TO WCRYPT_stCipherCtx
*
* This function is a CoDeSys wrapper for:
*
*	EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void);
*-----------------------------------------------------------------------------*/
EVP_CIPHER_CTX * WCRYPT_EVP_CIPHER_CTX_new(void);



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
		           			   int enc);


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DecryptFinal_ex : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_DecryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *outm, int *outl);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DecryptFinal_ex( EVP_CIPHER_CTX *ctx, 
                                unsigned char *outm, 
                                int *outl);


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DecryptInit_ex : DINT
*
* This function is a CoDeSys wrapper for
*
*		int	EVP_DecryptInit_ex( EVP_CIPHER_CTX *ctx,const EVP_CIPHER *cipher, ENGINE *impl, const unsigned char *key, const unsigned char *iv);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DecryptInit_ex( EVP_CIPHER_CTX *ctx,
                               const EVP_CIPHER *cipher, 
                               ENGINE *impl, 
                               const unsigned char *key, 
                               const unsigned char *iv);



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
                              int inl);


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DigestInit_ex : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_DigestInit_ex(EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DigestInit_ex(EVP_MD_CTX *ctx, 
                             const EVP_MD *type, 
                             ENGINE *impl);


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DigestSignFinal : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_DigestSignFinal(EVP_MD_CTX *ctx, unsigned char *sigret, size_t *siglen);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DigestSignFinal( EVP_MD_CTX *ctx,
			                    unsigned char *sigret, 
                                size_t *siglen);


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_DigestSignInit : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_DigestSignInit(EVP_MD_CTX *ctx, EVP_PKEY_CTX **pctx,const EVP_MD *type, ENGINE *e, EVP_PKEY *pkey);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_DigestSignInit(  EVP_MD_CTX *ctx, 
                                EVP_PKEY_CTX **pctx,
                                const EVP_MD *type,
                                ENGINE *e, 
                                EVP_PKEY *pkey);


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
                              size_t cnt);


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_EncryptFinal_ex : DINT
*
* This function is a CoDeSys wrapper for
*
*	int	EVP_EncryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl);
*-----------------------------------------------------------------------------*/
int	WCRYPT_EVP_EncryptFinal_ex( EVP_CIPHER_CTX *ctx, 
                                unsigned char *out, 
                                int *outl);


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
                                const unsigned char *iv);


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
                             int inl);


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
const EVP_CIPHER * WCRYPT_EVP_get_cipherbyname(const char *name);



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
const EVP_MD* WCRYPT_EVP_get_digestbyname(const char *name);



/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_MD_CTX_create : DWORD
*
* This function is a CoDEsys wrapper for
*
*	EVP_MD_CTX *EVP_MD_CTX_create(void);
*-----------------------------------------------------------------------------*/
EVP_MD_CTX* WCRYPT_EVP_MD_CTX_create(void);



/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_MD_CTX_destroy : DINT
*
* This function is a CoDeSys wrapper for
*
*	void EVP_MD_CTX_destroy(EVP_MD_CTX *ctx);
*-----------------------------------------------------------------------------*/
void WCRYPT_EVP_MD_CTX_destroy(EVP_MD_CTX *ctx);


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_PKEY_free : DINT
*
* This function is a CoDeSys wrapper for
*
*	void EVP_PKEY_free(EVP_PKEY *pkey);
*-----------------------------------------------------------------------------*/
void WCRYPT_EVP_PKEY_free(EVP_PKEY *pkey);




/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_EVP_PKEY_new_mac_key : DWORD
*
* This function is a CoDeSys wrapper for
*
*	EVP_PKEY *EVP_PKEY_new_mac_key(int type, ENGINE *e, const unsigned char *key, int keylen);
*-----------------------------------------------------------------------------*/
EVP_PKEY *WCRYPT_EVP_PKEY_new_mac_key( int type, 
								       ENGINE *e,	
								       const unsigned char *key, 
								       int keylen);



/** ------------------------------------------------------------------
* FUNCTION WCRYPT_fVersion : WORD
*
* This function provide some general information about this wrapper lib
*----------------------------------------------------------------------*/
unsigned short WCRYPT_fVersion();


/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_OpenSSL_add_all_algs : DINT
*
* This function is CoDeSys wrapper for
*
*	void OpenSSL_add_all_algorithms(void);
*
* Behavior depend on compile flag "OPENSSL_LOAD_CONF"!
*-----------------------------------------------------------------------------*/
void WCRYPT_OpenSSL_add_all_algs(void);



/**----------------------------------------------------------------------------
* FUNCTION WCRYPT_OPENSSL_cleanse : DINT
*
* This function is a CoDeSys wrapper for
*
*	void OPENSSL_cleanse(void *ptr, size_t len);
*-----------------------------------------------------------------------------*/
void WCRYPT_OPENSSL_cleanse(void *ptr, size_t len);


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_OPENSSL_free : DWORD
*
* This function is a CoDeSys wrapper to free memory ...
*
*	void OPENSSL_free(void *addr);
*------------------------------------------------------------------------------*/
void WCRYPT_OPENSSL_free( void *addr );


/** ---------------------------------------------------------------------------
* FUNCTION WCRYPT_OPENSSL_malloc : POINTER TO DWORD
*
* This function is a CoDeSys wrapper to allocate memory ...
*
*	void *OPENSSL_malloc(size_t num);
*------------------------------------------------------------------------------*/
void * WCRYPT_OPENSSL_malloc( size_t num );




