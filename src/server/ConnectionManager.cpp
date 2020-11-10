#include "ConnectionManager.hpp"
#include "general.hpp"

//bool			g_signalTerminate;

ConnectionManager::ConnectionManager(void)
{
	return ;
}

ConnectionManager::ConnectionManager(Tintin_reporter *logger, CryptoWrapper *cw) : _logger(logger), _cryptoWrapper(cw)
{
	_activeClients = 0;
	_listeningSocket = -1;
	return ;
}

ConnectionManager::~ConnectionManager(void)
{
	// destruct/unbind/unlink socket & stuff
	close(_listeningSocket);
	return ;
}


bool    ConnectionManager::initSocket(void) {
	int     sockfd;
	int     one = 1;
	
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0
		|| setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
		_logger->log(LOGLVL_ERROR, "Could not create socket. Quitting");
		return false;
	}

	_listeningSocket = sockfd;
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(LST_PORT);
	_sin.sin_addr.s_addr = INADDR_ANY;
	memset(&_sin.sin_zero, '\0', 8);
	
	if (bind(sockfd, (const struct sockaddr *)&_sin, sizeof(_sin)) < 0) {
		_logger->log(LOGLVL_ERROR, "Could not bind socket. Quitting");
		return false;
	}

	if (listen(sockfd, MAX_CLIENTS) != 0) {
		_logger->log(LOGLVL_ERROR, "Socket could not enable listening. Quitting.");
		return false;
	}
	_logger->log(LOGLVL_INFO, "Socket is online.");
	return true;
}

void    ConnectionManager::handleShellDisconnect(void) {
	int             exited;
	int             status = 0;

	for (std::list<int>::iterator it = _childsPIDs.begin(); it != _childsPIDs.end(); it++)
	{
		exited = waitpid((pid_t)*it, &status, WNOHANG);
		if (exited != -1 && exited != 0 && WIFEXITED(status)) {
			_logger->log(LOGLVL_INFO, "Client closed shell and disconnected.");
			--_activeClients;
		}
	}
}

bool	ConnectionManager::authClient(int clientFD, char **username) {
	char *password = NULL;

	/* Ask for username */
	if (_cryptoWrapper->sendEncrypted(clientFD, USERNAME_STR, strlen(USERNAME_STR)) <= 0)
		return (false);
	/* Receive and store username */
	if (_cryptoWrapper->recvEncrypted(clientFD, username) <= 0)
		return (false);

	/* Ask for password */
	if (_cryptoWrapper->sendEncrypted(clientFD, PASSWORD_STR, strlen(PASSWORD_STR)) <= 0)
		return (false);
	/* Receive and store password */
	if (_cryptoWrapper->recvEncrypted(clientFD, &password) <= 0)
		return (false);

	/* Verify user password with makefile defined password */
#ifdef  AUTH_PASSWORD
	if (strncmp(password, AUTH_PASSWORD, strlen(AUTH_PASSWORD)) == 0) {
		free(password);
		_cryptoWrapper->sendEncrypted(clientFD, VALIDATE_AUTHENTICATION, strlen(VALIDATE_AUTHENTICATION));
		return (true);
	}
#endif
	free(password);
	_cryptoWrapper->sendEncrypted(clientFD, DENY_AUTHENTICATION, strlen(DENY_AUTHENTICATION));
	return false;
}

int     ConnectionManager::acceptNewClients(void) {
	int 		newfd;
	char		*clientName = NULL;
	std::string clientNameString;

	/* Accept TCP connection */
	_logger->log(LOGLVL_INFO, "A new client is connecting.");
	if ((newfd = accept(_listeningSocket, NULL, NULL)) == -1) {
		_logger->log(LOGLVL_ERROR, "Error accepting client connection.");
		return (-1);
	}
	else
	{
		/* RSA public key exchange process	*/
		/*									*/
#ifdef USE_RSA

		if (_cryptoWrapper->receiveRemoteCertificate(newfd) != 0
			|| _cryptoWrapper->sendLocalCertificate(newfd) != 0) {
			_logger->log(LOGLVL_ERROR, "The RSA key exchange proccess failed with the client.");
			return -1;
		}
		_logger->log(LOGLVL_INFO, "The RSA key exchange proccess succeeded.");


		char *msg = "Denis le boss tu connais maggle";

		if (_cryptoWrapper->sendEncrypted(newfd, msg, strlen(msg)) <= 0) {
			std::cout << "Error sending encrypted message!" << std::endl;
		}

	exit(1);
#endif



		/* Ensure that no more than 3 clients can connect simultaneously */
		if (_activeClients >= MAX_CLIENTS) {
			_logger->log(LOGLVL_WARN, "A client tried to connect, but no slot is avaible.");
			_cryptoWrapper->sendEncrypted(newfd, RST_CMD, strlen(RST_CMD));
			close(newfd);
			return (-1);
		}

		/* Confirm connection to client */
		if (_cryptoWrapper->sendEncrypted(newfd, SYNACK_CMD, strlen(SYNACK_CMD)) <= 0) {
			_logger->log(LOGLVL_ERROR, "Client connection could not be confirmed.");
			close(newfd);
			return (-1);
		}

		/* Delay for fixing a weird send/recv & libcrypto bug */
		/* This is probably due to the daemon sending the connection confirmation   */
		/* right after the authentication requests, while the client interprets     */
		/* those two request as a single one, which fucks up the decryption buffer  */
		sleep(1);

		/* Client authentication process */
		_logger->log(LOGLVL_INFO, "Authenticating client.");
		if (!authClient(newfd, &clientName))
		{
			(clientName == NULL) ? clientNameString = std::string("undefined") : clientNameString = std::string(clientName);
			if (clientName != NULL)
				free(clientName);
			_logger->log(LOGLVL_WARN, "Authentication failed for client \'" + clientNameString + "\'. Closing connection.");
			close(newfd);
			return (-1);
		}

		/* Same as above */
		sleep(1);

		/* Log authentication success */
		_logger->log(LOGLVL_INFO, "Client \'" + std::string(clientName) + "\' successfully authenticated.");

	/* TODO client class */
	/* clientname must be at least 1 char long */
	free(clientName);

		/* Send over server banner */
		_cryptoWrapper->sendEncrypted(newfd, SERVER_BANNER, strlen(SERVER_BANNER));
		
		return (newfd);
	}
}

void    ConnectionManager::handleIncoming(void) {
	pid_t			shell_pid;
	int             newfd;
	struct timeval	tv;
	fd_set          recv_set;
	fd_set          master_set;

	FD_ZERO(&recv_set);
	FD_ZERO(&master_set);
	FD_SET(_listeningSocket, &master_set);

	while (!g_signalTerminate)
	{
		
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		recv_set = master_set;
		
		if (select(MAX_SELECT_FDS, &recv_set, NULL, NULL, &tv) == -1) {
			if (errno == EINTR)
				continue ;
			_logger->log(LOGLVL_ERROR, "Select syscall failed. Quitting.");
			return ;
		}

		for (int currentFD = 0; currentFD < MAX_SELECT_FDS; currentFD++) {
			if (FD_ISSET(currentFD, &recv_set) != 0) {
				if (currentFD == _listeningSocket)
				{
					if ((newfd = acceptNewClients()) == -1)
						continue ;
					FD_SET(newfd, &master_set);
					++_activeClients;
					//_logger->log(LOGLVL_INFO, "New client connection.");
				}
				else
				{
					int     readBytes;


		char	*recvInput = NULL;
		if ((readBytes = _cryptoWrapper->recvEncrypted(currentFD, &recvInput)) <= 0) {						
						if (readBytes == 0)
							_logger->log(LOGLVL_INFO, "A client disconnected.");
						else
							_logger->log(LOGLVL_ERROR, "Critival receive error! Disconnecting client.");

						close(currentFD);
						FD_CLR(currentFD, &master_set);
						--_activeClients;
					}
					else
					{

						if (strncmp(recvInput, QUIT_CMD, strlen(QUIT_CMD)) == 0
							|| strncmp(recvInput, EXIT_CMD, strlen(EXIT_CMD)) == 0) {
							free(recvInput);
							return ;
						}

						if (strncmp(recvInput, SHELL_CMD, strlen(SHELL_CMD)) == 0)
						{
							if ((shell_pid = this->popShell(currentFD)) == SHELL_SPAWN_ERROR) {
								_logger->log(LOGLVL_ERROR, "Shell spawn error");
								close(currentFD);
								FD_CLR(currentFD, &master_set);
								--_activeClients;
								_logger->log(LOGLVL_INFO, "Client disconnected due to shell spawn error.");
								free(recvInput);
								continue ;
							}
							_childsPIDs.push_back(shell_pid);
							_logger->log(LOGLVL_INFO, "Client spawned a shell with pid: " + std::to_string(shell_pid));
							FD_CLR(currentFD, &master_set);
						}
						else
							_logger->log(LOGLVL_LOG, "Received client data: \"" + std::string(recvInput) + "\"");
					}
					free(recvInput);
				}
			} 
		}
		handleShellDisconnect();
	}
	return ;
}

pid_t    ConnectionManager::popShell(int filedesc) {
	pid_t shellpop = fork();
	if (shellpop < 0)
		return (SHELL_SPAWN_ERROR);
	else if (shellpop == 0)
	{
		RAND_poll();

		int                 bytes;
		char				*userCMD = NULL;
		std::string         serverResponse;
		std::stringstream   serverResponseStream;
		std::ifstream       execFile;

		if (_cryptoWrapper->sendEncrypted(filedesc, CONFIRM_SHELL, strlen(CONFIRM_SHELL)) <= 0) {
			exit(EXIT_FAILURE);
		}
		while (1)
		{
			if ((bytes = _cryptoWrapper->recvEncrypted(filedesc, &userCMD)) <= 0) {
				exit(EXIT_FAILURE);
			}
			if (strncmp(userCMD, QUIT_CMD, strlen(QUIT_CMD)) == 0
				|| strncmp(userCMD, EXIT_CMD, strlen(EXIT_CMD)) == 0) {
				_cryptoWrapper->sendEncrypted(filedesc, EXIT_CMD, strlen(EXIT_CMD));
				_activeClients--;
				break ;
			} else if (strncmp(userCMD, DISCONNECT_CMD, strlen(DISCONNECT_CMD)) == 0) {
				_activeClients--;
				break ;
			}

			char *joined = (char*)malloc(strlen(userCMD) + strlen(EXEC_CMD) + 2);
			bzero(joined, strlen(userCMD) + strlen(EXEC_CMD) + 2);
			strncpy(joined, userCMD, strlen(userCMD));
			strncat(joined, EXEC_CMD, strlen(EXEC_CMD));
			free(userCMD);
			userCMD = joined;

/* CD not working */
			if (system(userCMD) != 0) {
				_cryptoWrapper->sendEncrypted(filedesc, EXEC_ERROR_CMD, strlen(EXEC_ERROR_CMD));
				free(userCMD);
				continue ;
			}
			free(userCMD);

			execFile.open(EXEC_FILE);
			if (execFile.is_open() == false)
			{
				free(userCMD);
				exit(EXIT_FAILURE);
			}

			serverResponse.clear();
			serverResponseStream.str("");
			while (std::getline(execFile, serverResponse))
				serverResponseStream << serverResponse << '\n';
			execFile.close();
			remove(EXEC_FILE);

			serverResponse = serverResponseStream.str();

			if (_cryptoWrapper->sendEncrypted(filedesc, serverResponse.c_str(), strlen(serverResponse.c_str())) <= 0) {
				exit(EXIT_FAILURE);
			}
		}
		exit(EXIT_SUCCESS);
	} else
		return shellpop;
}

ConnectionManager &ConnectionManager::operator=(const ConnectionManager & rhs)
{
	if (this != &rhs)
		*this = rhs;
	return *this;
}

std::ostream &operator<<(std::ostream &out, ConnectionManager const & pm)
{
	(void)pm;
	out << "foobar" << std::endl;
	return out;
}