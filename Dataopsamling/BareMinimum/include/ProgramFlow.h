#pragma once

#include "Led.h"
#include "Button.h"
#include "Storage.h"

class ProgramFlow
{
public:
	ProgramFlow(Led &led, Button &button, Storage &storage);

	/**
	* @brief Enter sleep mode.
	*/
	void Sleep();
	
	/**
	* @brief Configure pin interrupt on INT0.
	*/
	void ConfigureButtonInterrupt();
	
	/**
	* @brief Configure WDT interrupt.
	*/
	void ConfigureWdtInterrupt();
	
	/**
	* @brief Kick watchdog.
	*/
	void KickWdt();
	
	/**
	* @brief Handle button press.
	*/
	void HandleButton();
	
private:
	Led& _led;
	Button& _button;
	Storage& _storage;
};
