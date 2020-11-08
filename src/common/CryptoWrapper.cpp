#include "CryptoWrapper.hpp"

extern char	*__progname;

CryptoWrapper::CryptoWrapper(void)
{
	return ;
}

CryptoWrapper::CryptoWrapper(Cryptograph &cg) : _cryptograph(cg)
{
	mkdir(PUBKEY_FILE_PATH, 0744);
	return ;
}

CryptoWrapper::~CryptoWrapper(void)
{
	
#ifdef USE_RSA
	EVP_PKEY_free(_cryptograph.getLocalKeypairEVP());
	EVP_PKEY_free(_cryptograph.getRemotePublicEVP());

	// Local key pair?

  	EVP_CIPHER_CTX_free(_cryptograph.getRsaEncryptCTX());
  	EVP_CIPHER_CTX_free(_cryptograph.getRsaDecryptCTX());
#else
	EVP_CIPHER_CTX_free(_cryptograph.getAesEncryptCTX());
    EVP_CIPHER_CTX_free(_cryptograph.getAesDecryptCTX());

    free(_cryptograph.getAesKey());
    free(_cryptograph.getAesIv());
#endif
	return ;
}

char		*CryptoWrapper::readPEMFile(FILE *pemFile) {
	long	fsize = 0;
	char	*pemBuffer = NULL;

	fseek(pemFile, 0, SEEK_END);
	fsize = ftell(pemFile);
	fseek(pemFile, 0, SEEK_SET);

	pemBuffer = (char *)malloc(fsize + 1);
	fread(pemBuffer, 1, fsize, pemFile);

	pemBuffer[fsize] = 0;

	return pemBuffer;
}

int	CryptoWrapper::sendLocalPublicKey(int sockfd) {

	size_t 	sentBytes;
	FILE 	*fileptr = NULL;
	char	*PEMFile = NULL;

	std::string progName = __progname;
	std::string pemFile  = PUBKEY_FILE_PATH + progName + "_local.public.pem";

	if ((fileptr = fopen(pemFile.c_str(), "w+")) == NULL)
    {
        printf("Unable to create file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

	if (PEM_write_PUBKEY(fileptr, _cryptograph.getLocalKeypairEVP()) == 0) {
		printf("Failed to write pemfile: %s\n", strerror(errno));
	}

	fflush(fileptr);
	PEMFile = readPEMFile(fileptr);
	fclose(fileptr);

	if ((sentBytes = send(sockfd, PEMFile, strlen(PEMFile), 0)) <= 0) {
		printf("Failed to send pubkey: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	free(PEMFile);

	return 0;
}

int CryptoWrapper::receiveRemotePublicKey(int sockfd) {

	size_t		receivedBytes;
	FILE 		*fileptr = NULL;
	char 		pemBuffer[PEM_BUFFER];
	EVP_PKEY 	*remotePubKey = NULL;

	bzero(pemBuffer, PEM_BUFFER);
	if ((receivedBytes = recv(sockfd, pemBuffer, PEM_BUFFER - 1, 0)) <= 0) {
		printf("Failed to recv pubkey: %s\n", strerror(errno));
		exit(1);
	}

	std::string progName = __progname;
	std::string pemFile  = PUBKEY_FILE_PATH + progName + "_remote.public.pem";

	if ((fileptr = fopen(pemFile.c_str(), "w+")) == NULL) {
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

	if (fputs(pemBuffer, fileptr) <= 0) {
		printf("Unable to write pemBuffer to file: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	fflush(fileptr);
	fseek(fileptr, 0, SEEK_SET);

	if (PEM_read_PUBKEY(fileptr, &remotePubKey, NULL, 0) == NULL) {
		printf("Failed to read remote pubkey.\n");
		ERR_print_errors_fp(stderr);
		exit(1);
	}

	fclose(fileptr);

	_cryptograph.setRemotePublicKeyEVP(remotePubKey);

	return (0);
}

int	CryptoWrapper::sendEncrypted(int sockfd, const void *buf, size_t len) {
	int 			encryptedMessageLength = 0;
	unsigned char 	*encryptedMessage = NULL;
	size_t			sentBytes;

#ifdef USE_RSA

#else
	/* Encrypt message with the cryptograph */
	encryptedMessageLength = _cryptograph.AESEncrypt((const unsigned char*)buf, len + 1, &encryptedMessage);
	if (encryptedMessageLength == -1) {
		printf("Error encrypting message\n");
		exit (-1);
	}

	/* Send the encrypted message */
	sentBytes = send(sockfd, encryptedMessage, encryptedMessageLength, 0);
	if (sentBytes <= 0)
		return -1;

	free(encryptedMessage);
#endif
	return encryptedMessageLength;
}

int CryptoWrapper::recvEncrypted(int sockfd, char **decrypt_buffer) {
	int 			decryptedMessageLength = 0;
	char			encryptedMessageBuffer[GENERIC_BUFFER_SIZE]; // allocate those
	size_t			receivedBytes;

#ifdef USE_RSA

#else
	/* Receive the encrypted message */
	bzero(encryptedMessageBuffer, GENERIC_BUFFER_SIZE);
	receivedBytes = recv(sockfd, encryptedMessageBuffer, GENERIC_BUFFER_SIZE - 1, 0);
	if (receivedBytes <= 0) 
		return receivedBytes;

	/* Decrypt the message with the cryptograph */
	decryptedMessageLength = _cryptograph.AESDecrypt((unsigned char*)encryptedMessageBuffer,
														receivedBytes, (unsigned char **)decrypt_buffer);
	if (decryptedMessageLength == -1) {
		printf("Error decrypting message\n");
		exit (-1);
	}
#endif
	return decryptedMessageLength;
}


CryptoWrapper &CryptoWrapper::operator=(const CryptoWrapper & rhs)
{
    if (this != &rhs)
        *this = rhs;
    return *this;
}

std::ostream &operator<<(std::ostream &out, CryptoWrapper const & pm)
{
    (void)pm;
    out << "CryptoWrapper" << std::endl;
    return out;
}