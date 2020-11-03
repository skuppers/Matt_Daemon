#ifndef __TINTIN_REPORTER__H__
#define __TINTIN_REPORTER__H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <exception>

class Tintin_reporter
{
    private:
        std::ofstream*  _logfile;
        std::string     _logfileName;
        std::string     _buildLogEntry(std::string const & str);
    public:
        Tintin_reporter(void);
        Tintin_reporter(std::string log_filename);
        ~Tintin_reporter(void);

        void log(std::string const & str);
        bool isLogfileOpen(void) const;
        Tintin_reporter &operator=(Tintin_reporter const &rhs);
};

std::ostream & operator<<(std::ostream &out, Tintin_reporter const &in);

#endif  //!__TINTIN_REPORTER__H__