#include "KeyLoader.hpp"

KeyLoader::KeyLoader(void)
{
    return ;
}

KeyLoader::~KeyLoader(void)
{
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

EVP_PKEY 	*KeyLoader::ReadPrivateKey(const char *keyfile) {

	EVP_PKEY	*pkey;
	FILE 		*fp;
	
	if (!(fp = fopen(keyfile, "r"))){
		std::cerr << "Error opening private key: " << keyfile << ": " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	pkey = PEM_read_PrivateKey(fp, NULL, 0, NULL);

	fclose (fp);

  	if (pkey == NULL) 
		ERR_print_errors_fp (stderr);   

	return pkey;
}

//TODO Error handling + memleaks of the pemBuffer
int         KeyLoader::CertificateToStr(FILE *pemFile, char **pemBuffer) {
	long	fsize = 0;
    size_t  readBytes;

	fseek(pemFile, 0, SEEK_END);
	fsize = ftell(pemFile);
	fseek(pemFile, 0, SEEK_SET);

	*pemBuffer = (char *)malloc(fsize + 1);
    bzero(*pemBuffer, fsize + 1);

    if (*pemBuffer == NULL)
        return -1;
        
    readBytes = fread(*pemBuffer, 1, fsize, pemFile);

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