#ifndef _aeroponic_h
#define _aeroponic_h

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#define NDEBUG
#include "logging.h"
#include "common.h"

__BEGIN_DECLS

int
kill_processes(int[]);

pid_t
start_monitor_child(int[]);

__END_DECLS


#endif
