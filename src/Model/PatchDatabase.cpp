#include "PatchDatabase.h"

#include "An1File.h"
#include "PatchMemory.h"
#include "View/Browser.h"
#include "Database/Database.h"
#include "MidiMaster.h"
#include "View/GlobalWidgets.h"

//private functions:
void PatchDatabase::refreshTableView() {

	Db db;
	db.newStatement("SELECT rowid, type, name, file, effect, layer, arp_seq, comment FROM patch");

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
	AN1xPatch result{};
	//block controls db lifetime
	{
		Db db;
		db.newStatement("SELECT rowid, data FROM patch WHERE rowid=?");

		db.bind(1, rowid);

		while (db.hasRows()) {
			result = db.asBlob(1);
		}
	}

	MidiMaster::setCurrentPatch(
		result,
		{ PatchSource::Database, rowid }
	);
}

void PatchDatabase::deleteSelectedPatches(const std::set<long long>& rowids)
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

void PatchDatabase::importFileBufferToDb()
{
	Db db;

	db.execute("BEGIN TRANSACTION");

	for (auto& file : s_fileBuffer)
	{

		for (int i = 0; i < file.patchCount(); i++) {

			auto patch = file.getPatch(i);

			auto name = patch.getName();
			auto type = patch.getType();

			db.newStatement("INSERT INTO patch (type, name, layer, effect, arp_seq, file, comment, data, hash) VALUES (?,?,?,?,?,?,?,?,?)");

			db.bind(1, type);
			db.bind(2, name);
			db.bind(3, patch.getLayer());
			db.bind(4, patch.getEffect());
			db.bind(5, patch.hasArpSeqEnabled());
			db.bind(6, file.filename);
			db.bind(7, file.getComment(i));
			db.bind(8, patch.rawData().data(), AN1xPatch::PatchSize);
			db.bind(9, patch.getHash());

			db.execute();
		}
	}

	db.execute("END TRANSACTION");
	

	db.execute(
		"DELETE FROM patch WHERE rowid NOT IN("
		"SELECT MIN(rowid) FROM patch GROUP BY hash)"
	);


	refreshTableView();

	s_fileBuffer.clear();
}

void PatchDatabase::saveVoice(const AN1xPatch& p, long long rowid)
{

	std::string query = rowid ?
		"UPDATE patch SET data=?, type=?, name=?, layer=?, effect=?, arp_seq=?, hash=?  WHERE rowid=?"
		:
		"INSERT INTO patch (data, type, name, layer, effect, arp_seq, hash) VALUES(?,?,?,?,?,?,?)"
		;

	Db db;
	db.newStatement(query);


	db.bind(1, p.rawData().data(), AN1xPatch::PatchSize);
	db.bind(2, p.getType());
	db.bind(3, p.getName());
	db.bind(4, p.getLayer());
	db.bind(5, p.getEffect());
	db.bind(6, p.hasArpSeqEnabled());
	db.bind(7, p.getHash());

	if (rowid) {
		db.bind(8, rowid);
	}

	db.execute();

    db.execute(
        "DELETE FROM patch WHERE rowid NOT IN("
        "SELECT MIN(rowid) FROM patch GROUP BY hash)"
    );

	refreshTableView();

	if (!rowid) GlobalWidgets::browser->scrollToBottom();
}

AN1xPatch PatchDatabase::getPatch(long long rowid)
{
	Db db;
	db.newStatement("SELECT rowid, data FROM patch WHERE rowid=?");

	db.bind(1, rowid);

	while (db.hasRows()) {

		return AN1xPatch{ db.asBlob(1) };
	}

	return AN1xPatch{};
}

void PatchDatabase::updateComment(const std::string& comment, const std::set<long long>& rowids)
{
	Db db;

	db.execute("BEGIN TRANSACTION");

	for (auto rowid : rowids)
	{
		db.newStatement("UPDATE patch SET comment=? WHERE rowid=?");
		db.bind(1, comment);
		db.bind(2, rowid);
		db.execute();
	}

	db.execute("END TRANSACTION");

	refreshTableView();
}

void PatchDatabase::importExternalDb(const std::string& filepath)
{
	if (filepath == Db::getDbPath()) return;

	Db db;

	db.newStatement("ATTACH ? AS external");
	db.bind(1, filepath);
	db.execute();

	db.execute("INSERT INTO patch SELECT * FROM external.patch");
	db.execute("DETACH external");

	db.execute(
		"DELETE FROM patch WHERE rowid NOT IN("
		"SELECT MIN(rowid) FROM patch GROUP BY hash)"
	);

	refreshTableView();
}

