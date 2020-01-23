#include "solenoid.h"

int
main()
{
//	const size_t CHECK_DELAY_SECONDS = 300;
//	size_t last_solenoid_on_time = 0;
//	const size_t SOLENOID_INTERVAL_SECONDS = 43200;
//	const size_t SOL_ON_TIME_MILLISECONDS = 2000;

// write to logs start time

	check(bcm2835_init(), get_log_file(), "Failed to initialize bcm2835");

	bcm2835_gpio_fsel(SOLENOID_GPIO, BCM2835_GPIO_FSEL_OUTP);

	while(1) {
//sleep(13);
//exit(0);
//	    if ((size_t)time(NULL) >= last_solenoid_on_time + SOLENOID_INTERVAL_SECONDS) {
//		last_solenoid_on_time = (size_t)time(NULL);

		bcm2835_gpio_clr(SOLENOID_GPIO);
		bcm2835_delay(5000);

		bcm2835_gpio_set(SOLENOID_GPIO);
//		bcm2835_delay(SOL_ON_TIME_MILLISECONDS);
		bcm2835_delay(1000);

//	    }

//	    sleep(CHECK_DELAY_SECONDS);
//	    sleep(2);
	}
	return 0;
error:
	return -1;
}
