#ifndef _monitor_h
#define _monitor_h

#include <sys/wait.h> /* wait */

#include "logging.h"
#include "common.h"

__BEGIN_DECLS

int
check_processes(int*, int);

pid_t
start_child(enum process);

__END_DECLS

#endif
