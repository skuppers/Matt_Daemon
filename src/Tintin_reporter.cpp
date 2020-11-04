#include "Tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter(void)
{
    return ;
}

Tintin_reporter::Tintin_reporter(std::string log_filename) : _logfileName(log_filename)
{
    mkdir(DFLT_LOGFILE_DIR, 0744);
    std::ofstream* outFile = new std::ofstream(_logfileName);
    if (outFile->fail())
    {
        std::cout << "Matt_Daemon: Error creating logfile!" << std::endl << "Quitting." << std::endl;
        exit(EXIT_FAILURE);
    }
    this->_logfile = outFile;
}

Tintin_reporter::~Tintin_reporter(void)
{
    if (this->_logfile->is_open())
    {
        *this->_logfile << Tintin_reporter::_buildLogEntry("Tintin_Reporter is shutting down.") << std::endl;
        this->_logfile->close();
    }
}

std::string Tintin_reporter::_buildLogEntry(std::string const & str) {
    std::ostringstream entry;

    time_t now = time(0);
    tm *ltm = localtime(&now);

    entry << "[ " << ltm->tm_mday << "/";
    entry << 1 + ltm->tm_mon << "/" << 1900+ltm->tm_year << " - ";
    entry << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << " ] - ";
    entry << str;
    return entry.str();
}

void Tintin_reporter::log(std::string const & str) {
    if (this->_logfile->is_open())
        *this->_logfile << Tintin_reporter::_buildLogEntry(str) << std::endl;
    else
    {
        throw std::exception();
        return ;
    }
}

bool Tintin_reporter::isLogfileOpen(void) const {
    if (this->_logfile->is_open())
        return true;
    return false;
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