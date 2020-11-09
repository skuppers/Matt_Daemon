#ifndef __KEYLOADER__H__
#define __KEYLOADER__H__

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>

class KeyLoader
{
    private:

    public:
        KeyLoader(void);
    	~KeyLoader();

		EVP_PKEY 	*readx509Certificate(const char *certfile);
		EVP_PKEY 	*ReadPrivateKey(const char *keyfile);

		int 		PEMFileToStr(FILE *pemFile, char **pemBuffer);

};


#endif  //!__KEYLOADER__H__