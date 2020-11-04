#include <signal.h>
#include "general.hpp"
#include "Tintin_reporter.hpp"

Tintin_reporter *g_reporter;

void signal_handler(int sig)
{
	std::string signame = strsignal(sig);
	g_reporter->log("Signal handler catched (" + signame + ").\n");
	//printf("Catched signal");
}

void init_signal_handler(void)
{
	for (int sig = 1; sig < 32; sig++) {
		if (sig != SIGKILL && sig != SIGSTOP && signal(sig, signal_handler) == SIG_ERR) {
            std::cerr << "Matt_Daemon: Could not initialise signal handlers." << std::endl << "Quitting." << std::endl;
			exit(EXIT_FAILURE);
        }
	}
}