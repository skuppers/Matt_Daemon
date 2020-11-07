#include <signal.h>
#include <string.h>
#include "Tintin_reporter.hpp"
#include "general.hpp"

Tintin_reporter	*g_reporter;

void catch_primary_signal(int sig)
{
	std::string signame = strsignal(sig);
	g_reporter->log(LOGLVL_INFO, "Signal catched (" + signame + ").");
}

void catch_secondary_signal(int sig)
{
	std::string signame = strsignal(sig);
	g_reporter->log(LOGLVL_WARN, "Termination signal catched (" + signame + ").");
	g_signalTerminate = true;
}

void catch_tertiary_signal(int sig)
{
	std::string signame = strsignal(sig);
	g_reporter->log(LOGLVL_ERROR, "Internal error signal catched (" + signame + "). Quitting!");
	g_signalTerminate = true;
}

void init_signal_handler(void)
{
	if (SIG_ERR == signal(SIGHUP, 	  catch_primary_signal)
		|| SIG_ERR == signal(SIGINT,  catch_primary_signal)
		|| SIG_ERR == signal(SIGQUIT, catch_secondary_signal)
  		|| SIG_ERR == signal(SIGILL,  catch_tertiary_signal)
  		|| SIG_ERR == signal(SIGTRAP, catch_tertiary_signal)
  		|| SIG_ERR == signal(SIGABRT, catch_tertiary_signal)
  		|| SIG_ERR == signal(SIGIOT,  catch_tertiary_signal)
  		|| SIG_ERR == signal(SIGFPE,  catch_tertiary_signal)
  		|| SIG_ERR == signal(SIGBUS,  catch_tertiary_signal)
  		|| SIG_ERR == signal(SIGSEGV, catch_tertiary_signal)
  		|| SIG_ERR == signal(SIGSYS,  catch_tertiary_signal)
  		|| SIG_ERR == signal(SIGPIPE, catch_tertiary_signal)
  		|| SIG_ERR == signal(SIGALRM, catch_primary_signal)
  		|| SIG_ERR == signal(SIGTERM, catch_secondary_signal)
  		|| SIG_ERR == signal(SIGURG,  catch_primary_signal)
  		|| SIG_ERR == signal(SIGTSTP, catch_primary_signal)
		|| SIG_ERR == signal(SIGCONT, catch_primary_signal)
  		|| SIG_ERR == signal(SIGTTIN, catch_primary_signal)
  		|| SIG_ERR == signal(SIGTTOU, catch_primary_signal)
  		|| SIG_ERR == signal(SIGIO,   catch_primary_signal)
  		|| SIG_ERR == signal(SIGUSR1, catch_primary_signal)
  		|| SIG_ERR == signal(SIGUSR2, catch_primary_signal))
	{
		std::cerr << "Matt_Daemon: Could not initialise signal handlers." << std::endl << "Quitting." << std::endl;
		std::cerr << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}