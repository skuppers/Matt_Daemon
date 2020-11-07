#ifndef __BEN_AFK__H__
#define __BEN_AFK__H__

#include "CryptoWrapper.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define CONNECT_BUFF_SIZE   32
#define GENERIC_BUFFER_SIZE	4096

#define SHELL_PS1           "Ben_Afk> "
#define SHELL_PS2           "$ "
#define RST_CMD				"rst"
#define SYNACK_CMD 			"synack"
#define	QUIT_CMD			"quit"
#define EXIT_CMD			"exit"
#define SHELL_CMD			"shell"
#define DISCONNECT_CMD 		"disconnect"
#define EXEC_ERROR_CMD 		"exec_error"


class Ben_Afk
{
    private:
        CryptoWrapper       *_cryptoWrapper;
        std::string         _destIP;
        int                 _destPort;
        int                 _socket;
        struct sockaddr_in  _destAddr;
        
    public:
        Ben_Afk(void);
        Ben_Afk(std::string destination, int port, CryptoWrapper *cw);
        ~Ben_Afk(void);
        Ben_Afk &operator=(Ben_Afk const &rhs);
        
        bool            createSocket(void);
        bool            connectToDaemon(void);
        std::string     *readInput(void);
        int             communicate(std::string *input);
};

std::ostream & operator<<(std::ostream &out, Ben_Afk const &in);

#endif  //!__BEN_AFK__H__