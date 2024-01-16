#include "PatchDatabase.h"

#include "An1File.h"
#include "PatchMemory.h"
#include "View/Browser.h"
#include "Database/Database.h"
#include "MidiMaster.h";

#include <qdebug.h>

std::vector<AN1xPatch> patches;

Browser* s_browser{ nullptr };

void PatchDatabase::setBrowserView(Browser* b)
{
	s_browser = b;

	retrieve();

	s_browser->setPatchesToTableView(patches);
}

void PatchDatabase::setVoiceAsCurrent(int index)
{
	MidiMaster::setCurrentPatch(patches[index]);
}

void PatchDatabase::loadAn1File(const std::vector<unsigned char>& data)
{
	An1File file(data);

	patches.reserve(patches.size() + file.patchSize());

	for (int i = 0; i < file.patchSize(); i++) {

		auto patch = file.getPatch(i);

		auto name = patch.getName();

		if (name == "InitNormal" || name == "          ") continue;

		qDebug() << name.c_str();

		patches.push_back(patch);
	}

	patches.shrink_to_fit();
	
	save();

	s_browser->setPatchesToTableView(patches);
}

void PatchDatabase::save()
{
	Db db;

	db.execute("BEGIN TRANSACTION");

	db.execute("DELETE FROM patch");

	for (auto& p : patches) {

		db.newStatement("INSERT INTO patch (type, name, song, artist, comment, data) VALUES (?,?,?,?,?,?)");

		db.bind(1, p.getType());
		db.bind(2, p.getName());
		db.bind(3, p.metadata.song);
		db.bind(4, p.metadata.artist);
		db.bind(5, p.metadata.comment);
		db.bind(6, p.rawData().data(), AN1xPatch::PatchSize);

		db.execute();

	}
	
	db.execute("END TRANSACTION");
}

void PatchDatabase::retrieve()
{
	Db db("SELECT data, song, artist, comment FROM patch");
	
	while (db.hasRows()) {
		
		patches.emplace_back(db.asBlob(0));
		patches.back().metadata.song = db.asString(1);
		patches.back().metadata.artist = db.asString(2);
		patches.back().metadata.comment = db.asString(3);
	}
}
