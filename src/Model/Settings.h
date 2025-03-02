#pragma once

#include <string>

struct Settings {
	std::string midi_in{ "None" };
	std::string midi_out{ "None" };
	int midi_send_channel{ 1 };
	bool midi_thru = false;
};
