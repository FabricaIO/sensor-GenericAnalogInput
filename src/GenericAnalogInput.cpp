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
	values[0] = getMVValue();
	values[1] = getAnalogValue();
	return true;
}

/// @brief Configures the pin for use
/// @return True on success
bool GenericAnalogInput::configureInput() {
	pinMode(analog_config.Pin, INPUT);
	return true;
}

/// @brief Gets a reading from the analog input
/// @return The value of the reading
uint16_t GenericAnalogInput::getAnalogValue() {
	if (!analog_config.RollingAverage) {
		return analogRead(analog_config.Pin);
	}
	// Resize average queue if needed
	if(readingsAnalog.size() > analog_config.AverageSize) {
		readingsAnalog.resize(analog_config.AverageSize);
		readingsAnalog.shrink_to_fit();
	}
	if (readingsAnalog.size() == analog_config.AverageSize) {
		readingsAnalog.pop_back();
	}
	readingsAnalog.push_front(analogRead(analog_config.Pin));
	int reading = 0;
	for (const auto& r : readingsAnalog) {
		reading += r;
	}
	return reading / readingsAnalog.size();
}

/// @brief Gets a reading from the analog input as millivolts
/// @return The reading in millivolts
uint32_t GenericAnalogInput::getMVValue() {
	if (!analog_config.RollingAverage) {
		return analogReadMilliVolts(analog_config.Pin);
	}
	// Resize average queue if needed
	if(readingsMV.size() > analog_config.AverageSize) {
		readingsMV.resize(analog_config.AverageSize);
		readingsMV.shrink_to_fit();
	}
	if (readingsMV.size() == analog_config.AverageSize) {
		readingsMV.pop_back();
	}
	readingsMV.push_front(analogReadMilliVolts(analog_config.Pin));
	int reading = 0;
	for (const auto& r : readingsMV) {
		reading += r;
	}
	return reading / readingsMV.size();
}