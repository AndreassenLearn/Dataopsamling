#pragma once

#include <Arduino.h>

class Lm335
{
public:
	Lm335(uint8_t bit);
	
	/**
	* @brief Get current temperature in degree Celsius.
	* @return Current temperature in degree Celsius.
	*/
	float GetCelcius();
	
	/**
	* @brief Get current temperature in degree Kelvin.
	* @return Current temperature in degree Kelvin.
	*/
	float GetKelvin();
	
	/**
	* @brief Get current temperature in degree Celsius (8-bit).
	* @return 8 bit integer between -128 and 127.
	*/
	int8_t GetCelcius8Bit();
	
private:
	/**
	* @brief Update voltage reading from sensor pin.
	*/
	void Measure();
	
	uint8_t _bit;
	float _readingV = 0;
};
