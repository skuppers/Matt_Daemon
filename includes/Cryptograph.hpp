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

#define AES_ROUNDS 6

class Cryptograph
{
	private:
		/* AES */
		EVP_CIPHER_CTX 	*_aesEncryptContext;
		EVP_CIPHER_CTX 	*_aesDecryptContext;

		unsigned char 	*_aesKey;
    	unsigned char 	*_aesIv;

		size_t 			_aesKeyLength;
    	size_t 			_aesIvLength;
		
		int 	initAES(void);
    	int 	generateAesKey(unsigned char **aesKey, unsigned char **aesIv);
/*		int		AESEncrypt(const unsigned char *message, size_t messageLength, unsigned char **encryptedMessage);
		int		AESDecrypt(unsigned char *encryptedMessage, size_t encryptedMessageLength, unsigned char **decryptedMessage);
*/
		/* RSA */
//		int initRSA();


		/* General */
		bool	_useRSA;

		int init(void);
		int deInit(void);

	public:
		Cryptograph(void);
		Cryptograph(bool rsa);
		~Cryptograph(void);
		Cryptograph &operator=(Cryptograph const &rhs);

		/* AES */
		int AESEncrypt(const unsigned char *message, size_t messageLength, unsigned char **encryptedMessage);
    	int AESDecrypt(unsigned char *encryptedMessage, size_t encryptedMessageLength, unsigned char **decryptedMessage);
/*		int getAesKey(unsigned char **aesKey);
    	int setAesKey(unsigned char *aesKey, size_t aesKeyLen);
    	int getAesIv(unsigned char **aesIv);
    	int setAesIv(unsigned char *aesIv, size_t aesIvLen);*/



		/* Communcation wrappers */
/*		int sendCrypted();
		int recvCrypted(); */
};

std::ostream & operator<<(std::ostream &out, Cryptograph const &in);

#endif  //!__CRYPTOGRAPH__H__