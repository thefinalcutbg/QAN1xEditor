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

	typedef std::array<unsigned char, PatchSize> PatchRawData;

private:
	//holds the system data
	static std::array<unsigned char, SystemSize> s_system;

	//holds the current patch data
	PatchRawData m_data {0x0};

	unsigned char* getParameterAddress(ParamType type, unsigned char parameter);
	const unsigned char* getParameterAddress(ParamType type, unsigned char parameter) const;

public:

	//constructs InitNormal patch
	AN1xPatch(AN1x::InitType = AN1x::Normal);
	//constructs patch from AN1x bulk message
	AN1xPatch(const Message bulkMsg);

	//constructs AN1xPatch from blob data
	AN1xPatch(const void* ptr);

	//sets the data and returns a midi message for An1x
	std::vector<unsigned char> setParameter(ParamType type, unsigned char parameter, int value);
	
	//sets system data from AN1x message
	static bool setSystemData(const Message& bulkMessage);

	//creates an1x message to set system data
	static Message getSystemDataMsg();

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
	PatchRawData& rawData() { return m_data; }
	const PatchRawData& rawData() const { return m_data; }

	//returns patch name
	std::string getName() const;

	//returns patch type
	int getType() const;

	AN1x::VariFx getEffect() const;
	AN1x::Layer getLayer() const;
	bool hasArpSeqEnabled() const;

	std::string getHash() const;

	bool operator==(const AN1xPatch& other) const
	{
		return m_data == other.m_data;
	}

};