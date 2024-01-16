#pragma once
#include "An1x.h"

class Browser;

namespace PatchDatabase
{

	void setBrowserView(Browser* b);

	void setVoiceAsCurrent(int index);

	void loadAn1File(const std::vector<unsigned char>& data);

	void save();

	void retrieve();
}