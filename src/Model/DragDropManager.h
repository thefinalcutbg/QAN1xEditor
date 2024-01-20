#pragma once

#include <vector>
#include <set>

namespace DragDropManager {

	void droppedToDbTable(const std::vector<int>& selectedListItems);
	void droppedToMemoryList(const std::set<long long>& selectedRowids, int row);

};