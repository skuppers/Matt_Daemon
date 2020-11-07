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

	encryptedMessageLength = _cryptograph.AESEncrypt((const unsigned char*)buf, len + 1, &encryptedMessage);
	if (encryptedMessageLength == -1) {
		printf("Error encrypting message\n");
		exit (-1);
	}

/*	printf("Encrypted message length: %d\n", encryptedMessageLength);
	printf("\nCiphertext:");
	for(int i = 0; i < encryptedMessageLength; i++) //variable len is length of ciphertext memorized after encryption.
	{printf("%02X ",encryptedMessage[i]);}
	printf("\n");
*/
	sentBytes = send(sockfd, encryptedMessage, encryptedMessageLength, 0);
	if (sentBytes <= 0) {
		printf("Error sending data.");
		exit(-1);
	}

//	printf("Sent bytes: %lu\n", sentBytes);

	return encryptedMessageLength;
}

#include <unistd.h>
int CryptoWrapper::recvEncrypted(int sockfd, char **decrypt_buffer, size_t len) {
	int 			decryptedMessageLength;
	char			encryptedMessageBuffer[len];
	size_t			receivedBytes;

	bzero(encryptedMessageBuffer, len);
	receivedBytes = recv(sockfd, encryptedMessageBuffer, len - 1, 0);
	if (receivedBytes <= 0) {
		printf("Error receiving data.\n");
		exit(-1);
	}

/*	printf("Received %lu encrypted bytes\n", receivedBytes);
	printf("\nCiphertext:");
	for(int i = 0; i < receivedBytes; i++) //variable len is length of ciphertext memorized after encryption.
	{printf("%02X ",encryptedMessageBuffer[i]);}
	printf("\n");
*/

	decryptedMessageLength = _cryptograph.AESDecrypt((unsigned char*)encryptedMessageBuffer,
														receivedBytes, (unsigned char **)decrypt_buffer);
	if (decryptedMessageLength == -1) {
		printf("Error decrypting message\n");
		exit (-1);
	}

//	printf("Decrypted message length: %d\n", decryptedMessageLength);

//	printf("Decrypted message: |%s|\n", (char*)*decrypt_buffer);

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