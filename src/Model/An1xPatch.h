#pragma once
#include "An1x.h"
#include <array>

class An1xPatch {

	static constexpr int SystemSize = 28;
	static constexpr int CommonSize = 1640;
	static constexpr int SceneSize = 116;
	static constexpr int SeqSize = 70;

	static constexpr int PatchSize = CommonSize + SceneSize * 2 + SeqSize;

	static std::array<unsigned char, SystemSize> m_system;

	std::array<unsigned char, PatchSize> m_data {0x0};

	unsigned char* getParameterAddress(AN1xParam::Type type, unsigned char parameter);
	const unsigned char* getParameterAddress(AN1xParam::Type type, unsigned char parameter) const;

public:

	An1xPatch();
	An1xPatch(const std::vector<unsigned char> bulkMsg);

	//sets the data and returns a midi message for midi sync
	std::vector<unsigned char> setParameter(AN1xParam::Type type, unsigned char parameter, int value);
	bool setSystemData(const std::vector<unsigned char>& bulkMessage);

	int getParameter(AN1xParam::Type type, unsigned char param) const;
	std::vector<int> getTrackData() const;

	std::string getName() const;

};