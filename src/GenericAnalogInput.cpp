#include"GenericAnalogInput.h"

/// @brief Creates a generic analog input
/// @param Name The device name
/// @param Pin Pin to use
/// @param configFile Name of the config file to use
GenericAnalogInput::GenericAnalogInput(String Name, int Pin, String configFile) : Sensor(Name) {
	config_path = "/settings/sen/" + configFile;
	analog_config.Pin = Pin;
}

/// @brief Starts an analog input 
/// @return True on success
bool GenericAnalogInput::begin() {
	// Set description
	Description.type = "analog input";
	Description.parameterQuantity = 2;
	Description.parameters = {"Analog Signal", "ADC Reading"};
	Description.units = {"mv", "raw"};
	values.resize(Description.parameterQuantity);
	// Check if config exists
	if (!checkConfig(config_path)) {
		// Set defaults
		analog_config.ADC_Voltage_mv = 3300;
		analog_config.ADC_Resolution = 4096;
		analog_config.RollingAverage = false;
		analog_config.AverageSize = 5;
		return saveConfig(config_path, getConfig());
	} else {
		// Load settings
		return setConfig(Storage::readFile(config_path), false);
	}
}

/// @brief Gets the current config
/// @return A JSON string of the config
String GenericAnalogInput::getConfig() {
	// Allocate the JSON document
	JsonDocument doc;
	// Assign current values
	doc["Name"] = Description.name;
	doc["Pin"] = analog_config.Pin;
	doc["ADC_Voltage_mv"] = analog_config.ADC_Voltage_mv;
	doc["ADC_Resolution"] = analog_config.ADC_Resolution;
	doc["RollingAverage"] = analog_config.RollingAverage;
	doc["AverageSize"] = analog_config.AverageSize;

	// Create string to hold output
	String output;
	// Serialize to string
	serializeJson(doc, output);
	return output;
}

/// @brief Sets the configuration for this device
/// @param config A JSON string of the configuration settings
/// @param save If the configuration should be saved to a file
/// @return True on success
bool GenericAnalogInput::setConfig(String config, bool save) {
	// Allocate the JSON document
  	JsonDocument doc;
	// Deserialize file contents
	DeserializationError error = deserializeJson(doc, config);
	// Test if parsing succeeds.
	if (error) {
		Logger.print(F("Deserialization failed: "));
		Logger.println(error.f_str());
		return false;
	}
	// Assign loaded values
	Description.name = doc["Name"].as<String>();
	analog_config.Pin = doc["Pin"].as<int>();
	analog_config.ADC_Voltage_mv = doc["ADC_Voltage_mv"].as<int>();
	analog_config.ADC_Resolution = doc["ADC_Resolution"].as<int>();
	analog_config.RollingAverage = doc["RollingAverage"].as<bool>() ;
	analog_config.AverageSize = doc["AverageSize"].as<int>();

	if (save) {
		if (!saveConfig(config_path, getConfig())) {
			return false;
		}
	}
	return configureInput();
}

/// @brief Takes a measurement
/// @return True on success
bool GenericAnalogInput::takeMeasurement() {
	int analogValue = getAnalogValue(analog_config.RollingAverage);
	values[0] = analogToMV(analogValue);
	values[1] = analogValue;
	return true;
}

/// @brief Configures the pin for use
/// @return True on success
bool GenericAnalogInput::configureInput() {
	pinMode(analog_config.Pin, INPUT);
	return true;
}

/// @brief Gets a reading from the analog input
/// @param average Whether to use a rolling average for the measurement
/// @return The value of the reading
int GenericAnalogInput::getAnalogValue(bool average) {
	if (!average) {
		return analogRead(analog_config.Pin);
	}
	// Resize average queue if needed
	if(readings.size() > analog_config.AverageSize) {
		readings.resize(analog_config.AverageSize);
		readings.shrink_to_fit();
	}
	if (readings.size() == analog_config.AverageSize) {
		readings.pop_back();
	}
	readings.push_front(analogRead(analog_config.Pin));
	int reading = 0;
	for (const auto& r : readings) {
		reading += r;
	}
	return reading / readings.size();
}

/// @brief Converts an analog reading to mV
/// @param value The value to convert
/// @return The reading in millivolts
int GenericAnalogInput::analogToMV(int value) {
	return value * analog_config.ADC_Voltage_mv / analog_config.ADC_Resolution;
}