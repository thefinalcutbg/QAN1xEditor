#pragma once
#include "An1x.h"
#include <array>

class AN1xPatch {

public:
	static constexpr int SystemSize = 28;
	static constexpr int CommonSize = 1640;
	static constexpr int SceneSize = 116;
	static constexpr int SeqSize = 70;

	static constexpr int PatchSize = CommonSize + SceneSize * 2 + SeqSize;

private:
	//holds the system data
	static std::array<unsigned char, SystemSize> s_system;

	//holds the current patch data
	std::array<unsigned char, PatchSize> m_data {0x0};

	bool is_edited{ false };

	unsigned char* getParameterAddress(ParamType type, unsigned char parameter);
	const unsigned char* getParameterAddress(ParamType type, unsigned char parameter) const;

public:

	//constructs InitNormal patch
	AN1xPatch();
	//constructs patch from AN1x bulk message
	AN1xPatch(const Message bulkMsg);

	//constructs AN1xPatch from blob data
	AN1xPatch(long long rowid, const void* ptr);

	long long rowid{ 0 };

	//sets the data and returns a midi message for An1x
	std::vector<unsigned char> setParameter(ParamType type, unsigned char parameter, int value);
	
	//sets system data from AN1x message
	static bool setSystemData(const Message& bulkMessage);

	//creates an1x message to set system data
	static Message getSystemData();

	//restore system data to default
	static void restoreSystemData();

	//message to set the current patch on the AN1x
	Message getDataMessage(ParamType type) const;

	//returns value of a given parameter
	int getParameter(ParamType type, unsigned char param) const;
	
	void setFreeEGData(const std::vector<int>& data);

	//return track data
	std::vector<int> getFreeEGData() const;

	//returns the patch data
	decltype(m_data)& rawData() { return m_data; }
	const decltype(m_data)& rawData() const { return m_data; }

	//returns patch name
	std::string getName() const;

	//returns patch type
	int getType() const;

	bool operator==(const AN1xPatch& other) const
	{
		return m_data == other.m_data;
	}

	bool isEdited() const { return is_edited; }

	//checks if patch is initialized
	bool isDefaultInit() const;
};