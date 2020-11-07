#ifndef __CRYPTOWRAPPER__H__
#define __CRYPTOWRAPPER__H__

#include "Cryptograph.hpp"
#include <sys/socket.h>
#include <sys/types.h>

#define CONNECTION_BUFFER_SIZE	32
#define INPUT_BUFFER_SIZE		512
#define GENERIC_BUFFER_SIZE		4096
#define VALIDATE_AUTHENTICATION "VALIDATED"
#define DENY_AUTHENTICATION 	"DENY_GTFO"
#define AUTH_CONFIRM_LENGTH		10

class CryptoWrapper
{
	private:
		Cryptograph _cryptograph;
	public:
		CryptoWrapper(void);
		CryptoWrapper(Cryptograph &cg);
		~CryptoWrapper();
		CryptoWrapper &operator=(CryptoWrapper const &rhs);

		int		sendEncrypted(int sockfd, const void *buf, size_t len);
		int		recvEncrypted(int sockfd, char **decrypt_buffer);
};

std::ostream & operator<<(std::ostream &out, CryptoWrapper const &in);

#endif  //!__CRYPTOWRAPPER__H__