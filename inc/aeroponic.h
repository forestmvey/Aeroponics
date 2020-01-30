#ifndef _aeroponic_h
#define _aeroponic_h

#include <sys/stat.h> /* stat */
#include <signal.h> /* kill */
#include <fcntl.h> /* fcntl */

#include "common.h"
#include "logging.h"

__BEGIN_DECLS

int
clear_gpio();

int
set_processes(int*, char*, size_t*, int*);

int
kill_processes(int[]);

pid_t
start_monitor_child(int[]);

__END_DECLS


#endif
