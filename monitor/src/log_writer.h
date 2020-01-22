#ifndef _log_writer_h
#define _log_writer_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Log {
	int id;
	int aeroponic_uptime;
	int pump_uptime;
	int solonoid_uptime;
	int monitor_uptime;
	int process_failures;
};


__BEGIN_DECLS

int
input_error();

int
input_log();

__END_DECLS

#endif
