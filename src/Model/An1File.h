#pragma once
#include <vector>
#include "An1xPatch.h"

class An1File
{
	const std::vector<unsigned char> m_data;


public:

	const std::string& filename;

	An1File(const std::vector<unsigned char> bytes, const std::string& filename);
	AN1xPatch getPatch(int index);
	constexpr int patchSize() { return 128; }
};