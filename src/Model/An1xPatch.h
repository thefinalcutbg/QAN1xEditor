#pragma once
#include "An1x.h"
#include <array>

class An1xPatch {

	static constexpr int SystemSize = 28;
	static constexpr int CommonSize = 1640;
	static constexpr int SceneSize = 116;
	static constexpr int SeqSize = 70;

	static constexpr int PatchSize = CommonSize + SceneSize * 2 + SeqSize;

	static std::array<unsigned char, SystemSize> s_system;

	std::array<unsigned char, PatchSize> m_data {0x0};

	unsigned char* getParameterAddress(ParamType type, unsigned char parameter);
	const unsigned char* getParameterAddress(ParamType type, unsigned char parameter) const;

public:

	//constructs InitNormal patch
	An1xPatch();
	//constructs patch from AN1x bulk message
	An1xPatch(const std::vector<unsigned char> bulkMsg);

	//sets the data and returns a midi message for An1x
	std::vector<unsigned char> setParameter(ParamType type, unsigned char parameter, int value);
	
	//sets system data from AN1x message
	static bool setSystemData(const std::vector<unsigned char>& bulkMessage);

	//creates an1x message to set system data
	static std::vector<unsigned char> getSystemData();

	std::vector<unsigned char> getDataMessage(ParamType type) const;

	std::array<unsigned char, PatchSize>& rawData() { return m_data; };

	//returns value of a given parameter
	int getParameter(ParamType type, unsigned char param) const;
	
	void setTrackData(const std::vector<int>& data);

	//return track data
	std::vector<int> getTrackData() const;

	//returns patch name
	std::string getName() const;


};