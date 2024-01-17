#pragma once
#include "An1x.h"

class Browser;
class AN1xPatch;

namespace PatchDatabase
{

	void setBrowserView(Browser* b);

	void setVoiceAsCurrent(long long rowid);

	void deleteSelectedPatches(const std::set<long long> rowids);

	void loadAn1FileToBuffer(const std::vector<unsigned char>& data, const std::string& filename = {});

	void importFileBufferToDb(bool skipDuplicatePatches);

	void saveVoice(const AN1xPatch& p);
}