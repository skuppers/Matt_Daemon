#ifndef __CRYPTOGRAPH__H__
#define __CRYPTOGRAPH__H__

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <iostream>

class Cryptograph
{
	private:
		/* AES */
/*		EVP_CIPHER_CTX *aesEncryptContext;
		EVP_CIPHER_CTX *aesDecryptContext;
		unsigned char *aesKey;
    	unsigned char *aesIv;
		size_t aesKeyLength;
    	size_t aesIvLength;
		int init();
    	int generateAesKey(unsigned char **aesKey, unsigned char **aesIv); */

		/* RSA */
	public:
		Cryptograph(void);
	//	Cryptograph(int foobar);
		~Cryptograph(void);
		Cryptograph &operator=(Cryptograph const &rhs);


		/* AES */
/*		int aesEncrypt(const unsigned char *message, size_t messageLength, unsigned char **encryptedMessage);
    	int aesDecrypt(unsigned char *encryptedMessage, size_t encryptedMessageLength, unsigned char **decryptedMessage);
		int getAesKey(unsigned char **aesKey);
    	int setAesKey(unsigned char *aesKey, size_t aesKeyLen);
    	int getAesIv(unsigned char **aesIv);
    	int setAesIv(unsigned char *aesIv, size_t aesIvLen);
*/


		/* Communcation wrappers */
/*		int sendCrypted();
		int recvCrypted(); */
};

std::ostream & operator<<(std::ostream &out, Cryptograph const &in);

#endif  //!__CRYPTOGRAPH__H__