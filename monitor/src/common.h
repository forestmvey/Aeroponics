#ifndef _common_h
#define _common_h
#include <stdbool.h>
#include <bcm2835.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "logging.h"

#define READ 0
#define WRITE 1

#define PIPES_BUFF_SIZE 5

#define PROC_FAILURE_LIMIT 10

#define SOLENOID_GPIO RPI_GPIO_P1_07
#define PUMP_GPIO RPI_GPIO_P1_12

enum process {
	PUMP,
	SOLENOID,
	MONITOR__PROCESS__MAX,
	MONITOR = 2,
        AEROPONIC__PROCESS__MAX = 3
};

/*
 * Print the process associated with the enumeration
 */
char*
get_process_string(int process)
{
        switch(process) {
        case (PUMP):
            return "PUMP";
        case (SOLENOID):
            return "SOLENOID";
        case (MONITOR):
            return "MONITOR";
        default:
            return "INVALID PROCESS PARAMETER";
        }
}

int
clear_gpio()
{
printf("beginning gpio clear\n");
        check(bcm2835_init(), "Failed to initialize bcm2835");

	bcm2835_gpio_fsel(PUMP_GPIO, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(SOLENOID_GPIO, BCM2835_GPIO_FSEL_OUTP);

        bcm2835_gpio_set(SOLENOID_GPIO);
        bcm2835_gpio_set(PUMP_GPIO);

printf("ending gpio clear\n");

	return 0;
error:
	return -1;
}

#endif
