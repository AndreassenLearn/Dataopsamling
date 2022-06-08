#include "../include/Measurer.h"

Measurer::Measurer(uint64_t msMeasureInterval, uint64_t msCountUpInterval, Storage &storage, Lm335 &temperatureSensor) :
	_storage(storage),
	_temperatureSensor(temperatureSensor),
	_COUNT_TARGET(msMeasureInterval / msCountUpInterval),
	S_INTERVAL((msMeasureInterval / msCountUpInterval) * (msCountUpInterval / 1000))
{}

void Measurer::Service()
{
	// Check if we have counted up enough for the next measurement.
	if (_measureCountUp >= _COUNT_TARGET)
	{
		// Take and store measurement.
		_storage.SaveNext(_temperatureSensor.GetCelcius8Bit());
		
		// Reset counter.
		_measureCountUp = 0;
	}
}

void volatile Measurer::CountUp()
{
	_measureCountUp++;
}
