#include <stdio.h>
#include <bcm2835.h>
#include <time.h>
#include <unistd.h>

#define PUMP RPI_GPIO_P1_07

int
main(int argc, char *argv[])
{
	size_t check_delay_seconds = 600;
	size_t last_pump_time = 0;
//	size_t pump_interval_seconds = 43200;
	size_t pump_on_time_milliseconds = 20000;
printf("pump argc: %d %s\n", argc, argv[0]);

	if (!bcm2835_init())
	    return -1;

	bcm2835_gpio_fsel(PUMP, BCM2835_GPIO_FSEL_OUTP);

	while(1) {
	    if ((size_t)time(NULL) >= last_pump_time) {
		last_pump_time = (size_t)time(NULL);

		bcm2835_gpio_set(PUMP);
		bcm2835_delay(pump_on_time_milliseconds);
		bcm2835_gpio_clr(PUMP);
	    }

	    sleep(check_delay_seconds);
	}
	return 0;
}
