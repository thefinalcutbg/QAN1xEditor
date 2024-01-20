#include <QString>

struct PatchRow
{
	long long rowid{ 0 };
	int type{ 0 };
	int layer{ 0 };
	int effect{ 0 };
	bool arp_seq{ false };
	QString name;
	QString file;
	QString comment;
};