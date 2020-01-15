#ifndef _aeroponic_h
#define _aeroponic_h

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define NDEBUG
#include "logging.h"
#include "common.h"

__BEGIN_DECLS

pid_t
start_monitor_child(int[]);

__END_DECLS


#endif
