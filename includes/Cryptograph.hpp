#ifndef __CRYPTOGRAPH__H__
#define __CRYPTOGRAPH__H__

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <iostream>
#include <string.h>

/* For HTONL */
#include <netinet/in.h>

#define AES_ROUNDS 6
#define RSA_KEYLEN 2048
#define CRYPT_BUFFER_SIZE		4096

class Cryptograph
{
	private:
//#ifdef USE_RSA

		/* RSA */
		EVP_PKEY 		*_localKeypair;
		EVP_PKEY        *_remotePublicKey;

		EVP_CIPHER_CTX  *_rsaEncryptContext;
    	EVP_CIPHER_CTX  *_rsaDecryptContext;

		int 	initRSA();
		int 	generateRsaKeypair(EVP_PKEY **keypair);
		int		bioToString(BIO *bio, unsigned char **string);

//#else
		/* AES */
		EVP_CIPHER_CTX 	*_aesEncryptContext;
		EVP_CIPHER_CTX 	*_aesDecryptContext;

		unsigned char 	*_aesKey;
    	unsigned char 	*_aesIv;

		size_t 			_aesKeyLength;
    	size_t 			_aesIvLength;
		
		int 	initAES(void);
    	int 	generateAesKey(unsigned char **aesKey, unsigned char **aesIv);

//#endif //USE_RSA

		/* General */
		int init(void);
		int deInit(void);

	public:
		Cryptograph(void);
		~Cryptograph(void);
		Cryptograph &operator=(Cryptograph const &rhs);

//#ifdef USE_RSA

		/* RSA */

		int 	RSAEncrypt(const unsigned char *message, size_t messageLength, unsigned char **encryptedMessage);
		int		RSADecrypt(unsigned char *encryptedMessage, size_t encryptedMessageLength, unsigned char **decryptedMessage,
							unsigned char *encryptedKey, size_t encryptedKeyLength, unsigned char *iv, size_t ivLength);

		int				getLocalPrivateKey(unsigned char **privateKey);
		int				getLocalPublicKey(unsigned char **publickey);

		EVP_PKEY		*getLocalKeypairEVP(void);

		EVP_PKEY		*getRemotePublicEVP(void);
		void			setRemotePublicKeyEVP(EVP_PKEY *remoteEVP_PKEY);

		EVP_CIPHER_CTX	*getRsaEncryptCTX(void);
		EVP_CIPHER_CTX	*getRsaDecryptCTX(void);

		EVP_PKEY		*readx509Certificate(const char *certfile);
		EVP_PKEY		*ReadPrivateKey(const char *keyfile);

		//writekeytofile ?

//#else
		/* AES */
		int AESEncrypt(const unsigned char *message, size_t messageLength, unsigned char **encryptedMessage);
    	int AESDecrypt(unsigned char *encryptedMessage, size_t encryptedMessageLength, unsigned char **decryptedMessage);

		EVP_CIPHER_CTX	*getAesEncryptCTX(void);
		EVP_CIPHER_CTX	*getAesDecryptCTX(void);
		unsigned char	*getAesKey(void);
    	unsigned char	*getAesIv(void);

//#endif //USE_RSA
		
};

std::ostream & operator<<(std::ostream &out, Cryptograph const &in);

#endif  //!__CRYPTOGRAPH__H__