#include "DragDropManager.h"
#include "PatchMemory.h"
#include "PatchDatabase.h"
#include <qdebug.h>

void DragDropManager::droppedToDbTable(const std::vector<int>& selectedListItems)
{
	for (auto idx : selectedListItems)
	{
		PatchDatabase::saveVoice(PatchMemory::getPatch(idx), 0);
	}
}

void DragDropManager::droppedToMemoryList(const std::set<long long>& selectedRowids, int row)
{

	for (auto rowid : selectedRowids)
	{
		if (row > 127) return;

		PatchMemory::setPatch(PatchDatabase::getPatch(rowid), row);

		row++;
		
	}

}
