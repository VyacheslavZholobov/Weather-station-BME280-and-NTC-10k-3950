/*
 * Created: 23.12.2019
 * 
 */ 


#ifndef NTC_NEW_FAST_H_
#define NTC_NEW_FAST_H_

#include <avr/pgmspace.h>
#define NTC_PIN	7
#define ADC_READ_ARDUINO // Comment out if you don't need the arduino ADC reading function 

#ifdef ADC_READ_ARDUINO
uint16_t read_ADC (uint8_t adc_input)
{
	return analogRead(adc_input);
}

uint16_t read_ADC_X10 (uint8_t adc_pin)
{
	uint16_t adc_current = 0;
	for (uint16_t i = 0; i < 10; i++)
	{
		adc_current += analogRead(adc_pin);
	}
	return adc_current;
}
#endif //------------------------------------------------------------

// Temperature value returned if the sum of ADC results is greater than the first value of the table
#define TEMPERATURE_UNDER -550
// Temperature value returned if the sum of ADC results is less than the last value of the table
#define TEMPERATURE_OVER 1250	
// Temperature value corresponding to the first value in the table
#define TEMPERATURE_TABLE_START -550
// Table Step 
#define TEMPERATURE_TABLE_STEP 50

/* Table of ADC total value (10x) depending on temperature. From higher value to lower value
	The following parameters are used to construct the table:
	R1(T1): 10кОм(25°С)
	Table R/T characteristics: NTC 10k 3950 Arduino, values are taken from: 103_3950_lookuptable arduino.pdf
	Circuit diagram: +5V - 10k - NTC - GND
*/
const uint16_t termo_table[] PROGMEM = {
	10039, 10005,	9956,	9883,	9778,	9628,	9431,	9189,	
	8909,	8584,	8210,	7788,	7309,	6790,	6243,	5683,
	5120,	4571,	4048,	3559,	3111,	2706,	2346,	2030,
	1753,	1514,	1308,	1132,	981,	852,	743,	647,
	567,	495,	433,	379,	331
};

// The function calculates the temperature value in tenths of degrees Celsius depending on the total ADC value.
	int16_t calc_temperature_Fast (uint16_t  adcsum) {
	uint8_t l = 0;
	uint8_t r = (sizeof(termo_table) / sizeof(termo_table[0])) - 1;
	uint16_t thigh = pgm_read_word(&termo_table[r]); // Read and memorize the last element in the array
	
	// Checking out of limits and boundary values
	if (adcsum <= thigh) {
		#ifdef TEMPERATURE_OVER 
		if (adcsum < thigh)
		return TEMPERATURE_OVER;
		#endif
		return TEMPERATURE_TABLE_STEP * r + TEMPERATURE_TABLE_START;
	}
	uint16_t  tlow = pgm_read_word(&termo_table[0]);
	if (adcsum >= tlow) {
		#ifdef TEMPERATURE_UNDER
		if (adcsum > tlow)
		return TEMPERATURE_UNDER-5;
		#endif
		return TEMPERATURE_TABLE_START;
	}

	// Binary table search
	while ((r - l) > 1) {
		uint8_t  m = (l + r) >> 1;
		uint16_t  mid = pgm_read_word(&termo_table[m]);
		if (adcsum > mid) {
			r = m;
			} else {
			l = m;
		}
	}		

	uint16_t  vl = pgm_read_word(&termo_table[l]);
	if (adcsum >= vl) {
		return l * TEMPERATURE_TABLE_STEP + TEMPERATURE_TABLE_START;
	}
	uint16_t  vr = pgm_read_word(&termo_table[r]);
	uint16_t  vd = vl - vr;										
	int32_t temp = TEMPERATURE_TABLE_START + r * TEMPERATURE_TABLE_STEP;
	
	if (vd) {
		temp -= ((TEMPERATURE_TABLE_STEP * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
	}
	return temp;
}

int16_t Read_Temp_NTC_Fast (int8_t ntc_pin)
{
	uint16_t ADC_current = 0;
	for (uint8_t i=0; i<10; i++)
	{
		ADC_current += read_ADC(ntc_pin);
	}
	return calc_temperature_Fast(ADC_current);
}
#endif /* NTC_NEW_FAST_H_ */