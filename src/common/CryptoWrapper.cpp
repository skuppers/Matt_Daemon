#include "CryptoWrapper.hpp"

extern char	*__progname;

CryptoWrapper::CryptoWrapper(void)
{
	_cryptograph = new Cryptograph();
	_keyLoader = new KeyLoader();
	return ;
}

CryptoWrapper::~CryptoWrapper(void)
{
#ifdef USE_RSA
	EVP_PKEY_free(_cryptograph->getLocalKeypairEVP());
	EVP_PKEY_free(_cryptograph->getRemotePublicEVP());

  	EVP_CIPHER_CTX_free(_cryptograph->getRsaEncryptCTX());
  	EVP_CIPHER_CTX_free(_cryptograph->getRsaDecryptCTX());
#else
	EVP_CIPHER_CTX_free(_cryptograph->getAesEncryptCTX());
	EVP_CIPHER_CTX_free(_cryptograph->getAesDecryptCTX());

	free(_cryptograph->getAesKey());
	free(_cryptograph->getAesIv());
#endif
	return ;
}

#ifdef USE_RSA

int		CryptoWrapper::sendLocalCertificate(int sockfd) {

	int			readBytes;
	int 		sentBytes;

	FILE 		*fileptr 				= NULL;
	char 		*certificateFileName 	= NULL;
	char		*certificateFileContent	= NULL;


	/* Define the path for reading the local certificate		*/
	/* The name will be defined by whoever uses this function	*/
	/* Either the client or the server							*/
	if (strncmp(__progname, CLIENT_NAME, strlen(CLIENT_NAME)) == 0) {
		certificateFileName = (char*)CLIENT_CERT;
	} else if (strncmp(__progname, SERVER_NAME, strlen(SERVER_NAME)) == 0) {
		certificateFileName = (char*)SERVER_CERT;
	} else {
		std::cerr << "The local certificate file name could not be determined!" << std::endl;
		return -1;
	}

	/* Open the previously defined file, in read mode	*/
	/* for reading the local certificate 				*/
	if ((fileptr = fopen(certificateFileName, "r+")) == NULL) {
		std::cerr << "Unable to open the local certificate file: " << strerror(errno) << std::endl;
		return -1;
	}

	/* Read the content of the file and store it	*/
	/* into the 'certificateFileContent' buffer		*/
	readBytes = _keyLoader->CertificateToStr(fileptr, &certificateFileContent);

	/* Close the file */
	fclose(fileptr);

	/* Handle reading error */
	if (readBytes <= 0) {
		std::cerr << "Unable to read the local certificate file: " << strerror(errno) << std::endl;
		return -1;
	}

	/* Send over the local certificate */
	if ((sentBytes = send(sockfd, certificateFileContent, readBytes, 0)) <= 0) {
		std::cerr << "Failed to send the local certificate: " << strerror(errno) << std::endl;
		return -1;
	}

	/* Check for partial send */
	if (sentBytes != readBytes) {
		std::cerr << "Failed to send the entire local certificate!" << std::endl;
		return -1;
	}

	return 0;
}

int 	CryptoWrapper::receiveRemoteCertificate(int sockfd) {

	int				receivedBytes;
	std::string 	remoteCertificateName;
	char 			certRecvBuffer[CERT_BUFFER];
	
	FILE 			*fileptr = NULL;
	EVP_PKEY 		*remoteCertificate = NULL;

	/* Receiving the remote certificate and store it into the 'certRecvBuffer' buffer */
	bzero(certRecvBuffer, CERT_BUFFER);
	if ((receivedBytes = recv(sockfd, certRecvBuffer, CERT_BUFFER - 1, 0)) <= 0) {
		std::cerr << "Failed to receive the remote certificate: " << strerror(errno) << std::endl;
		return -1;
	}

	/* Define the path for storing the remote certificate		*/
	/* The name will be defined by whoever uses this function	*/
	/* Either the client or the server							*/
	if (strncmp(__progname, CLIENT_NAME, strlen(CLIENT_NAME)) == 0) {
		remoteCertificateName = CLIENT_CERT + std::string(".remote");
	} else if (strncmp(__progname, SERVER_NAME, strlen(SERVER_NAME)) == 0) {
		remoteCertificateName = SERVER_CERT + std::string(".remote");
	} else {
		std::cerr << "The remote certificate file name could not be determined!" << std::endl;
		return -1;
	}

	/* Create/open the previously defined file, in write mode	*/
	/* for storing the remote certificate 						*/
	if ((fileptr = fopen(remoteCertificateName.c_str(), "w+")) == NULL) {
		std::cerr << "Unable to create the remote certificate file." << std::endl;
		return -1;
	}

	/* Write the content of the 'certRecvBuffer' into the file */
	if (fputs(certRecvBuffer, fileptr) <= 0) {
		std::cerr << "Unable to write the remote certificate to file: " << strerror(errno) << std::endl;
		return -1;
	}

	/* Flush and close the file */
	fflush(fileptr);
	fclose(fileptr);

	/* Extract the EVP_PKEY from the certificate file */
	remoteCertificate = _keyLoader->readx509Certificate(remoteCertificateName.c_str());
	if (remoteCertificate == NULL) {
		std::cerr << "Failed to read the remote certificate file." << std::endl;
		return -1;
	}

	/* Save the EBP_PKEY into a private variable for later use */
	_cryptograph->setRemotePublicKeyEVP(remoteCertificate);

	return (0);
}

#endif

int		CryptoWrapper::sendEncrypted(int sockfd, const void *buf, size_t len) {
	int				sentBytes;
	int 			encryptedMessageLength = 0;
	unsigned char 	*encryptedMessage = NULL;
	
	/* Encrypt message with the cryptograph */
#ifdef USE_RSA
	encryptedMessageLength = _cryptograph->RSAEncrypt((const unsigned char*)buf, len, &encryptedMessage);
#else
	encryptedMessageLength = _cryptograph->AESEncrypt((const unsigned char*)buf, len + 1, &encryptedMessage);
#endif

	if (encryptedMessageLength == -1) {
		printf("Error encrypting message\n");
		return -1;
	}

	/* Send the encrypted message */
	sentBytes = send(sockfd, encryptedMessage, encryptedMessageLength, 0);

	free(encryptedMessage);

	if (sentBytes <= 0) {
		printf("Error sending message\n");
		return -1;
	}

	return encryptedMessageLength;
}

int 	CryptoWrapper::recvEncrypted(int sockfd, char **decrypt_buffer) {
	int				receivedBytes;
	int 			decryptedMessageLength = 0;
	char			encryptedMessageBuffer[GENERIC_BUFFER_SIZE];
	
	bzero(encryptedMessageBuffer, GENERIC_BUFFER_SIZE);

	/* Receive the encrypted message */
	receivedBytes = recv(sockfd, encryptedMessageBuffer, GENERIC_BUFFER_SIZE - 1, 0);

	if (receivedBytes <= 0) 
		return receivedBytes;

	/* Decrypt the message with the cryptograph */
#ifdef USE_RSA
	decryptedMessageLength = _cryptograph->RSADecrypt((unsigned char*)encryptedMessageBuffer, receivedBytes, (unsigned char **)decrypt_buffer);
#else
	decryptedMessageLength = _cryptograph->AESDecrypt((unsigned char*)encryptedMessageBuffer, receivedBytes, (unsigned char **)decrypt_buffer);
#endif

	if (decryptedMessageLength == -1) {
		printf("Error decrypting message\n");
		return -1;
	}
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