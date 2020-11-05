#ifndef __BEN_AFK__H__
#define __BEN_AFK__H__

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class Ben_Afk
{
    private:
        std::string         _destIP;
        int                 _destPort;
        int                 _socket;
        struct sockaddr_in  _destAddr;
    public:
        Ben_Afk(void);
        Ben_Afk(std::string destination, int port);
        ~Ben_Afk();
        
        void    createSocket(void);
};

#endif  //!__BEN_AFK__H__