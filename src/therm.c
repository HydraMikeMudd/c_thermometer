#include <dht11.h>
#include <stdio.h>
#include <sched.h>
#include <wiringPi.h>

#define PRINTOUT_DELAY 5000

int main(void) {

	if (wiringPiSetup() == -1) {
		printf("WiringPi setup failed\n");
		return 1;
	}

	// Set high priority
	struct sched_param sch_params;
	sch_params.sched_priority = 99;
	sched_setscheduler(0, SCHED_FIFO, &sch_params);

	temp_data curr_data;

	while (1) {
		printf("Probing DHT11...\n");
		dht11_probe();
		curr_data = dht11_get_data();

		if (curr_data.celsius_temp == -1 || curr_data.humidity_whole == -1) {
			printf("Invalid data\n");
		}
		else {
			printf("Humidity = %d.%d\n", curr_data.humidity_whole, curr_data.humidity_decimal);
			printf("Temperature Celsius = %d C\n", curr_data.celsius_temp);
			printf("Temperature Fahrenheit = %.1f F\n", curr_data.fahrenheit_temp);
		}

		delay(PRINTOUT_DELAY);
	}

	return 0;
}
