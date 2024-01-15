#pragma once
#include <vector>
#include "An1xPatch.h"

class Browser;

namespace PatchMemory {

	void loadFromAn1x(const std::vector<int>& indexes);
	void sendToAn1x(const std::vector<int>& indexes);
	void setBrowserView(Browser* b);
	void patchRecieved(const AN1xPatch& patch);
	void loadAn1xMemPatch(int index);
};