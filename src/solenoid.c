#include "../inc/solenoid.h"

/*
 * The solenoid binary is used to operate a solenoid through a relay attached to
 * the raspberry pi's GPIO. The solenoid intervals are set to open up the solenoid
 * every 15 minutes for 2 seconds to deliver nutrient rich solution to plants in system.
 */
int
main()
{
	const size_t SOL_ON_TIME_MILLISECONDS = 2000; /* 2 seconds */
	const size_t SOL_OFF_TIME_MILLISECONDS = 898000; /* 15 minutes minus 2 seconds */

	check(bcm2835_init(), "Failed to initialize bcm2835");

	bcm2835_gpio_fsel(SOLENOID_GPIO, BCM2835_GPIO_FSEL_OUTP);

	while(1) {
	    bcm2835_gpio_set(SOLENOID_GPIO);
	    bcm2835_delay(SOL_OFF_TIME_MILLISECONDS);
	    bcm2835_gpio_clr(SOLENOID_GPIO);
	    bcm2835_delay(SOL_ON_TIME_MILLISECONDS);
	}
	return 0;
error:
	return -1;
}
