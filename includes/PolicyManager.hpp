#ifndef __POLICYMANAGER__H__
#define __POLICYMANAGER__H__

#include "Tintin_reporter.hpp"
#include <iostream>
#include <sys/file.h>
#include <string.h>
#include <unistd.h>

class PolicyManager
{
    private:
        std::string     _lockfilePath;
        int             _fd;
    public:
        PolicyManager(void);
        PolicyManager(std::string lockpath);
        ~PolicyManager(void);
        PolicyManager &operator=(PolicyManager const &rhs);

        void            checkUID(void) const;
        std::string     getLockfilePath(void) const;
        void            lock(void);
        void            unlock(void);

        void	        logEncryptionType(Tintin_reporter &l);
};

std::ostream & operator<<(std::ostream &out, PolicyManager const &in);

#endif  //!__POLICYMANAGER__H__