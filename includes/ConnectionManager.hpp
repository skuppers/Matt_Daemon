#ifndef __CONNECTIONMANAGER__H__
#define __CONNECTIONMANAGER__H__

#include <iostream>

#define MAX_CLIENTS 	3

class ConnectionManager
{
	private:
    	int		_activeClients;
	public:
    	ConnectionManager(void);
    	~ConnectionManager(void);

		ConnectionManager &operator=(ConnectionManager const &rhs);
};

std::ostream & operator<<(std::ostream &out, ConnectionManager const &in);

#endif  //!__CONNECTIONMANAGER__H__