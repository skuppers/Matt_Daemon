#include "PolicyManager.hpp"
#include "Tintin_reporter.hpp"
#include "general.hpp"

int main(void)
{
    PolicyManager policymngr(DFLT_LOCKFILE);

    policymngr.checkUID();
    policymngr.lock();

    // Create Log directory
//    Tintin_reporter tintin(DFLT_LOGFILE);   // Handle file errors
//    tintin.log("Tintin reporter startup.");
    
    // Init Signal handler -> write to logfile

    // Daemonize

    // Create TCP socket
    // Listen to port 4242
    // bind() & listen()

    //  ->  handle connections max 3.
    //  ->  Write entries to logfile ()

    sleep(7);

    // Client
    // -> Remote shell
    // -> Chiffrage traffic
    // -> Authentication Asym keys
    
    return 0;
}