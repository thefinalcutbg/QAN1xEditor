#include <QString>

struct PatchRow
{
	long long rowid{ 0 };
	int type{ 0 };
	QString name;
	QString file;
	QString created_by;
	QString song;
	QString artist;
	QString comment;
};