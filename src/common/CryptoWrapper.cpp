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
	return ;
}

int	CryptoWrapper::sendEncrypted(int sockfd, const void *buf, size_t len) {
	int 			encryptedMessageLength;
	unsigned char 	*encryptedMessage = NULL;
	size_t			sentBytes;

	encryptedMessageLength = _cryptograph.AESEncrypt((const unsigned char*)buf, len, &encryptedMessage);
	if (encryptedMessageLength == -1) {
		printf("Error encrypting message\n");
		exit (-1);
	}
	sentBytes = send(sockfd, encryptedMessage, encryptedMessageLength, 0);
	if (sentBytes <= 0) {
		printf("Error sending data.");
		exit(-1);
	}
	return encryptedMessageLength;
}
#include <unistd.h>
int CryptoWrapper::recvEncrypted(int sockfd, void *buf, size_t len) {
	int 			decryptedMessageLength;
	char			encryptedMessageBuffer[len];
	size_t			receivedBytes;

	bzero(encryptedMessageBuffer, len);
	receivedBytes = recv(sockfd, encryptedMessageBuffer, len - 1, 0);
	if (receivedBytes <= 0) {
		printf("Error receiving data.\n");
		exit(-1);
	}
	printf("Received %lu bytes\n", receivedBytes);

	/* Possible overflow when decrypting it ? */
	decryptedMessageLength = _cryptograph.AESDecrypt((unsigned char*)encryptedMessageBuffer,
														receivedBytes, (unsigned char **)&buf);
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