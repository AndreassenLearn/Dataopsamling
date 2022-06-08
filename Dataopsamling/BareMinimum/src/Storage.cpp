#include "../include/Storage.h"

#include <avr/eeprom.h>

#include "../include/Flags.h"

Storage::Storage()
{
	_cursor = begin();
	_isFull = false;
}

void Storage::Initiate(uint16_t secondsBetweenRecords)
{
	_sRecordInterval = secondsBetweenRecords;
	
	// Iterate and set cursor.
	for (uint16_t i = begin(); i != end(); ++i)
	{
		if (Read(i) == RECORD_CURRENT_INDEX_VALUE)
		{
			_cursor = i;
			
			if (_cursor != begin())
			{
				Write(_cursor, RECORD_RESUMED_FROM_VALUE);
				IncrementCursor();
			}
			
			return;
		}
	}
	
	// No place for next record was found.
	_isFull = true;
}

bool Storage::SaveNext(int8_t celsius)
{
	// No further saving is done if storage is full.
	if (_isFull) return false;
	
	// Convert celsius to data value.
	uint8_t data = CelsiusToData(celsius);
	
	// Ensure not equal to special value.
	switch (data)
	{
		case RECORD_CURRENT_INDEX_VALUE:
			data++;
		case RECORD_RESUMED_FROM_VALUE:
			data++;
		default:
			break;
	}
	
	// Write data at cursor.
	Write(_cursor, data);

#ifdef VERBOSE_MODE
	// Print out measurement status.
	Serial.println("--- Wrote to storage ---");
	Serial.print("Data: ");
	Serial.println(data);
	Serial.print("Cursor: ");
	Serial.println(_cursor);
#endif
	
	// Increment cursor and mark next record location.
	if (!IncrementCursor())
	{
		return false;
	}
	
	return true;
}

void Storage::PrintAll()
{
#ifdef VERBOSE_MODE
	Serial.println("Printing all records:");
#else
	Serial.println("seconds;Celsius"); // CSV header.
#endif
	
	// Print each record.
	uint16_t streak = 0;
	for (uint16_t i = begin(); i != end(); ++i)
	{
		uint8_t record = Read(i);
		if (record == RECORD_CURRENT_INDEX_VALUE)
		{
			// Continuation point: Stop further printing - we don't want old records.
#ifdef VERBOSE_MODE
			Serial.println("Resume point.");
#endif
			break;
		}
		else if (record == RECORD_RESUMED_FROM_VALUE)
		{
			// Point of resumed measurement: Indicate this.
#ifdef VERBOSE_MODE
			Serial.println("Measurement was paused for an unknown duration.");
#endif
			streak = 0;
		}
		else
		{
#ifdef VERBOSE_MODE
			// Print Celsius and data value.
			Serial.print(streak * _sRecordInterval);
			Serial.print("s: ");
			
			Serial.print(DataToCelsius(record));
			Serial.print("\xB0");
			Serial.print("C");
			
			Serial.print(" (Data: ");
			Serial.print(record);
			Serial.print(")");
			
			Serial.println();
#else
			Serial.print((streak * _sRecordInterval));
			Serial.print(";");
			Serial.println(DataToCelsius(record));
#endif
			streak++;
		}
	}

#ifdef VERBOSE_MODE
	Serial.println("End of measurements.");
#endif

	Serial.flush();
}

void Storage::DeleteAll()
{
	ResetCursor();
	Write(_cursor, RECORD_CURRENT_INDEX_VALUE);
	_isFull = false;
#ifdef VERBOSE_MODE
	Serial.println("All measurements was deleted.");
#endif
}

bool Storage::IsFull()
{
	return _isFull;
}

uint8_t Storage::Read(uint16_t index)
{
	return eeprom_read_byte((uint8_t*)index);
}

void Storage::Write(uint16_t index, uint8_t value)
{
	if (Read(index) != value)
	{
		eeprom_write_byte((uint8_t*)index, value);
	}
}

void Storage::ResetCursor()
{
	_cursor = begin();
}

bool Storage::IncrementCursor()
{
	// If cursor exceeds storage.
	if ((_cursor) >= end())
	{
		_isFull = true;
		
		return false;
	}
	
	// Increment cursor and write resume point.
	Write(++_cursor, RECORD_CURRENT_INDEX_VALUE);
	
	return true;
}

uint8_t Storage::CelsiusToData(int8_t celsius)
{
	return (uint8_t)(celsius + 128);
}

int8_t Storage::DataToCelsius(uint8_t data)
{
	return (int8_t)(((int16_t)data) - 128);
}
