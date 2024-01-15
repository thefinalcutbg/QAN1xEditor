#include "PatchDatabase.h"
#include "An1File.h"
#include "PatchMemory.h"
#include "View/Browser.h"
#include <qdebug.h>

std::vector<AN1xPatch> patches;

Browser* s_browser{ nullptr };

void PatchDatabase::setBrowserView(Browser* b)
{
	s_browser = b;
}

void PatchDatabase::loadAn1File(const std::vector<unsigned char>& data)
{
	An1File file(data);

	patches.reserve(patches.size() + file.patchSize());

	for (int i = 0; i < file.patchSize(); i++) {

		auto patch = file.getPatch(i);

		if (patch.isDefaultInit()) continue;

		patches.push_back(patch);
	}

	patches.shrink_to_fit();
	

}
