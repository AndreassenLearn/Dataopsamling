#pragma once

#include "Storage.h"
#include "Lm335.h"

class Measurer
{
public:
	Measurer(uint64_t msMeasureInterval, uint64_t msCountUpInterval, Storage &storage, Lm335 &temperatureSensor);
	
	const uint16_t S_INTERVAL;
	
	/**
	* @brief Service routine.
	*/
	void Service();
	
	/**
	* @brief Count up towards next measurement.
	*/
	volatile void CountUp();
	
private:
	const uint16_t _COUNT_TARGET;
	volatile uint16_t _measureCountUp = 0;

	Storage& _storage;
	Lm335&	_temperatureSensor;
};
