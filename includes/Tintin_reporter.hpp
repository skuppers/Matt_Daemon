#ifndef __TINTIN_REPORTER__HPP__
#define __TINTIN_REPORTER__HPP__

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <exception>
#include <sys/stat.h>

#define DFLT_LOGFILE_DIR	"/var/log/matt_daemon/"

#define LOG_INFO "[INFO]"
#define LOG_WARN "[WARN]"
#define LOG_ERROR "[ERROR]"

static const char *LogLevel[] {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

enum LogLevel
{
    LOGLVL_INFO = 0,
    LOGLVL_WARN = 1,
    LOGLVL_ERROR = 2
};

class Tintin_reporter
{
    private:
        std::ofstream*  _logfile;
        std::string     _logfileName;
        std::string     _buildLogEntry(int loglvl, std::string const & str);
    public:
        Tintin_reporter(void);
        Tintin_reporter(std::string log_filename);
        ~Tintin_reporter(void);

        void log(int loglevel, std::string const & str);
        bool isLogfileOpen(void) const;
        Tintin_reporter &operator=(Tintin_reporter const &rhs);
};

std::ostream & operator<<(std::ostream &out, Tintin_reporter const &in);

#endif  //!__TINTIN_REPORTER__H__