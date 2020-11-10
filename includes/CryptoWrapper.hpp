#ifndef __CRYPTOWRAPPER__H__
#define __CRYPTOWRAPPER__H__

#include "Cryptograph.hpp"
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "KeyLoader.hpp"

#define CONNECTION_BUFFER_SIZE	32
#define INPUT_BUFFER_SIZE		512
#define GENERIC_BUFFER_SIZE		8192
#define CERT_BUFFER				4096

#define VALIDATE_AUTHENTICATION "VALIDATED"
#define DENY_AUTHENTICATION 	"DENY_GTFO"
#define AUTH_CONFIRM_LENGTH		10

class CryptoWrapper
{
	private:
		Cryptograph *_cryptograph;
		KeyLoader	*_keyLoader;
		
	public:
		CryptoWrapper(void);
		~CryptoWrapper(void);
		CryptoWrapper &operator=(CryptoWrapper const &rhs);

		int		sendEncrypted(int sockfd, const void *buf, size_t len);
		int		recvEncrypted(int sockfd, char **decrypt_buffer);

		int		sendLocalCertificate(int sockfd);
		int		receiveRemoteCertificate(int sockfd);
};

std::ostream & operator<<(std::ostream &out, CryptoWrapper const &in);

#endif  //!__CRYPTOWRAPPER__H__