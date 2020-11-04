#ifndef __CONNECTIONMANAGER__H__
#define __CONNECTIONMANAGER__H__

#include "Tintin_reporter.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENTS 	3

class ConnectionManager
{
	private:
		Tintin_reporter		*_logger;
    	int					_activeClients;

		int					_listeningSocket;
		struct sockaddr_in 	_sin;

	public:
    	ConnectionManager(void);
		ConnectionManager(Tintin_reporter	*logger);
    	~ConnectionManager(void);

		void	initSocket();
		void	handleIncoming();


		ConnectionManager &operator=(ConnectionManager const &rhs);
};

std::ostream & operator<<(std::ostream &out, ConnectionManager const &in);

#endif  //!__CONNECTIONMANAGER__H__