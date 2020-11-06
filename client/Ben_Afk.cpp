#include "Ben_Afk.hpp"

Ben_Afk::Ben_Afk(void)
{
	return ;
}

Ben_Afk::Ben_Afk(std::string dest, int port) : _destIP(dest), _destPort(port)
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
	char buff[CONNECT_BUFF_SIZE];
	bzero(buff, CONNECT_BUFF_SIZE);
	recv(_socket, &buff, CONNECT_BUFF_SIZE - 1, 0);
	if (strncmp(buff, RST_CMD, strlen(RST_CMD)) == 0) {
		std::cerr << "Error connecting to " << _destIP << ":" << _destPort << " : no slot avaible." << std::endl;
		return false;
	} else if (strncmp(buff, SYNACK_CMD, strlen(SYNACK_CMD)) == 0) {
		std::cout << "Connected to " << _destIP << ":" << _destPort << std::endl;
		return true;
	}
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

	if (send(_socket, input->c_str(), strlen(input->c_str()), 0) < 0) {
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
		char        serverResponse[GENERIC_BUFFER_SIZE];
		
		bzero(serverResponse, GENERIC_BUFFER_SIZE);
		if ((bytes = recv(_socket, serverResponse, GENERIC_BUFFER_SIZE - 1, 0)) <= 0)
		{
			std::cerr << "Error spawning shell. Quitting." << std::endl;
			delete input;
			return false;
		}
		serverResponse[bytes] = 0;
		fputs(serverResponse, stdout);

		delete input;
		while (1)
		{
			std::cout << SHELL_PS2;
			std::getline(std::cin, shellCMD);
			if (!std::cin) {
				std::cout << std::endl << "Exiting." << std::endl;
				send(_socket, DISCONNECT_CMD, strlen(DISCONNECT_CMD), 0);
				return false;
			}
			if (shellCMD.length() == 0)
				continue ;
			if (send(_socket, shellCMD.c_str(), strlen(shellCMD.c_str()), 0) <= 0) {
				std::cerr << "Error sending command. Quitting shell." << std::endl;
				return false ;
			}
			if ((bytes = recv(_socket, serverResponse, 4096, 0)) <= 0) {
				std::cerr << "Error receiving shell response. Quitting shell." << std::endl;
				return false ;
			}
			serverResponse[bytes] = 0;
			if (strncmp(serverResponse, EXIT_CMD, strlen(EXIT_CMD)) == 0) {
				return false ;
			} else if (strncmp(serverResponse, EXEC_ERROR_CMD, strlen(EXEC_ERROR_CMD)) == 0) {
				std::cerr << "Error executing command." << std::endl;
				continue ;
			}
			fputs(serverResponse, stdout);
		}
	}
	delete input;
	return true;
}