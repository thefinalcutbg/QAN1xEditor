#include "PatchMemory.h"
#include "An1xPatch.h"
#include "MidiMaster.h"
#include "View/Browser.h"
#include <stack>
#include <qdebug.h>

std::array<int, 128> patch_order = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127 };

std::array<AN1xPatch, 128> s_patches;

AN1xPatch& getPatch(int idx) {
	return s_patches[patch_order[idx]];
};

//my implementation
/*
void rowMoved(int from, int to)
{
	if(to > from) to--;

	std::array<int, 128> new_order;

	int i, y;

	for (i = 0, y = 0; i < 128; i++, y++)
	{
		if (y == from) {

			if (from < to) {
				y++;
			}
			else if (from > to) {
				i--;
				continue;
			}
		}

		if (y == to) {

			new_order[i] = patch_order[from];

			if (from > to) {
				i++;
			}
			else if (from < to) {
				y--;
				continue;
			}

		}

		new_order[i] = patch_order[y];
	}

	patch_order = new_order;
}
*/

//lacho's implementation
void PatchMemory::rowMoved(int from, int to)
{
	if(to > from) to--;

	int moved_val = patch_order[from];

	if (to < from) {
		for (int i = from; i > to; i--) {
			patch_order[i] = patch_order[i - 1];
		}
	}
	else if (from < to) {
		for (int i = from; i < to; i++) {
			patch_order[i] = patch_order[i + 1];
		}
	}

	patch_order[to] = moved_val;
}


std::stack<int> loadStack;

Browser* browser{ nullptr };

void PatchMemory::loadFromAn1x(const std::vector<int>& indexes)
{
	loadStack = {};

	if (indexes.empty()) return;

	for (int i = indexes.size() - 1; i >= 0; i--) {
		loadStack.push(indexes[i]);
	}

	browser->setProgressBarCount(loadStack.size());

	MidiMaster::requestVoice(loadStack.top());
}

void PatchMemory::sendToAn1x(const std::vector<int>& indexes)
{
	if (indexes.empty()) return;

	browser->setProgressBarCount(indexes.size());

	for (auto idx : indexes) {

		if (idx < 0 || idx > 127) continue;

		Message msg = { 0xF0, 0x43, 0x00, 0x5C, 0x0F, 0x16, 0x11, (unsigned char)idx, 0x00 };

		for (auto value : getPatch(idx).rawData()) msg.push_back(value);
		
		AN1x::addCheckSum(msg);

		browser->incrementProgressBar();

		MidiMaster::sendBulk(msg);


	}
}

void PatchMemory::setBrowserView(Browser* b)
{
	browser = b;
}

void PatchMemory::patchRecieved(const AN1xPatch& patch)
{
	int recievedIdx = loadStack.top();
	loadStack.pop();

	getPatch(recievedIdx) = patch;
	
	if (browser) {
		browser->setPatchName(recievedIdx, patch.getName(), patch.getType());
	}

	browser->incrementProgressBar();

	if (loadStack.empty()) {
		return;
	}
	MidiMaster::requestVoice(loadStack.top());

}

void PatchMemory::loadAn1xMemPatch(int index)
{
	if (index < 0 || index > 127) return;

	MidiMaster::setCurrentPatch(getPatch(index));

	
}
