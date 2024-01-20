#pragma once
#include <set>
#include <vector>

namespace ClipboardManager 
{
	void copyRequestFromDatabase(const std::set<long long>& rowids);
	void copyRequestFromMemoryList(const std::vector<int> rows);
	void pasteToListRequested(int row);
};