#include "ClipoboardManager.h"
#include "An1xPatch.h"
#include "PatchDatabase.h"
#include "PatchMemory.h"

std::vector<AN1xPatch> s_clipboard;

void ClipboardManager::copyRequestFromDatabase(const std::set<long long>& rowids)
{
	s_clipboard.clear();

	int counter = 0;

	for (auto rowid : rowids) {
		
		if (counter >= 128) return;

		s_clipboard.push_back(PatchDatabase::getPatch(rowid));
		counter++;
	}

}

void ClipboardManager::copyRequestFromMemoryList(const std::vector<int> rows)
{
	s_clipboard.clear();

	for (auto row : rows) {

		if (row < 0 || row > 127) continue;

		s_clipboard.push_back(PatchMemory::getPatch(row));
	}
}

void ClipboardManager::pasteToListRequested(int row)
{
	if (row < 0 || row > 127) return;

    for (size_t i = 0; i < s_clipboard.size(); i++) {
		PatchMemory::setPatch(s_clipboard[i], i + row);
	}
}
