#pragma once

#include <Arduino.h>

#define RECORD_CURRENT_INDEX_VALUE 0
#define RECORD_RESUMED_FROM_VALUE 1

class Storage
{
public:
	Storage();
	
	/**
	* @brief Prepare storage for writing records from where it left prior to this program execution.
	* @param secondsBetweenRecords - Number of seconds between stored records. Used for outputting stored data.
	*/
	void Initiate(uint16_t secondsBetweenRecords);
	
	/**
	* @brief Save next temperature reading (Celsius).
	* @param celcius - Temperature reading in 8-bit signed integer.
	* @return Whether storage is now full.
	*/
	bool SaveNext(int8_t celcius);
	
	/**
	* @brief Print all stored measurements.
	*/
	void PrintAll();
	
	/**
	* @brief Make all records overwritable.
	*/
	void DeleteAll();
	
	/**
	* @brief Check whether storage is full or not.
	* @return True if full; otherwise false.
	*/
	bool IsFull();
	
private:
	// Basic operations.
	/**
	* @brief Read byte from underlying storage.
	* @param index - Index to read from.
	* @return Byte at index.
	*/
	uint8_t Read(uint16_t index);
	
	/**
	* @brief Write a byte to underlying storage.
	* @param index - Index to write from.
	* @param value - Byte value to write.
	*/
	void Write(uint16_t index, uint8_t value);
	
	/**
	* @brief Reset cursor to beginning of storage. WARNING: Storage can be overwritten.
	*/
	void ResetCursor();
	
	/**
	* @brief Increment cursor to next record location. If no more space; storage will be marked as full.
	* @return True on success and false if storage is full.
	*/
	bool IncrementCursor();
	
	// Conversions.
	/**
	* @brief Convert Celsius to data value.
	* @param celsius - 8 bit Celsius value.
	* @return 8 bit data value.
	*/
	uint8_t CelsiusToData(int8_t celsius);
	
	/**
	* @brief Convert data value to Celsius.
	* @param data - 8 bit data value.
	* @return 8 bit Celsius value.
	*/
	int8_t DataToCelsius(uint8_t data);
	
	// STL and C++11 iteration capability. NOTE: Not fully implemented.
	uint16_t begin()	{ return 0x00; }
	uint16_t end()		{ return length(); }
	uint16_t length() { return E2END + 1; }
	
	uint16_t _cursor;
	bool _isFull;
	
	uint16_t _sRecordInterval;
};
