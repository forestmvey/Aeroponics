#include <bcm2835.h>

#define SEVEN RPI_GPIO_P1_07
#define EIGHTEEN RPI_GPIO_P1_12

int
main()
{
	if (!bcm2835_init())
	    return -1;

	bcm2835_gpio_fsel(SEVEN, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(EIGHTEEN, BCM2835_GPIO_FSEL_OUTP);
	unsigned int delay = 1000;
	while(1) {
	    bcm2835_gpio_set(SEVEN);
	    bcm2835_gpio_set(EIGHTEEN);
	    bcm2835_delay(delay);
	    bcm2835_gpio_clr(SEVEN);
	    bcm2835_gpio_clr(EIGHTEEN);
	    bcm2835_delay(delay);
	}
	return 0;
}
