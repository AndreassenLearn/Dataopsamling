#include "../include/ProgramFlow.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include "../include/Flags.h"

ProgramFlow::ProgramFlow(Led &led, Button &button, Storage &storage) :
	_led(led),
	_button(button),
	_storage(storage)
{}

void ProgramFlow::Sleep()
{
#ifdef VERBOSE_MODE
	Serial.print(millis());
	Serial.println(" - Enters sleep...");
#endif

	Serial.flush(); // Wait for Serial to finish transfer.
	
	SMCR |= (1UL << SM1); // Set sleep mode: Power down.
	cli(); // Disable interrupts.
	SMCR |= (1UL << SE); // Enable sleep.
	PRR |= (1UL << PRTWI) | (1UL << PRTIM2) | (1UL << PRTIM0) | (1UL << PRTIM1) | (1UL << PRSPI) | (1UL << PRADC); // Disable peripherals.
#ifndef VERBOSE_MODE
	PRR |= (1UL << PRUSART0); // Disable USART0.
#endif
	//sleep_bod_disable();
	EIMSK |= (1UL << INT0); // Ensure INT0 is enabled.
	sei(); // Enable interrupts.
	asm("SLEEP"); // Enter sleep - equal to sleep_cpu().
	SMCR &= ~(1UL << SE); // Disable sleep.
	power_all_enable(); // Re-enable all peripherals.
	
	// The above SLEEP instructions can also be done using the following AVR functions.
	//set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//sleep_mode(); // Enter sleep - while also setting and clearing the SE bit before and after.
	
	// The above POWER instructions can also be done using the following AVR functions.
	//power_adc_disable();
	//power_usart0_disable();
	//power_spi_disable();
	//power_timer0_disable();
	//power_timer1_disable();
	//power_timer2_disable();
	//power_twi_disable();
	//sleep_bod_disable();
	
#ifdef VERBOSE_MODE
	Serial.print(millis());
	Serial.println(" - Woke from sleep!");
	Serial.flush(); // Wait for Serial to finish transfer.
#endif
	
	while (Serial.available()) {
		Serial.read();
	}
}

void ProgramFlow::ConfigureButtonInterrupt()
{
	EICRA |= (1UL << ISC01); // Configure INT0 to trigger on falling.
	EIMSK |= (1UL << INT0); // Enable INT0.
}

void ProgramFlow::ConfigureWdtInterrupt()
{
	cli(); // Disable interrupts.
	
	asm("WDR"); // Watchdog reset.
	
	WDTCSR |= (1UL << WDCE) | (1UL << WDE); // Set WDCE and allow changes to WDE, WDEI and WDPx.
#if WDT_PRESCALER_SETTING == 0
	WDTCSR = (1UL << WDIE) | (1UL << WDE) | (1UL << WDP3) | (0UL << WDP2) | (0UL << WDP1) | (1UL << WDP0); // Set interrupt mode, system reset mode and prescaler of 1024 - 8s.
#elif WDT_PRESCALER_SETTING == 1
	WDTCSR = (1UL << WDIE) | (1UL << WDE) | (1UL << WDP3) | (0UL << WDP2) | (0UL << WDP1) | (0UL << WDP0); // Set interrupt mode, system reset mode and prescaler of 512 - 4s.
#endif
	
	sei(); // Enable interrupts.
}

void ProgramFlow::KickWDT()
{
	WDTCSR |= (1UL << WDIE); // Re-enable interrupt.
}

void ProgramFlow::HandleButton()
{
	switch (_button.GetState())
	{
		case Button::Released:
			if (_storage.IsFull())
			{
				_led.Blink(true, 1000); // LED will blink slowly when storage is full.
			}
			else
			{
				// Button isn't being pushed and storage isn't full; go to sleep.
				_led.Off();
				_led.Blink(false);
				KickWDT();
				Sleep();
			}
			break;
		case Button::Pushed:
			_led.Blink(false);
			_led.On();
			break;
		case Button::Pressed:
			_storage.PrintAll();
			break;
		case Button::Held:
			_led.Blink(true);
			break;
		case Button::LongPressed:
			_storage.DeleteAll();
			break;
		default:
			break;
	}
}
