#ifndef __CONNECTIONMANAGER__H__
#define __CONNECTIONMANAGER__H__

#include "Tintin_reporter.hpp"
#include "Cryptograph.hpp"
#include "CryptoWrapper.hpp"
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

#define MAX_CLIENTS 		3
#define MAX_SELECT_FDS		16
#define LST_PORT			4242
#define SHELL_SPAWN_ERROR	-42

#define	QUIT_CMD			"quit"
#define EXIT_CMD			"exit"
#define SHELL_CMD			"system"
#define RST_CMD				"rst"
#define SYNACK_CMD 			"synack"
#define DISCONNECT_CMD 		"disconnect"
#define EXEC_ERROR_CMD 		"exec_error"
#define CONFIRM_SHELL 		"Spawned shell.\n"
#define DENY_SHELL			"shell_error"

#define EXEC_FILE			"/tmp/matt.ex"
#define EXEC_CMD			" > /tmp/matt.ex "

#define USERNAME_STR		"Username: "
#define PASSWORD_STR		"Password: "

#define SERVER_BANNER		"\n#-----#-----#-----#-----#-----#-----#\n|                                   |\n|      Welcome to Matt Daemon       |\n|   Beware, your input is logged ;) |\n|                                   |\n#-----#-----#-----#-----#-----#-----#\n"

class ConnectionManager
{
	private:
		Tintin_reporter		*_logger;
		CryptoWrapper		*_cryptoWrapper;
    	int					_activeClients;
		int					_listeningSocket;
		struct sockaddr_in 	_sin;
		std::list<int>		_childsPIDs;
		void				handleShellDisconnect(void);
		int					acceptNewClients(void);
		bool				authClient(int clientFD, char **name);
	public:
    	ConnectionManager(void);
		ConnectionManager(Tintin_reporter *logger, CryptoWrapper *cw);
    	~ConnectionManager(void);
		ConnectionManager &operator=(ConnectionManager const &rhs);

		bool	initSocket(void);
		void	handleIncoming(void);
		pid_t	popShell(int filedesc);
};

std::ostream & operator<<(std::ostream &out, ConnectionManager const &in);

#endif  //!__CONNECTIONMANAGER__H__