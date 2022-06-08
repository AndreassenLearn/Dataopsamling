#pragma once

#include <Arduino.h>

// Button press durations
#define MS_BUTTON_DEBOUNCE 30
#define MS_BUTTON_HOLD 2000
#define MS_BUTTON_LONG_PRESS 5000

#if !(MS_BUTTON_PRESS < MS_BUTTON_HOLD) || !(MS_BUTTON_HOLD < MS_BUTTON_LONG_PRESS)
#error Button timings does not align!
#endif

class Button
{
public:
	enum State {
		Released = 0,
		Pushed,
		Pressed,
		Held,
		LongPressed,
		Disabled,
		Interrupt
	};

	Button(volatile uint8_t *ddrPtr, volatile uint8_t *portPtr, volatile uint8_t *pinPtr, uint8_t bit);
	
	/**
	* @brief Service routine.
	*/
	void Service(uint64_t currentMillis);
	
	/**
	* @brief Get button state. NOTE: A pressed state will be returned once only.
	*/
	State GetState();
	
	/**
	* @brief Set button state.
	*/
	volatile void SetState(State state);
	
private:
	/**
	* @brief Read bit in input register of port.
	* @return True if pin is high; otherwise false.
	*/
	bool HasInput();

	volatile uint8_t* _pInPtr;
	uint8_t _bit;
	uint64_t _firstButtonMillis = 0;
	uint64_t _previousButtonMillis = 0;
	
	volatile State _state = Released;
	bool _returnedPress = false;
};
