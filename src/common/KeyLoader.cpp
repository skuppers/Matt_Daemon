#include "KeyLoader.hpp"

KeyLoader::KeyLoader(void)
{
    return ;
}

KeyLoader::~KeyLoader()
{
}

EVP_PKEY 	*KeyLoader::readx509Certificate(const char *certfile) {

	X509 		*x509;
	EVP_PKEY	*pkey;
	FILE 		*fp;
	
	if (!(fp = fopen(certfile, "r")))
		return NULL;

	x509 = PEM_read_X509(fp, NULL, 0, NULL);

	fclose(fp);

	if (x509 == NULL)
	{
		ERR_print_errors_fp(stderr);
		return NULL;   
	}

	if ((pkey = X509_extract_key(x509)) == NULL) 
		ERR_print_errors_fp(stderr);

	X509_free(x509);

	return pkey;
}

EVP_PKEY 	*KeyLoader::ReadPrivateKey(const char *keyfile) {

	EVP_PKEY	*pkey;
	FILE 		*fp;
	
	if (!(fp = fopen(keyfile, "r")))
		return NULL;

	pkey = PEM_read_PrivateKey(fp, NULL, 0, NULL);

	fclose (fp);

  	if (pkey == NULL) 
		ERR_print_errors_fp (stderr);   

	return pkey;
}
#include <iostream>
#include <unistd.h>
#include <string.h>
int         KeyLoader::PEMFileToStr(FILE *pemFile, char **pemBuffer) {
	long	fsize = 0;
    size_t  readBytes;

	fseek(pemFile, 0, SEEK_END);
	fsize = ftell(pemFile);
	fseek(pemFile, 0, SEEK_SET);

    std::cout << "Allocating " << fsize + 1 << " bytes" << std::endl;
	*pemBuffer = (char *)malloc(fsize + 16);
    bzero(*pemBuffer, fsize + 16);
    if (*pemBuffer == NULL)
        return -1;

    write(STDOUT_FILENO, "Check", 5);

	readBytes = fread(*pemBuffer, 1, fsize, pemFile);


	return readBytes;
}