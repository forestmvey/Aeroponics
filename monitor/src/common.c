#include "common.h"

int
log_write(const char* fmt, ...)
{
	va_list ap;
	FILE *outfile = NULL;
	time_t now;
	struct tm  ts;
	char log_buf[40];
	char *time_end = NULL;
        size_t time_len;


	time(&now);
	ts = *localtime(&now);
	time_len = strftime(log_buf, sizeof(log_buf), "%Y-%m", &ts);
	check_sys(time_len != 0, "AEROPONIC COMMON ERROR: log filename error");
        time_end = &log_buf[time_len];
	check_sys(snprintf(time_end, sizeof(log_buf) - strlen(log_buf), ".%s", "log") < (int)(sizeof(log_buf) - strlen(log_buf)),
	    "AEROPONIC COMMON ERROR: snprintf truncated for lot filename");

	outfile = fopen(log_buf, "a");
	check_sys(outfile != NULL, "AEROPONIC COMMON ERROR: cannot open log files");

	va_start(ap, fmt);
	check_sys(vfprintf(outfile, fmt, ap) > 0, "vprintf failed");
	va_end(ap);

	check_sys(fclose(outfile) == 0, "fclose failed");


	return 0;
error:
	return -1;
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
        check(bcm2835_init(), "Failed to initialize bcm2835");

	bcm2835_gpio_fsel(PUMP_GPIO, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(SOLENOID_GPIO, BCM2835_GPIO_FSEL_OUTP);

        bcm2835_gpio_set(SOLENOID_GPIO);
        bcm2835_gpio_set(PUMP_GPIO);

	return 0;
error:
	return -1;
}

