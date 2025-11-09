/*
* This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2024 Sam Groveman
* 
* External libraries needed:
* ArduinoJSON: https://arduinojson.org/
*
* Contributors: Sam Groveman
*/
#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Sensor.h>
#include <deque>

/// @brief Class describing a generic output on a GPIO pin
class GenericAnalogInput : public Sensor {
	protected:
		/// @brief Output configuration
		struct {
			/// @brief The pin number attached to the output
			int Pin;

			/// @brief Whether to use a rolling average
			bool RollingAverage;

			/// @brief The size of the rolling average
			int AverageSize;
		} analog_config;

		/// @brief Path to configuration file
		String config_path;

		/// Queue holding readings for the rolling average
		std::deque<uint16_t> readingsAnalog;

		/// Queue holding readings for the rolling average
		std::deque<uint32_t> readingsMV;

		bool configureInput();
		uint16_t getAnalogValue();
		uint32_t getMVValue();

	public:
		GenericAnalogInput(String Name, int Pin, String configFile = "GenericAnalogInput.json");
		bool begin();
		String getConfig();
		bool setConfig(String config, bool save);
		bool takeMeasurement();
};