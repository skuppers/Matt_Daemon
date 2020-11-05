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

void Ben_Afk::createSocket() {

    int     sockfd;
    int     one = 1;
    
    struct protoent *proto = getprotobyname("tcp");
    if ((sockfd = socket(PF_INET, SOCK_STREAM, proto->p_proto)) < 0
        || setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
        std::cerr << "Ben_AFK: Failed to create socket. Quitting." << std::endl;
        exit(EXIT_FAILURE);
    }
    _socket = sockfd;

    _destAddr.sin_family = AF_INET;
    _destAddr.sin_port = htons(_destPort);
    _destAddr.sin_addr.s_addr = inet_addr(_destIP.c_str());
    memset(&_destAddr.sin_zero, '\0', 8);
    
    if (connect(sockfd, (struct sockaddr*)&_destAddr, sizeof(struct sockaddr)) < 0) {
        std::cerr << "Error connecting to " << _destIP << ":" << _destPort << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Connected." << std::endl;
    return ;
}

void printUsage(void)
{
    std::cout << "Usage: ./Ben_AFK <destination> <port>" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int ac, char **av)
{
    if (ac < 2)
        printUsage();
    
    //verify input

    Ben_Afk benny(av[1], atoi(av[2]));
    benny.createSocket();

    return 0;
}