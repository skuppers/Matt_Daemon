#ifndef __GENERAL__HPP__
#define __GENERAL__HPP__

#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define DFLT_LOCKFILE           "/var/lock/matt_daemon.lock"
#define DFLT_LOGFILE			"/var/log/matt_daemon/matt_daemon.log"

#define LOCAL_LOCKFILE      	"matt_daemon.lock"
#define LOCAL_LOGFILE_PATH  	"matt_daemon.log"

extern bool				g_signalTerminate;

void                    init_signal_handler(void);

#endif  //!__GENERAL__H__