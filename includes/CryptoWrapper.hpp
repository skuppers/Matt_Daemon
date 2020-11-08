#ifndef __CRYPTOWRAPPER__H__
#define __CRYPTOWRAPPER__H__

#include "Cryptograph.hpp"
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#define CONNECTION_BUFFER_SIZE	32
#define INPUT_BUFFER_SIZE		512
#define GENERIC_BUFFER_SIZE		4096
#define PEM_BUFFER				512

#define VALIDATE_AUTHENTICATION "VALIDATED"
#define DENY_AUTHENTICATION 	"DENY_GTFO"
#define AUTH_CONFIRM_LENGTH		10

#define PUBKEY_FILE_PATH		"/var/run/matt_daemon/"

class CryptoWrapper
{
	private:
		Cryptograph _cryptograph;
		char		*readPEMFile(FILE *pemFile);
	public:
		CryptoWrapper(void);
		CryptoWrapper(Cryptograph &cg);
		~CryptoWrapper();
		CryptoWrapper &operator=(CryptoWrapper const &rhs);

		int		sendEncrypted(int sockfd, const void *buf, size_t len);
		int		recvEncrypted(int sockfd, char **decrypt_buffer);

		int		sendLocalPublicKey(int sockfd);
		int		receiveRemotePublicKey(int sockfd);
};

std::ostream & operator<<(std::ostream &out, CryptoWrapper const &in);

#endif  //!__CRYPTOWRAPPER__H__