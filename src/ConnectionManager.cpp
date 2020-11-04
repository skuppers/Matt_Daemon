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

void    ConnectionManager::handleIncoming() {
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

        //printf("Still listening...\n");
        
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
                if (currentFD == _listeningSocket) { // New connection

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
                    char    buf[256];

                    memset(&buf, '\0', 256);
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
                        buf[strcspn(buf, "\n")] = '\0';
                        if (strncmp(buf, "quit", 256) == 0)
                            return ;
                        _logger->log(LOGLVL_INFO, "Received client data:");
                        _logger->log(LOGLVL_INFO, buf);
                    }

                }
            } 
        }

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