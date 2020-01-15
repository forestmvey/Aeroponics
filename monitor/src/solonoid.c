#include <bcm2835.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#define SOLENOID RPI_GPIO_P1_07

int
main()
{
	const size_t CHECK_DELAY_SECONDS = 300;
	size_t last_solenoid_on_time = 0;
	const size_t SOLENOID_INTERVAL_SECONDS = 43200;
	const size_t SOL_ON_TIME_MILLISECONDS = 2000;

// write to logs start time

	if (!bcm2835_init())
	    return -1;

	bcm2835_gpio_fsel(SOLENOID, BCM2835_GPIO_FSEL_OUTP);

	while(1) {
	    if ((size_t)time(NULL) >= last_solenoid_on_time + SOLENOID_INTERVAL_SECONDS) {
		last_solenoid_on_time = (size_t)time(NULL);

		bcm2835_gpio_set(SOLENOID);
		bcm2835_delay(SOL_ON_TIME_MILLISECONDS);
		bcm2835_gpio_clr(SOLENOID);
	    }

	    sleep(CHECK_DELAY_SECONDS);
	}
	return 0;
}
