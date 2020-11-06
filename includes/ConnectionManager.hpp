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

#define MAX_CLIENTS 		3
#define MAX_SELECT_FDS		16
#define LST_PORT			4242
#define GENERIC_BUFFER_SIZE	4096
#define INPUT_BUFFER_SIZE	512
#define SHELL_SPAWN_ERROR	-42

#define	QUIT_CMD			"quit"
#define EXIT_CMD			"exit"
#define SHELL_CMD			"shell"
#define RST_CMD				"rst"
#define SYNACK_CMD 			"synack"
#define DISCONNECT_CMD 		"disconnect"
#define EXEC_ERROR_CMD 		"exec_error"
#define CONFIRM_SHELL 		"Spawned shell.\n"
#define DENY_SHELL			"shell_error"

#define EXEC_FILE			"/tmp/matt.ex"
#define EXEC_CMD			" > /tmp/matt.ex"

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
		ConnectionManager(Tintin_reporter *logger);
    	~ConnectionManager(void);
		ConnectionManager &operator=(ConnectionManager const &rhs);

		bool	initSocket(void);
		void	handleIncoming(void);
		pid_t	popShell(int filedesc);

		//void 	testCrypto(Cryptograph cg);
};

std::ostream & operator<<(std::ostream &out, ConnectionManager const &in);

#endif  //!__CONNECTIONMANAGER__H__