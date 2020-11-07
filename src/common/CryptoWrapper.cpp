#include "CryptoWrapper.hpp"

CryptoWrapper::CryptoWrapper(void)
{
	_cryptograph = NULL;
	return ;
}

CryptoWrapper::CryptoWrapper(Cryptograph &cg) : _cryptograph(cg)
{

	return ;
}

CryptoWrapper::~CryptoWrapper(void)
{
	EVP_CIPHER_CTX_free(_cryptograph.getEncryptCTX());
    EVP_CIPHER_CTX_free(_cryptograph.getDecryptCTX());

    free(_cryptograph.getAesKey());
    free(_cryptograph.getAesIv());
	return ;
}

int	CryptoWrapper::sendEncrypted(int sockfd, const void *buf, size_t len) {
	int 			encryptedMessageLength;
	unsigned char 	*encryptedMessage = NULL;
	size_t			sentBytes;

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

	return encryptedMessageLength;
}

int CryptoWrapper::recvEncrypted(int sockfd, char **decrypt_buffer) {
	int 			decryptedMessageLength;
	char			encryptedMessageBuffer[GENERIC_BUFFER_SIZE]; // allocate those
	size_t			receivedBytes;

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