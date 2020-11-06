#ifndef __CRYPTOWRAPPER__H__
#define __CRYPTOWRAPPER__H__

#include "Cryptograph.hpp"
#include <sys/socket.h>
#include <sys/types.h>

#define GENERIC_BUFFER_SIZE	4096

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
		int		recvEncrypted(int sockfd, void *buf, size_t len);
};

std::ostream & operator<<(std::ostream &out, CryptoWrapper const &in);

#endif  //!__CRYPTOWRAPPER__H__