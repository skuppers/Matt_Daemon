#ifndef __CONNECTIONMANAGER__H__
#define __CONNECTIONMANAGER__H__

#include "Tintin_reporter.hpp"
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <list>

#define MAX_CLIENTS 	3
#define MAX_SELECT_FDS	16
#define LST_PORT		4242

class ConnectionManager
{
	private:
		Tintin_reporter		*_logger;
    	int					_activeClients;
		int					_listeningSocket;
		struct sockaddr_in 	_sin;
		std::list<int>		_childsPIDs;
		void				handleShellDisconnect(void);
		int					acceptNewClients(void);

	public:
    	ConnectionManager(void);
		ConnectionManager(Tintin_reporter	*logger);
    	~ConnectionManager(void);

		bool	initSocket(void);
		void	handleIncoming(void);
		pid_t	popShell(int filedesc);
		ConnectionManager &operator=(ConnectionManager const &rhs);
};

std::ostream & operator<<(std::ostream &out, ConnectionManager const &in);

#endif  //!__CONNECTIONMANAGER__H__