#ifndef _common_h
#define _common_h

#include <stdbool.h>
#include <bcm2835.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <syslog.h>

#include "logging.h"

#define READ 0
#define WRITE 1

#define PIPES_BUFF_SIZE 5

#define PROC_FAILURE_LIMIT 10

#define SECONDS_IN_HOUR 3600

#define SOLENOID_GPIO RPI_GPIO_P1_07
#define PUMP_GPIO RPI_GPIO_P1_12

enum process {
	PUMP,
	SOLENOID,
	MONITOR__PROCESS__MAX,
	MONITOR = 2,
        AEROPONIC__PROCESS__MAX = 3
};

__BEGIN_DECLS

FILE*
get_log_file();

char*
get_process_string(enum process);

int
clear_gpio();

__END_DECLS

#endif
