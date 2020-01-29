#include "common.h"

/*
 * log_write takes a format string supplied by a macro in logging.h and opens a log
 * file in the logs directory with the given date format of "year-month.log".
 */
int
log_write(const char* fmt, ...)
{
	va_list ap;
	FILE *outfile = NULL;
	time_t now;
	struct tm  ts;
	char log_buf[40];
	char *time_end = NULL;
        size_t time_len, time_len_check;

	/*
	 * Using the time_len variable use snprintf and strftime to concatenate the current date
	 * onto the logs directory to open a new or existing log file to append message
	 */
	check_sys((time_len = snprintf(log_buf, sizeof(log_buf), "%s", "logs/")) < (int)sizeof(log_buf), "snprintf truncated");
	time_len_check = time_len;
	time(&now);
	ts = *localtime(&now);
	time_end = &log_buf[time_len];
	time_len += strftime(time_end, sizeof(log_buf) - strlen(log_buf), "%Y-%m", &ts);
	check_sys(time_len > time_len_check, "AEROPONIC COMMON ERROR: log filename error");
        time_end = &log_buf[time_len];
	check_sys(snprintf(time_end, sizeof(log_buf) - strlen(log_buf), ".%s", "log") < (int)(sizeof(log_buf) - strlen(log_buf)),
	    "AEROPONIC COMMON ERROR: snprintf truncated for lot filename");
	outfile = fopen(log_buf, "a");
	check_sys(outfile != NULL, "AEROPONIC COMMON ERROR: cannot open log files");

	/*
	 * Write the variadic message to log file
	 */
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

/*
 * Clear GPIO pins on for unrecoverable errors
 */
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

