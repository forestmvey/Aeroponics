#include "common.h"

FILE*
get_log_file()
{
	FILE *outfile = NULL;
	time_t now;
	struct tm  ts;
	char buf[80];
	char *time_end = NULL;
        size_t time_len;


	time(&now);
	ts = *localtime(&now);
	time_len = strftime(buf, sizeof(buf), "%Y-%m", &ts);
	if (time_len == 0) {
	    syslog(LOG_ALERT, "AEROPONIC COMMON ERROR: log filename error");
	}
        time_end = &buf[time_len];
	if (snprintf(time_end, sizeof(buf) - strlen(buf), ".%s", "log") >= (int)(sizeof(buf) - strlen(buf))) {
	    syslog(LOG_ALERT, "AEROPONIC COMMON ERROR: snprintf truncated for lot filename");
	}

	outfile = fopen(buf, "a");
	if (outfile == NULL) {
	    syslog(LOG_ALERT, "AEROPONIC COMMON ERROR: cannot open log files");
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
        check(bcm2835_init(), get_log_file(), "Failed to initialize bcm2835");

	bcm2835_gpio_fsel(PUMP_GPIO, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(SOLENOID_GPIO, BCM2835_GPIO_FSEL_OUTP);

        bcm2835_gpio_set(SOLENOID_GPIO);
        bcm2835_gpio_set(PUMP_GPIO);

printf("ending gpio clear\n");

	return 0;
error:
	return -1;
}

