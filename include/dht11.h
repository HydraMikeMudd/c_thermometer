#ifndef DHT11_H
#define DHT11_H

typedef struct {
	int celsius_temp;
	float fahrenheit_temp;
	int humidity_whole;
	int humidity_decimal;
} temp_data;

void dht11_send_start(void);
void dht11_read_response(void);
void dht11_probe(void);
temp_data dht11_get_data(void);

#endif
