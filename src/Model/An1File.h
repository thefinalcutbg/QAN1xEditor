#pragma once
#include <vector>
#include "An1xPatch.h"

class An1File
{
	const std::vector<unsigned char> m_data;

public:

	An1File(const std::vector<unsigned char> bytes);
	AN1xPatch getPatch(int index);
	constexpr int patchSize() { return 128; }
};