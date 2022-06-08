#pragma once

#include <Arduino.h>

class Led
{
public:
	Led(volatile uint8_t *ddrPtr, volatile uint8_t *portPtr, uint8_t pin);
	
	/**
	* @brief Service routine.
	* @param Milliseconds at time of call.
	*/
	void Service(uint64_t currentMillis);
	
	/**
	* @brief Turn LED on.
	*/
	void On();
	
	/**
	* @brief Turn LED off.
	*/
	void Off();
	
	/**
	* @brief Switch LED to the opposite state.
	*/
	void Toggle();
	
	/**
	* @brief Start blinking of LED.
	* @param blink - Whether to blink or not.
	*/
	void Blink(bool blink, uint64_t interval = 100);
	
private:
	uint64_t _msBlinkInterval;
	uint64_t _previousBlinkMillis = 0;
	
	volatile uint8_t* _portPtr;
	uint8_t _bit;
	bool _isBlinking = false;
};
