#pragma once

#include "An1xPatch.h"

class Browser;

namespace PatchDatabase
{
	void refreshTableView();

	void setVoiceAsCurrent(long long rowid);

	void deleteSelectedPatches(const std::set<long long>& rowids);

	void loadAn1FileToBuffer(const std::vector<unsigned char>& data, const std::string& filename = {});

    void setFavourite(bool fav, long long rowid);

	void importFileBufferToDb();

	void saveVoice(const AN1xPatch& p, long long rowid);

	AN1xPatch getPatch(long long rowid);

	void updateComment(const std::string& comment, const std::set<long long>& rowids);

	void importExternalDb(const std::string& filepath);
}
