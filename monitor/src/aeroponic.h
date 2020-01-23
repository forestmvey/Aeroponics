#ifndef _aeroponic_h
#define _aeroponic_h

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <bcm2835.h>

#include "logging.h"
#include "common.h"

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
