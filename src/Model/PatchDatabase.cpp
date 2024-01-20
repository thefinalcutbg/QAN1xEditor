#include "PatchDatabase.h"

#include "An1File.h"
#include "PatchMemory.h"
#include "View/Browser.h"
#include "Database/Database.h"
#include "MidiMaster.h"
#include <unordered_set>
#include "View/GlobalWidgets.h"

//private functions:
void PatchDatabase::refreshTableView() {

	Db db("SELECT rowid, type, name, file, effect, layer, arp_seq, comment FROM patch");

	std::vector<PatchRow> rows;

	while (db.hasRows()) {
		rows.emplace_back();
		rows.back().rowid = db.asRowId(0);
		rows.back().type = db.asInt(1);
		rows.back().name = db.asString(2).c_str();
		rows.back().file = db.asString(3).c_str();
		rows.back().effect = db.asInt(4);
		rows.back().layer = db.asInt(5);
		rows.back().arp_seq = db.asBool(6);
		rows.back().comment = db.asString(7).c_str();
	}

	GlobalWidgets::browser->setPatchesToTableView(rows);
}


void PatchDatabase::setVoiceAsCurrent(long long rowid)
{
	Db db("SELECT rowid, data FROM patch WHERE rowid=?");

	db.bind(1, rowid);

	while (db.hasRows()) {

		MidiMaster::setCurrentPatch(
			{ db.asRowId(0), db.asBlob(1) }, 
			{ PatchSource::Database, rowid }
		);
		return;
	}
}

void PatchDatabase::deleteSelectedPatches(const std::set<long long> rowids)
{
	Db db;

	db.execute("BEGIN TRANSACTION");

	for (auto rowid : rowids) {
		
		MidiMaster::notifyRowidDelete(rowid);

		db.newStatement("DELETE FROM patch WHERE rowid=?");

		db.bind(1, rowid);

		db.execute();
	}

	db.execute("END TRANSACTION");

	refreshTableView();
}

std::vector<An1File> s_fileBuffer;

void PatchDatabase::loadAn1FileToBuffer(const std::vector<unsigned char>& data, const std::string& filename)
{
	s_fileBuffer.push_back({ data, filename });
}

void PatchDatabase::importFileBufferToDb(bool skipDuplicatePatches)
{

	std::unordered_set<std::string> unique_nametype{"InitNormal0", ""};

	Db db;

	if (skipDuplicatePatches)
	{
		db.newStatement("SELECT name, type FROM patch");
		
		while (db.hasRows())
		{
			unique_nametype.insert(db.asString(0) + db.asString(1));
			
		}
	}

	db.execute("BEGIN TRANSACTION");

	for (auto& file : s_fileBuffer)
	{

		for (int i = 0; i < file.patchSize(); i++) {

			auto patch = file.getPatch(i);

			auto name = patch.getName();
			auto type = patch.getType();

			if (skipDuplicatePatches) {

				auto name = patch.getName();

				auto key = name + std::to_string(type);

				if (unique_nametype.count(key)) continue;

				unique_nametype.insert(key);
			}

			db.newStatement("INSERT INTO patch (type, name, layer, effect, arp_seq, file, comment, data) VALUES (?,?,?,?,?,?,?,?)");

			db.bind(1, type);
			db.bind(2, name);
			db.bind(3, patch.getLayer());
			db.bind(4, patch.getEffect());
			db.bind(5, patch.hasArpSeqEnabled());
			db.bind(6, file.filename);
			db.bind(7, "");
			db.bind(8, patch.rawData().data(), AN1xPatch::PatchSize);

			db.execute();
		}


	}

	db.execute("END TRANSACTION");

	refreshTableView();

	s_fileBuffer.clear();
}

void PatchDatabase::saveVoice(const AN1xPatch& p, long long rowid)
{
	Db db;

	if (rowid) {
		db.newStatement("UPDATE patch SET data=?, type=?, layer=?, effect=?, arp_seq=? name=? WHERE rowid=?");
		db.bind(7, rowid);
	}
	else {
		db.newStatement("INSERT INTO patch (data, type, name, layer, effect, arp_seq) VALUES(?,?,?,?,?,?)");
	}

	db.bind(1, p.rawData().data(), AN1xPatch::PatchSize);
	db.bind(2, p.getType());
	db.bind(3, p.getName());
	db.bind(4, p.getLayer());
	db.bind(5, p.getEffect());
	db.bind(6, p.hasArpSeqEnabled());
	
	db.execute();

	refreshTableView();

	if (!rowid) GlobalWidgets::browser->scrollToBottom();
}

AN1xPatch PatchDatabase::getPatch(long long rowid)
{
	Db db("SELECT rowid, data FROM patch WHERE rowid=?");

	db.bind(1, rowid);

	while (db.hasRows()) {

		return AN1xPatch{ db.asRowId(0), db.asBlob(1) };
	}

	return AN1xPatch{};
}

