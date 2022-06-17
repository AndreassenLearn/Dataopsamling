#include "../include/Lm335.h"

#include <avr/sleep.h>

Lm335::Lm335(uint8_t bit)
{
	_bit = bit;
}

float Lm335::GetCelcius()
{
	// C = K - 273.15
	return GetKelvin() - 273.15; 
}

float Lm335::GetKelvin()
{
	Measure();
	
	// For LM335 every 10mV increase means a 1 Kelvin increase; 10mV/degreeK.
	// Since 1V equals 1000mV, this means that:
	// K = voltage * 100.
	return _readingV * 100;
}

int8_t Lm335::GetCelcius8Bit()
{
	float celcius = GetCelcius();
	
	// Round of decimals
	if (celcius >= 0)
	{
		celcius = (long)(celcius + 0.5);
	}
	else
	{
		celcius = (long)(celcius - 0.5);
	}
	
	// Cannot be more than an 8 bit unsigned integer.
	if (celcius > INT8_MAX)
	{
		return INT8_MAX;
	}
	
	// Cannot be less than an 8 bit unsigned integer.
	if (celcius < INT8_MIN)
	{
		return INT8_MIN;
	}
	
	return (int8_t)celcius;
}

void Lm335::Measure()
{	
	ADCSRA |= (1UL << ADEN); // Enable ADC.
	ADCSRA |= (1UL << ADPS0) | (1UL << ADPS1) | (1UL << ADPS2); // Set ADC clock prescaler to 128.
	ADMUX = (1UL << REFS0) | (_bit & 0x07); // Use AVcc reference. 0x07 is a mask that ensures no change is made to b3, b4, b5, b6, and b7 regardless of the value of _bit.
	
	//SMCR |= (1UL << SM0); // Set sleep mode: ADC noise reduction.
	//cli(); // Disable interrupts.
	//SMCR |= (1UL << SE); // Enable sleep.
	//sei(); // Enable interrupts.
	//asm("SLEEP"); // Enter sleep - and start ADC conversion.
	//SMCR &= ~(1UL << SE); // Disable sleep.
	
	// The above can also be done using the following AVR functions.
	set_sleep_mode(SLEEP_MODE_ADC);
	sleep_mode(); // Enter sleep - and start ADC conversion.
	
	// Awake again. However, this wasn't necessarily from the ADC interrupt.
	// Make sure conversion is done - check if ADSC is still set.
	while (ADCSRA & (1UL << ADSC)) {}
	
	// Voltage = value * Max. voltage / resolution
	// Voltage = (0 to 1023) * 5V / 1024
	_readingV = ADC * (5.0 / 1024.0); // Save measurement.
}
