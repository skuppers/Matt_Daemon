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
	char buff[16];
	bzero(buff, 16);
	recv(_socket, &buff, 15, 0);
	if (strncmp(buff, "rst", 3) == 0) {
		std::cerr << "Error connecting to " << _destIP << ":" << _destPort << " : no slot avaible." << std::endl;
		return false;
	} else if (strncmp(buff, "synack", 6) == 0) {
		std::cout << "Connected to " << _destIP << ":" << _destPort << std::endl;
		return true;
	}
	return false;
}

std::string *Ben_Afk::readInput(void) {
	std::string *input = new std::string();
	std::cout << "Ben_Afk> ";
	std::getline(std::cin, *input);
	if (!std::cin) {
		std::cout << std::endl << "Exiting." << std::endl;
		exit(42);
	}
	return input;
}

int        Ben_Afk::communicate(std::string *input) {

	if (send(_socket, input->c_str(), strlen(input->c_str()), 0) < 0) {
		std::cerr << "Error sending data!. Quitting." << std::endl;
		delete input;
		return false ;
	}

	if (input->compare("quit") == 0 || input->compare("exit") == 0)
	{
		std::cout << "Matt_daemon is shutting down." << std::endl;
		return false;
	}
	else if (input->compare("shell") == 0)
	{
		int         bytes;
		char        serverResponse[4096];
		std::string shellCMD;

		bzero(serverResponse, 4096);
		if ((bytes = recv(_socket, serverResponse, 4096, 0)) <= 0)
		{
			std::cerr << "Error spawning shell." << std::endl;
			return true;
		}
		serverResponse[bytes] = 0;
		fputs(serverResponse, stdout);

		while (1)
		{
			std::cout << "$ ";
			std::getline(std::cin, shellCMD);
			if (!std::cin) {
				std::cout << std::endl << "Exiting." << std::endl;
				send(_socket, "disconnect", 10, 0);
				exit(42);
			}
			if (shellCMD.length() == 0)
				continue ;
			if (send(_socket, shellCMD.c_str(), strlen(shellCMD.c_str()), 0) <= 0) {
				std::cerr << "Error sending command. Quitting shell." << std::endl;
				delete input;
				return false ;
			}
			if ((bytes = recv(_socket, serverResponse, 4096, 0)) <= 0) {
				std::cerr << "Error receiving shell response." << std::endl;
				delete input;
				return false ;
			}
			serverResponse[bytes] = 0;
			if (strncmp(serverResponse, "exit", 4) == 0) {
				delete input;
				return false ;
			} else if (strncmp(serverResponse, "exec_error", 10) == 0) {
				std::cerr << "Error executing command." << std::endl;
				continue ;
			}
			fputs(serverResponse, stdout);
		}
	}
	delete input;
	return true;
}