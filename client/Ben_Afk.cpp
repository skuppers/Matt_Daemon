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
#include <netinet/tcp.h>
#include <fcntl.h>
int        Ben_Afk::communicate(std::string *input) {

    char buf[1024];
    bzero(buf, 1024);

    if (send(_socket, input->c_str(), strlen(input->c_str()), 0) < 0) {
        std::cerr << "Error sending data!. Quitting." << std::endl;
        delete input;
        return false ;
    }
    if (input->compare("quit") == 0)
    {
        std::cout << "Matt_daemon is shutting down." << std::endl;
        return false;
    } else if (input->compare("shell") == 0) {
        
    /*    n = recv(_socket, buf, 1023, 0);
        buf[n] = 0;
        if(fputs(buf, stdout) == EOF)
            printf("\n Error : Fputs error\n");*/

    int     sockfd;
    int     one = 1;
    
    struct protoent *proto = getprotobyname("tcp");
    if ((sockfd = socket(PF_INET, SOCK_STREAM, proto->p_proto)) < 0
        || setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
        exit(EXIT_FAILURE);
    }


    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(4242);
    sin.sin_addr.s_addr = INADDR_ANY;
    memset(&sin.sin_zero, '\0', 8);
    
    if (bind(sockfd, (const struct sockaddr *)&sin, sizeof(sin)) < 0) {
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 3) != 0) {
        exit(EXIT_FAILURE);
    }
    printf("Waiting for rev shell\n");
    int newfd = accept(sockfd, NULL, NULL);

    printf("Rev shell connected\n");

        int n = 0;
        std::string shellCMD;

        while (1) {
            printf("$");
            std::getline(std::cin, shellCMD);
            //shellCMD[strcspn(shellCMD.c_str(), "\n")] = '\0';
            
            if (send(newfd, shellCMD.c_str(), strlen(shellCMD.c_str()), 0) <= 0) {
                printf("Error sending.\n");
            }

            printf("Sent. Now listening.\n");
            if ((n = recv(newfd, buf, 1023, 0)) <= 0) {
                printf ("Error receiving.\n");
            }
            printf("Received.");
            buf[n] = 0;
            if(fputs(buf, stdout) == EOF)
                printf("\n Error : Fputs error\n");
        }
        delete input;
        return true;
    }

    recv(_socket, buf, sizeof(buf), 0);
    std::cout << buf << std::endl;

    delete input;
    return true;
}