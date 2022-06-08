#include "../include/Button.h"

Button::Button(volatile uint8_t *ddrPtr, volatile uint8_t *portPtr, volatile uint8_t *pInPtr, uint8_t bit)
{
	_pInPtr = pInPtr;
	_bit = bit;
	
	*ddrPtr &= ~(1UL << _bit); // Clear bit in data direction register.
	*portPtr |= (1UL << _bit); // Set port bit - enable pull-up.
}

Button::State Button::GetState()
{
	if (_state == Pressed || _state == LongPressed)
	{
		// Button has registered a press. Return this state only if it hasn't been returned before.
		if (_returnedPress)
		{
			return Pushed;
		}
		
		_returnedPress = true; // Pressed state will be returned: set flag.
	}
	
	return _state;
}

volatile void Button::SetState(State state)
{
	_state = state;
}

void Button::Service(uint64_t currentMillis)
{
	// If de-bounce is over.
	if (currentMillis - _previousButtonMillis > MS_BUTTON_DEBOUNCE)
	{
		_previousButtonMillis = currentMillis;
		
		if (HasInput())
		{
			if (_state != LongPressed)
			{
				// If waiting for release after registered long press.
				if (_state == Disabled) return;
				
				// Button pin is low and a possible long press is still pending.
				
				// Check if button wasn't already pushed or if its interrupt was triggered.
				if (_state == Released || _state == Interrupt)
				{
					// First low - check again after de-bounce time.
					_firstButtonMillis = currentMillis; // Set millis for beginning of button press.
					_state = Pushed;
					
					return;
				}
				
				// Check for hold.
				if (currentMillis - _firstButtonMillis > MS_BUTTON_HOLD)
				{
					// Button is held by user.
					
					// Check if button wasn't already held.
					if (_state == Pushed) _state = Held;
					
					// Check for long press.
					if (currentMillis - _firstButtonMillis > MS_BUTTON_LONG_PRESS)
					{
						_state = LongPressed;
					}
				}
			}
			else
			{
				// Button was hold through into a long press - no more to do before release.
				_state = Disabled;
			}
		}
		else if (_state == Pushed)
		{
			// Button was pushed then released = a press.
			_state = Pressed;
		}
		else
		{
			_state = Released;
			_returnedPress = false; // Reset button press flag - the button can now register a new press action.
		}
	}
}

bool Button::HasInput()
{
	// Test bit in port input register.
	if (*_pInPtr & (1UL << _bit)) return false;
	
	return true;
}
