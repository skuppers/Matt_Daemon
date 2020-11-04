#include "PolicyManager.hpp"
#include "Tintin_reporter.hpp"
#include "ConnectionManager.hpp"
#include "general.hpp"

#include <signal.h>

extern Tintin_reporter *g_reporter;

void	daemonize(Tintin_reporter *reporter)
{
	pid_t pid = fork();
	if (pid < 0) {
		reporter->log("[ERROR] - Matt_Daemon could not fork. Quitting.");
		exit(EXIT_FAILURE);
	}
	if (pid > 0)
		exit(EXIT_SUCCESS);
	if (setsid() < 0) {
		reporter->log("[ERROR] - Matt_Daemon could not detach from session. Quitting.");
		exit(EXIT_FAILURE);
	}
	umask(022);
	chdir("/");
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int		main(void)
{
    PolicyManager policymgr(DFLT_LOCKFILE);
    policymgr.checkUID();
    policymgr.lock();

    Tintin_reporter logger(DFLT_LOGFILE);		// Make logfile append mode
	g_reporter = &logger;
    logger.log("Tintin reporter startup.");
    
    init_signal_handler(); // Children signal handling when forking for executing a shell

	//daemonize(&logger);

	close(STDIN_FILENO);
//	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	//logger.log("Succesfully daemonized.");
	
	// Create TCP socket
	// Listen to port 4242
    // bind() & listen()
	ConnectionManager conmgr(&logger);
	conmgr.initSocket();

	printf("Socket is online\n");
    
	conmgr.handleIncoming();

    //  ->  handle connections max 3.
    //  ->  Write entries to logfile ()

    // Client
    // -> Remote shell
    // -> Chiffrage traffic
    // -> Authentication Asym keys
    
    return 0;
}