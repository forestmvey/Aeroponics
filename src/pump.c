#include "pump.h"

/*
 * The pump binary is used to operate a water pump through a relay attached to
 * the raspberry pi's GPIO. The pump intervals are to stay dormant for 12 hours
 * of the day and then to push water to pressurize accumulator tanks for 20 seconds.
 */
int
main()
{
	size_t PUMP_ON_TIME_MILLISECONDS = 20000; /* 20 seconds */
	size_t PUMP_OFF_TIME_MILLISECONDS = 43180000; /* 12 hours minus 20 seconds */

	check(bcm2835_init(), "Failed to initialize bcm2835 for pump");

	bcm2835_gpio_fsel(PUMP_GPIO, BCM2835_GPIO_FSEL_OUTP);

	while(1) {
	    bcm2835_gpio_set(PUMP_GPIO);
	    bcm2835_delay(PUMP_OFF_TIME_MILLISECONDS);
	    bcm2835_gpio_clr(PUMP_GPIO);
	    bcm2835_delay(PUMP_ON_TIME_MILLISECONDS);

	}
	return 0;
error:
	return -1;
}
