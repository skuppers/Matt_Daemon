#include "Tintin_reporter.hpp"

static const char *LogLevel[] {
	LOG_LOG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

Tintin_reporter::Tintin_reporter(void)
{
    std::cout << "Creating Tintin_reporter default." << std::endl;
    return ;
}

Tintin_reporter::Tintin_reporter(std::string log_filename) : _logfileName(log_filename)
{
    std::cout << "Creating Tintin_reporter." << std::endl;
    mkdir(DFLT_LOGFILE_DIR, 0744);
    std::ofstream* outFile = new std::ofstream(_logfileName);
    if (outFile->fail())
    {
        std::cout << "Matt_Daemon: Error creating logfile!" << std::endl << "Quitting." << std::endl;
        exit(EXIT_FAILURE);
    }
    this->_logfile = outFile;
    log(LOGLVL_INFO, "Tintin reporter is online.");
}

Tintin_reporter::~Tintin_reporter(void)
{
    std::cout << "Deleting Tintin_reporter." << std::endl;
    if (this->_logfile->is_open())
    {
        *this->_logfile << Tintin_reporter::_buildLogEntry(LOGLVL_INFO, "Matt_Daemon is shutting down.") << std::endl;
        this->_logfile->close();
        delete this->_logfile;
    }
}

std::string Tintin_reporter::_buildLogEntry(int loglvl, std::string const & str) {
    std::ostringstream  entry;
    char                timestring[26];
    const std::time_t   currentTime = std::time(0);

    std::strftime(timestring, 26, "[%d/%m/%Y-%H:%M:%S]", std::localtime(&currentTime));
    entry << timestring << LogLevel[loglvl] << " - " << str;
    return entry.str();
}

void Tintin_reporter::log(int loglevel, std::string const & str) {
    if (this->_logfile->is_open())
        *this->_logfile << Tintin_reporter::_buildLogEntry(loglevel, str) << std::endl;
}

Tintin_reporter &Tintin_reporter::operator=(const Tintin_reporter & rhs)
{
    if (this != &rhs)
        *this = rhs;
    return *this;
}

std::ostream &operator<<(std::ostream &out, Tintin_reporter const & tintin)
{
    (void)tintin;
    out << "Wazzap tintin?" << std::endl;
    return out;
}