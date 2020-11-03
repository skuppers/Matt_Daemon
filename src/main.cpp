#include "Tintin_reporter.hpp"

int main(void)
{
    // TODO:
    // Init Logger
    std::string filepath = "matt_daemon.log";
    Tintin_reporter tintin(filepath);
    tintin.log("Startup");
    
    // Init Signal handler -> write to logfile
    // Create lockfile
    // Listen to port 4242

    // Daemonize
    //  ->  handle connections max 3.
    //  ->  Write entries to logfile ()


    // Client
    // -> Remote shell
    // -> Chiffrage traffic
    // -> Authentication Asym keys
    
    return 0;
}