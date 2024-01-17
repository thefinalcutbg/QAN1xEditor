#include "PatchDatabase.h"

#include "An1File.h"
#include "PatchMemory.h"
#include "View/Browser.h"
#include "Database/Database.h"
#include "MidiMaster.h"

Browser* s_browser{ nullptr };

//private functions:
void refreshTableView() {

	Db db("SELECT rowid, type, name, file, created_by, song, artist, comment FROM patch");

	std::vector<PatchRow> rows;

	while (db.hasRows()) {
		rows.emplace_back();
		rows.back().rowid = db.asRowId(0);
		rows.back().type = db.asInt(1);
		rows.back().name = db.asString(2).c_str();
		rows.back().file = db.asString(3).c_str();
		rows.back().created_by = db.asString(4).c_str();
		rows.back().song = db.asString(5).c_str();
		rows.back().artist = db.asString(6).c_str();
		rows.back().comment = db.asString(7).c_str();
	}

	s_browser->setPatchesToTableView(rows);
}


//interface:
void PatchDatabase::setBrowserView(Browser* b)
{
	s_browser = b;

	refreshTableView();
}

void PatchDatabase::setVoiceAsCurrent(long long rowid)
{
	Db db("SELECT rowid, data FROM patch WHERE rowid=?");

	db.bind(1, rowid);

	while (db.hasRows()) {

		MidiMaster::setCurrentPatch(AN1xPatch{db.asRowId(0), db.asBlob(1)});
		return;
	}
}

void PatchDatabase::deleteSelectedPatches(const std::set<long long> rowids)
{
	Db db;

	db.execute("BEGIN TRANSACTION");

	for (auto rowid : rowids) {

		db.newStatement("DELETE FROM patch WHERE rowid=?");

		db.bind(1, rowid);

		db.execute();
	}

	db.execute("END TRANSACTION");

	refreshTableView();
}

void PatchDatabase::loadAn1File(const std::vector<unsigned char>& data, const std::string& filename)
{
	An1File file(data, filename);

	Db db;

	db.execute("BEGIN TRANSACTION");

	for (int i = 0; i < file.patchSize(); i++) {

		auto patch = file.getPatch(i);

		auto name = patch.getName();

		if (name == "InitNormal" || name == "          ") continue;

		db.newStatement("INSERT INTO patch (type, name, file, comment, data) VALUES (?,?,?,?,?)");

		db.bind(1, patch.getType());
		db.bind(2, patch.getName());
		db.bind(3, file.filename);
		db.bind(4, "");
		db.bind(5, patch.rawData().data(), AN1xPatch::PatchSize);

		db.execute();
	}

	db.execute("END TRANSACTION");

	refreshTableView();
}

