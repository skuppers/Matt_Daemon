#include "PolicyManager.hpp"

PolicyManager::PolicyManager(void)
{
    _lockfilePath = "/var/lock/matt_daemon.lock";
    _fd = -1;
    return ;
}

PolicyManager::PolicyManager(std::string lockpath) : _lockfilePath(lockpath)
{
    _fd = -1;
    return ;
}

PolicyManager::~PolicyManager()
{
    this->unlock();
    return ;
}

std::string PolicyManager::getLockfilePath(void) const {
    return this->_lockfilePath;
}

void      PolicyManager::lock(void) {
    int fd = open(_lockfilePath.c_str(), O_CREAT | O_RDWR, 0644);
    if (fd < 0 || flock(fd, LOCK_EX | LOCK_NB) < 0) {
        std::cerr << "Can't lock : " << _lockfilePath << std::endl;
        std::cerr << strerror(errno) << std::endl << "Matt Daemon: Quitting." << std::endl;
		exit(EXIT_FAILURE);
    }
    _fd = fd;
    return ;
}

void      PolicyManager::unlock(void)  {
    if (flock(_fd, LOCK_UN) < 0) {
        std::cerr << "Can't unlock :" << _lockfilePath << std::endl;
        std::cerr << strerror(errno) << std::endl << "Matt Daemon: Quitting." << std::endl;
		exit(EXIT_FAILURE);
    }
    close(_fd);
    return ;
}




PolicyManager &PolicyManager::operator=(const PolicyManager & rhs)
{
    if (this != &rhs)
        *this = rhs;
    return *this;
}

std::ostream &operator<<(std::ostream &out, PolicyManager const & pm)
{
    out << "Lockfile: " << pm.getLockfilePath() << std::endl;
    return out;
}