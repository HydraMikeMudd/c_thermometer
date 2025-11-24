#include "dht11.h"
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>

#define SIGNAL_PIN 7
// DHT11 waits 80us plus 5 variable iterations to read 40 bits
#define MAX_TIMINGS 85 
// Threshold that determines if bit is 1 or 0, possibly adjust later
#define ONE_BIT_THRESHOLD 28 

static int dht11_data[5] = { 0, 0, 0, 0 };
static uint8_t last_state = HIGH;
static int wait_counter = 0;
static int bits_read = 0;

void dht11_send_start() {
	pinMode(SIGNAL_PIN, OUTPUT);
	digitalWrite(SIGNAL_PIN, LOW);
	delay(18); // MCU needs to pull down voltage for at least 18 ms
	digitalWrite(SIGNAL_PIN, HIGH);
	delayMicroseconds(40); // MCU needs to wait for 40 us for DHT11 response
	
	return;
}

void dht11_read_response() {
	pinMode(SIGNAL_PIN, INPUT);
	
	for (int i = 0; i < MAX_TIMINGS; i++) {
		wait_counter = 0;
		while (digitalRead(SIGNAL_PIN) == last_state) {
			wait_counter++;
			delayMicroseconds(1);
			if (wait_counter == 255) {
				// Signal too long, break
				break;
			}
		}
		last_state = digitalRead(SIGNAL_PIN);

		if (wait_counter == 255) {
			// invalid signal
			break;
		}

		if ((i >= 4) && (i % 2 == 0)) {
			// Avoid first response signals and ensures reading voltage length
			dht11_data[bits_read/8] <<= 1;
			if (wait_counter > ONE_BIT_THRESHOLD) {
				dht11_data[bits_read/8] |= 1;
			}
			bits_read++;
		}
	}

	return;
}

void dht11_probe() {
	// Reset state
	last_state = HIGH;
	wait_counter = 0;
	bits_read = 0;

	// Check DHT11
	dht11_send_start();
	dht11_read_response();

	return;
}


temp_data dht11_get_data() {
	temp_data result;

	if ((bits_read >= 40) && (dht11_data[4] == ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xFF))) {
		result.celsius_temp = dht11_data[2];
		result.fahrenheit_temp = dht11_data[2] * 9. / 5. + 32;
		result.humidity_whole = dht11_data[0];
		result.humidity_decimal = dht11_data[1];
	}
	else {
		result.celsius_temp = -1;
		result.fahrenheit_temp = -1.0;
		result.humidity_whole = -1;
		result.humidity_decimal = -1;
	}

	return result;
}
