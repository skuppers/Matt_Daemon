#include "PolicyManager.hpp"
#include "Tintin_reporter.hpp"
#include "ConnectionManager.hpp"
#include "Cryptograph.hpp"
#include "CryptoWrapper.hpp"
#include "general.hpp"

extern Tintin_reporter	*g_reporter;
bool					g_signalTerminate;

void	daemonize(Tintin_reporter *reporter)
{
	pid_t pid = fork();
	if (pid < 0) {
		reporter->log(LOGLVL_ERROR, "Matt_Daemon could not fork. Quitting.");
		exit(EXIT_FAILURE);
	}
	if (pid > 0)
		exit(EXIT_SUCCESS);
	if (setsid() < 0) {
		reporter->log(LOGLVL_ERROR, "Matt_Daemon could not detach from session. Quitting.");
		exit(EXIT_FAILURE);
	}
	umask(022);
	chdir("/");
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	reporter->log(LOGLVL_INFO, "Succesfully daemonized.");
}

int		main(void)
{
	std::string		errorMessage;

    PolicyManager policymgr(DFLT_LOCKFILE);
    policymgr.checkUID();
    policymgr.lock();

    Tintin_reporter logger(DFLT_LOGFILE);
	policymgr.logEncryptionType(logger);
	g_reporter = &logger;

    g_signalTerminate = false;
    init_signal_handler();

	//daemonize(&logger);
	
	CryptoWrapper 	cw;
	if (cw.getCryptograph()->checkIntegrity(&errorMessage) == false) {
		logger.log(LOGLVL_ERROR, errorMessage);
		return -1;
	}

	ConnectionManager conmgr(&logger, &cw);
	if (conmgr.initSocket()) 
		conmgr.handleIncoming();
    
    return 0;
}