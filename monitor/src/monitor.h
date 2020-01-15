#ifndef _monitor_h
#define _monitor_h

#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#define NDEBUG
#include "logging.h"
#include "common.h"

__BEGIN_DECLS

int
check_processes(int[], int);

pid_t
start_pump_child();

pid_t
start_solonoid_child();

__END_DECLS

#endif
