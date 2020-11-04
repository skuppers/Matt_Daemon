#include "ConnectionManager.hpp"

ConnectionManager::ConnectionManager(void)
{
    return ;
}

ConnectionManager::~ConnectionManager(void)
{
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
    out << "foobar" << std::endl;
    return out;
}