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
    
    struct protoent *proto = getprotobyname("tcp");
    if ((sockfd = socket(PF_INET, SOCK_STREAM, proto->p_proto)) < 0
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
    std::cout << "Connected to " << _destIP << ":" << _destPort << std::endl;
    return true;
}

std::string *Ben_Afk::readInput(void) {
    std::string *input = new std::string();
    std::cout << "Ben_Afk> ";
    std::getline(std::cin, *input);
    return input;
}

int        Ben_Afk::communicate(std::string *input) {

    if (send(_socket, input->c_str(), strlen(input->c_str()), 0) < 0) {
        std::cerr << "Error sending data!. Quitting." << std::endl;
        delete input;
        return false ;
    }
    if (input->compare("quit") == 0)
    {
        std::cout << "Matt_daemon is shutting down." << std::endl;
        return false;
    }
    if (input->compare("shell") == 0)
    {
        char buf[256];
        bzero(buf, 256);
        recv(_socket, buf, sizeof(buf), 0);
        std::cout << buf << std::endl;
    }
    delete input;
    return true;
}