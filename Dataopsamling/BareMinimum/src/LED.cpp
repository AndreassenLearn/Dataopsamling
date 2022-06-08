#include "../include/Led.h"

Led::Led(volatile uint8_t* ddrPtr, volatile uint8_t* portPtr, uint8_t bit)
{
	_portPtr = portPtr;
	_bit = bit;
	*ddrPtr |= (1UL << _bit); // Set bit in data direction register.
}

void Led::Service(uint64_t currentMillis)
{
	if (_isBlinking && (currentMillis - _previousBlinkMillis > _msBlinkInterval))
	{
		_previousBlinkMillis = currentMillis;
		
		Toggle();
	}
}

void Led::On()
{
	*_portPtr |= (1UL << _bit); // Set port bit.
}

void Led::Off()
{
	*_portPtr &= ~(1UL << _bit); // Clear port bit.
}

void Led::Blink(bool blink, uint64_t interval)
{
	_isBlinking = blink;
	_msBlinkInterval = interval;
}

void Led::Toggle()
{
	*_portPtr ^= (1UL << _bit); // Toggle port bit.
}
