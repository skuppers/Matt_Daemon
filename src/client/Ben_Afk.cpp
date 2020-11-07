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

bool    Ben_Afk::connectToDaemon(void) {
	if (connect(_socket, (struct sockaddr*)&_destAddr, sizeof(struct sockaddr)) < 0) {
		std::cerr << "Error connecting to " << _destIP << ":" << _destPort << std::endl;
		return false;
	}

	char *buff = NULL;
	_cryptoWrapper->recvEncrypted(_socket, &buff, CONNECT_BUFF_SIZE);

	if (strncmp(buff, RST_CMD, strlen(RST_CMD)) == 0) {
		std::cerr << "Error connecting to " << _destIP << ":" << _destPort << " : no slot avaible." << std::endl;
		free(buff);
		return false;
		
	} else if (strncmp(buff, SYNACK_CMD, strlen(SYNACK_CMD)) == 0) {
		std::cout << "Connected to " << _destIP << ":" << _destPort << std::endl;
		free(buff);
		return true;
	}

	std::cerr << "Fatal connection error.\n" << std::endl;
	free(buff);
	return false;
}

std::string *Ben_Afk::readInput(void) {
	std::string *input = new std::string();
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

		if ((bytes = _cryptoWrapper->recvEncrypted(_socket, &serverResponse, GENERIC_BUFFER_SIZE)) <= 0)
		{
			std::cerr << "Error spawning shell. Quitting." << std::endl;
			if (serverResponse != NULL)
				free(serverResponse);
			delete input;
			return false;
		}

//		serverResponse[bytes] = 0;
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
			if ((bytes = _cryptoWrapper->recvEncrypted(_socket, &serverResponse, GENERIC_BUFFER_SIZE)) <= 0) {
				std::cerr << "Error receiving shell response. Quitting shell." << std::endl;
				return false ;
			}
//			serverResponse[bytes] = 0;
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