#include "Tintin_reporter.hpp"
#include "general.hpp"

int main(void)
{
    if (getuid() != 0)
    {
        std::cerr << "Matt Daemon: You need to be root to start this program." << std::endl;
        return EXIT_FAILURE;
    }

    // lockfile --> flock(lock_filepath, LOCK_FLAGS)

    // Create Log directory
    Tintin_reporter tintin(LOCAL_LOGFILE_PATH);   // Handle file errors
    tintin.log("Tintin reporter startup.");
    
    // Init Signal handler -> write to logfile

    // Daemonize

    // Create TCP socket
    // Listen to port 4242
    // bind() & listen()

    //  ->  handle connections max 3.
    //  ->  Write entries to logfile ()


    // Client
    // -> Remote shell
    // -> Chiffrage traffic
    // -> Authentication Asym keys
    
    return 0;
}