#pragma once
#include <vector>
#include <set>
#include "An1xPatch.h"

class Browser;
class An1File;

namespace PatchMemory {

	void loadFromAn1x(const std::vector<int>& indexes);
	void sendToAn1x(const std::vector<int>& indexes);
	void initPatches(const std::vector<int>& indexes);
	void patchRecieved(const AN1xPatch& patch);
    void patchSent();
	void loadAn1xMemPatch(int index);
	void rowMoved(int from, int to);
	void setPatch(const AN1xPatch& patch, int row);
	void loadAn1File(const An1File& file);
	AN1xPatch& getPatch(int index);
	std::vector<unsigned char> getFile();
};
