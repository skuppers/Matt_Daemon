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
    
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0
        || setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
        _logger->log("ERROR: could not create socket.");
        exit(EXIT_FAILURE);
    }

    _listeningSocket = sockfd;
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4343);
    _sin.sin_addr.s_addr = INADDR_ANY;
    memset(&_sin.sin_zero, '\0', 8);
    
    if (bind(sockfd, (const struct sockaddr *)&_sin, sizeof(_sin)) < 0) {
        _logger->log("ERROR: could not bind socket.");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, MAX_CLIENTS) != 0) {
        _logger->log("ERROR: socket could not listen.");
        exit(EXIT_FAILURE);
    }
    return ;
}

void    ConnectionManager::handleIncoming() {
    fd_set  recv_set;
    fd_set  master_set;


    struct sockaddr_in remoteaddr;

    struct timeval	tv;

	tv.tv_sec = 1;
	tv.tv_usec = 0;


    FD_ZERO(&recv_set);
    FD_ZERO(&master_set);
    FD_SET(_listeningSocket, &master_set);
    _activeClients = _listeningSocket;
    while (1)
    {
        printf("Still listening...\n");
        recv_set = master_set;
        if (select(_activeClients + 1, &recv_set, NULL, NULL, &tv) == -1) {
          //  if (errno != EINTR) {
                printf("Select failed.\n");
                exit(EXIT_FAILURE);
            //}
        }
        printf("Currently %d clients\n", _activeClients);
        for (int i = 0; i < _activeClients; i++)
        {
            if (FD_ISSET(i, &recv_set)) {
                if (i == _listeningSocket) { // New connection

                    socklen_t addr_len = sizeof(remoteaddr);

                    int newfd;

                    if ((newfd = accept(_listeningSocket, (struct sockaddr*)&remoteaddr, &addr_len)) == -1) {
                        printf("Error accepting client connection.\n");
                    } else {
                        FD_SET(newfd, &master_set); // add new socket to master_set
                       // if (newfd > _activeClients) {
                       //     _activeClients = newfd;     // Keep track of max
                       // }
                        printf("New client connection from: ");
                        printf("%s\n", inet_ntoa(remoteaddr.sin_addr));
                    }



                } else { // Handle client data
                    int nbytes;
                    char buf[256];

                    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket hung up\n");
                        } else {
                            printf("recv error!\n");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master_set); // remove from master set
                    }
                    else
                    {
                        printf("Received data\n");
                    }
                    

                }
            } 
        }
    sleep(1);

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