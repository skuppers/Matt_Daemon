#include "KeyLoader.hpp"

KeyLoader::KeyLoader(void)
{
    return ;
}

KeyLoader::~KeyLoader(void)
{
	return ;
}

EVP_PKEY 	*KeyLoader::readx509Certificate(const char *certfile) {

	X509 		*x509;
	EVP_PKEY	*pkey;
	FILE 		*fp;
	
	if (!(fp = fopen(certfile, "r"))) {
		std::cerr << "Error opening certificate: " << certfile << ": " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

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

/* Opens the specified file and extract the private key from it */
EVP_PKEY 	*KeyLoader::ReadPrivateKey(const char *keyfile) {

	EVP_PKEY	*pkey;
	FILE 		*fp;
	
	if (!(fp = fopen(keyfile, "r")))
		return NULL;

	/* OpenSSL PEM call, extract the privatekey from the specified file */
	pkey = PEM_read_PrivateKey(fp, NULL, 0, NULL);

	fclose (fp);

  	if (pkey == NULL) {
		ERR_print_errors_fp(stderr);
		return NULL;
	}

	return pkey;
}

/* Take in a FILE pointer and returns the content of the file as an allocated string */
int         KeyLoader::CertificateToStr(FILE *pemFile, char **pemBuffer) {
	long	fsize = 0;
    size_t  readBytes;

	if (fseek(pemFile, 0, SEEK_END) != 0)
		return -1;
	fsize = ftell(pemFile);
	if (fseek(pemFile, 0, SEEK_SET) != 0)
		return -1;

	if ((*pemBuffer = (char *)malloc(fsize + 1)) == NULL)
		return -1;
    bzero(*pemBuffer, fsize + 1);
        
    if ((readBytes = fread(*pemBuffer, 1, fsize, pemFile)) <= 0) {
		free(*pemBuffer);
		return -1;
	}

	return readBytes;
}

KeyLoader &KeyLoader::operator=(const KeyLoader & rhs)
{
	if (this != &rhs)
		*this = rhs;
	return *this;
}

std::ostream &operator<<(std::ostream &out, KeyLoader const & pm)
{
	(void)pm;
	out << "KeyLoader" << std::endl;
	return out;
}