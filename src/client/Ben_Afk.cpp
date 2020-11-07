#include "Ben_Afk.hpp"

Ben_Afk::Ben_Afk(void)
{
	return ;
}

Ben_Afk::Ben_Afk(std::string dest, int port, CryptoWrapper *cw) : _cryptoWrapper(cw), _destIP(dest), _destPort(port)
{
	_socket = -1;
	return ;
}

Ben_Afk::~Ben_Afk(void)
{
	return ;
}

bool    Ben_Afk::createSocket() {

	int     sockfd;
	int     one = 1;
	
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0
		|| setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
		std::cerr << "Ben_AFK: Failed to create socket. Quitting." << std::endl;
		return false;
	}
	_socket = sockfd;
	_destAddr.sin_family = AF_INET;
	_destAddr.sin_port = htons(_destPort);
	_destAddr.sin_addr.s_addr = inet_addr(_destIP.c_str());
	memset(&_destAddr.sin_zero, '\0', 8);
	return true;
}

bool	Ben_Afk::authenticate(int fd) {
	char	*decryptBuffer = NULL;
	char	*authSuccess = NULL;

	/* Receive login prompt */
	if (_cryptoWrapper->recvEncrypted(fd, &decryptBuffer) <= 0)
		return (false);
	std::cout << decryptBuffer;
	free(decryptBuffer);
	decryptBuffer = NULL;

	/* Prompt username & send it back */
	std::string *username = readInput(false);
	if (username == nullptr || _cryptoWrapper->sendEncrypted(fd, username->c_str(), username->length()) <= 0)
		return (false);
	delete username;

	/* Receive password prompt */
	if (_cryptoWrapper->recvEncrypted(fd, &decryptBuffer) <= 0)
		return (false);
	std::cout << decryptBuffer;
	free(decryptBuffer);
	decryptBuffer = NULL;

	/* Prompt password & send it back */
	std::string *password = readInput(false);
	if (password == nullptr || _cryptoWrapper->sendEncrypted(fd, password->c_str(), password->length()) <= 0)
		return (false);
	delete password;

	/* Receive server response: VALIDATE_AUTHENTICATION / DENY_AUTHENTICATION */
	_cryptoWrapper->recvEncrypted(fd, &authSuccess);
	if (strncmp(authSuccess, VALIDATE_AUTHENTICATION, AUTH_CONFIRM_LENGTH) == 0) {
		free (authSuccess);
		return (true);
	}
	free (authSuccess);
	return (false);
}

bool    Ben_Afk::connectToDaemon(void) {
	/* Initial TCP connection */
	if (connect(_socket, (struct sockaddr*)&_destAddr, sizeof(struct sockaddr)) < 0) {
		std::cerr << "Error connecting to " << _destIP << ":" << _destPort << std::endl;
		return false;
	}

	/* Connection confirmation, Connection error, or Connection reset */

	char *buff = NULL;
	_cryptoWrapper->recvEncrypted(_socket, &buff);

	if (strncmp(buff, RST_CMD, strlen(RST_CMD)) == 0) {
		std::cerr << "Error connecting to " << _destIP << ":" << _destPort << " : no slot avaible." << std::endl;
		free(buff);
		return (false);
		
	} else if (strncmp(buff, SYNACK_CMD, strlen(SYNACK_CMD)) == 0) {
		std::cout << "Connected to " << _destIP << ":" << _destPort << std::endl;
		free(buff);
	} else {
		std::cerr << "Fatal connection error.\n" << std::endl;
		free(buff);
		return (false);
	}

	/* Client authentication */
	if (authenticate(_socket)) {
		buff = NULL;
		_cryptoWrapper->recvEncrypted(_socket, &buff);
		std::cout << buff;
		free (buff);
		return (true);
	}
	return (false);
}

std::string *Ben_Afk::readInput(bool printShell) {
	std::string *input = new std::string();
	if (printShell)
		std::cout << SHELL_PS1;
	std::getline(std::cin, *input);
	if (!std::cin) {
		std::cout << std::endl << "Exiting." << std::endl;
		delete input;
		return nullptr;
	}
	return input;
}

int        Ben_Afk::communicate(std::string *input) {

	if (_cryptoWrapper->sendEncrypted(_socket, input->c_str(), strlen(input->c_str())) < 0) {
		std::cerr << "Error sending data!. Quitting." << std::endl;
		delete input;
		return false ;
	}

	if (input->compare(QUIT_CMD) == 0 || input->compare(EXIT_CMD) == 0)
	{
		std::cout << "Matt_daemon is shutting down." << std::endl;
		delete input;
		return false;
	}
	else if (input->compare(SHELL_CMD) == 0)
	{
		int         bytes;
		std::string shellCMD;
		char		*serverResponse = NULL;		

		if ((bytes = _cryptoWrapper->recvEncrypted(_socket, &serverResponse)) <= 0)
		{
			std::cerr << "Error spawning shell. Quitting." << std::endl;
			if (serverResponse != NULL)
				free(serverResponse);
			delete input;
			return false;
		}

		fputs(serverResponse, stdout);
		free(serverResponse);
		serverResponse = NULL;
		delete input;

		while (1)
		{
			std::cout << SHELL_PS2;
			std::getline(std::cin, shellCMD);
			if (!std::cin) {
				std::cout << std::endl << "Exiting." << std::endl;
				_cryptoWrapper->sendEncrypted(_socket, DISCONNECT_CMD, strlen(DISCONNECT_CMD));
				return false;
			}
			if (shellCMD.length() == 0)
				continue ;
			if (_cryptoWrapper->sendEncrypted(_socket, shellCMD.c_str(), strlen(shellCMD.c_str())) <= 0) {
				std::cerr << "Error sending command. Quitting shell." << std::endl;
				return false ;
			}
			if ((bytes = _cryptoWrapper->recvEncrypted(_socket, &serverResponse)) <= 0) {
				std::cerr << "Error receiving shell response. Quitting shell." << std::endl;
				return false ;
			}
			if (strncmp(serverResponse, EXIT_CMD, strlen(EXIT_CMD)) == 0) {
				free(serverResponse);
				return false ;
			} else if (strncmp(serverResponse, EXEC_ERROR_CMD, strlen(EXEC_ERROR_CMD)) == 0) {
				std::cerr << "Error executing command." << std::endl;
				free(serverResponse);
				continue ;
			}
			fputs(serverResponse, stdout);
			free(serverResponse);
		}
	}
	
	delete input;
	return true;
}

Ben_Afk &Ben_Afk::operator=(const Ben_Afk & rhs)
{
	if (this != &rhs)
		*this = rhs;
	return *this;
}

std::ostream &operator<<(std::ostream &out, Ben_Afk const & pm)
{
	(void)pm;
	out << "foobar" << std::endl;
	return out;
}