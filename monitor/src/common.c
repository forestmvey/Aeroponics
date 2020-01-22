#include "common.h"

FILE*
get_log_file()
{
	FILE *outfile = fopen("test.dat", "a");
	if (outfile == NULL) {
	    printf("Error opening file");
	}
	return outfile;
}

/*
 * Print the process associated with the enumeration
 */
char*
get_process_string(enum process proc)
{
        switch(proc) {
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

