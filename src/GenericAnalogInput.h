/*
* This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2024 Sam Groveman
* 
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
			int Pin = A0;

			/// @brief The voltage used by the ADC in mv
			int ADC_Voltage_mv = 3300;

			/// @brief The resolution of the ADC
			int ADC_Resolution = 4096;

			/// @brief Whether to use a rolling average
			bool RollingAverage = false;

			/// @brief The size of the rolling average
			int AverageSize = 5;
		} analog_config;

		/// @brief Path to configuration file
		String config_path;

		/// Queue holding readgins for the rolling average
		std::deque<int> readings;

		bool configureInput();
		int getAnalogValue(bool average);
		int analogToMV(int value);

	public:
		GenericAnalogInput(int Pin, String configFile = "GenericAnalogInput.json");
		bool begin();
		String getConfig();
		bool setConfig(String config);
		bool takeMeasurement();
};