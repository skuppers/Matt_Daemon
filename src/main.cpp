#include "PolicyManager.hpp"
#include "Tintin_reporter.hpp"
#include "SignalHandler.hpp"
#include "general.hpp"

#include <signal.h>

extern Tintin_reporter *g_reporter;

int main(void)
{
    PolicyManager policymngr(DFLT_LOCKFILE);
    policymngr.checkUID();
    policymngr.lock();

    Tintin_reporter reporter(DFLT_LOGFILE);
    reporter.log("Tintin reporter startup.");
    g_reporter = &reporter;
    
    init_signal_handler(); // Children signals ?


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