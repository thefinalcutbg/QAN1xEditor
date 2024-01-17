#pragma once
#include "An1x.h"

class Browser;

namespace PatchDatabase
{

	void setBrowserView(Browser* b);

	void setVoiceAsCurrent(long long rowid);

	void deleteSelectedPatches(const std::set<long long> rowids);

	void loadAn1File(const std::vector<unsigned char>& data, const std::string& filename = {});

}