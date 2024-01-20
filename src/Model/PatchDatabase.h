#pragma once
#include "An1x.h"
#include "An1xPatch.h"

class Browser;

namespace PatchDatabase
{
	void refreshTableView();

	void setVoiceAsCurrent(long long rowid);

	void deleteSelectedPatches(const std::set<long long> rowids);

	void loadAn1FileToBuffer(const std::vector<unsigned char>& data, const std::string& filename = {});

	void importFileBufferToDb(bool skipDuplicatePatches);

	void saveVoice(const AN1xPatch& p);

	AN1xPatch getPatch(long long rowid);
}