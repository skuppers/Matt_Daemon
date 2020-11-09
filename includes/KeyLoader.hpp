#ifndef __KEYLOADER__H__
#define __KEYLOADER__H__

#include <string.h>
#include <iostream>
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
		KeyLoader &operator=(KeyLoader const &rhs);

		EVP_PKEY 	*readx509Certificate(const char *certfile);
		EVP_PKEY 	*ReadPrivateKey(const char *keyfile);
		int 		CertificateToStr(FILE *pemFile, char **pemBuffer);

};

std::ostream & operator<<(std::ostream &out, KeyLoader const &in);

#endif  //!__KEYLOADER__H__