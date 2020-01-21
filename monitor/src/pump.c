#include <stdio.h>
#include <bcm2835.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"
#include "logging.h"

int
main()
{
//	size_t CHECK_DELAY_SECONDS = 600;
//	size_t last_pump_time = 0;
//	size_t PUMP_INTERVAL_SECONDS = 43200;
//	size_t PUMP_INTERVAL_SECONDS = 60;
//	size_t PUMP_ON_TIME_MILLISECONDS = 20000;

	check(bcm2835_init(), "Failed to initialize bcm2835 for pump");

	bcm2835_gpio_fsel(PUMP_GPIO, BCM2835_GPIO_FSEL_OUTP);

	while(1) {
//sleep(22);
//exit(0);
//	    if ((size_t)time(NULL) >= last_pump_time + PUMP_INTERVAL_SECONDS) {
//		last_pump_time = (size_t)time(NULL);

		bcm2835_gpio_clr(PUMP_GPIO);
		bcm2835_delay(5000);


		bcm2835_gpio_set(PUMP_GPIO);
//		bcm2835_delay(PUMP_ON_TIME_MILLISECONDS);
		bcm2835_delay(1000);

//	    }

//	    sleep(CHECK_DELAY_SECONDS);
//	    sleep(3);
	}
	return 0;
error:
	return -1;
}
