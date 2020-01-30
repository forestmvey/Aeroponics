#ifndef _common_h
#define _common_h

#include <stdbool.h> /* true false */
#include <unistd.h> /* fork execl write pipe close read sleep */
#include <stdio.h> /* FILE snprintf fopen vfprintf fclose */
#include <time.h> /* time localtime strftime */
#include <stdlib.h> /* exit atoi */
#include <syslog.h> /* syslog */
#include <stdarg.h> /* va_start va_end */

#include <bcm2835.h> /* bcm2835_init bcm2835_gpio_fsel bcm2835_gpio_set */

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

int
log_write(const char*, ...);

char*
get_process_string(enum process);

int
clear_gpio();

__END_DECLS

#endif
