#include <stdio.h>
#include <bcm2835.h>
#include <time.h>
#include <unistd.h>

#define PUMP RPI_GPIO_P1_07

int
main()
{
	size_t CHECK_DELAY_SECONDS = 600;
	size_t last_pump_time = 0;
//	size_t PUMP_INTERVAL_SECONDS = 43200;
	size_t PUMP_INTERVAL_SECONDS = 60;
	size_t PUMP_ON_TIME_MILLISECONDS = 20000;

	if (!bcm2835_init())
	    return -1;

	bcm2835_gpio_fsel(PUMP, BCM2835_GPIO_FSEL_OUTP);

	while(1) {
	    if ((size_t)time(NULL) >= last_pump_time + PUMP_INTERVAL_SECONDS) {
		last_pump_time = (size_t)time(NULL);

		bcm2835_gpio_set(PUMP);
		bcm2835_delay(PUMP_ON_TIME_MILLISECONDS);
		bcm2835_gpio_clr(PUMP);
	    }

	    sleep(CHECK_DELAY_SECONDS);
	}
	return 0;
}
