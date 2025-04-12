#pragma once
#include <string>

struct AdvancedMidiSettings {
	int midi_send_channel{ 1 };
	bool midi_thru = false;
	int device_no{ 1 };
	int buffer_size{ 0 };
	int msDelay{ 50 };
};

struct MidiDeviceNames {
	std::string midi_in{ "None" };
	std::string midi_out{ "None" };
};
