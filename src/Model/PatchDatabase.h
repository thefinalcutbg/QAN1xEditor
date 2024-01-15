#pragma once
#include "An1x.h"

class Browser;

namespace PatchDatabase
{

	void setBrowserView(Browser* b);

	void loadAn1File(const std::vector<unsigned char>& data);

	void save();

	void retrieve();
}