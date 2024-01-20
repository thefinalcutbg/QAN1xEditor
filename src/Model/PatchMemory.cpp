#include "PatchMemory.h"
#include "An1xPatch.h"
#include "MidiMaster.h"
#include "View/Browser.h"
#include <stack>
#include <qdebug.h>
#include "View/GlobalWidgets.h"

constexpr std::array<int, 128> orderInit() {

	decltype(orderInit()) result;

	for (int i = 0; i < 128; i++) {
		result[i] = i;
	}

	return result;

}

std::array<int, 128> patch_order = orderInit();


AN1xPatch& PatchMemory::getPatch(int row) {

	static std::array<AN1xPatch, 128> s_patches;

	return s_patches[patch_order[row]];
};

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

void PatchMemory::loadFromAn1x(const std::vector<int>& indexes)
{
	loadStack = {};

	if (indexes.empty()) return;

	for (int i = indexes.size() - 1; i >= 0; i--) {
		loadStack.push(indexes[i]);
	}

	GlobalWidgets::browser->setProgressBarCount(loadStack.size());

	MidiMaster::requestVoice(loadStack.top());
}

void PatchMemory::sendToAn1x(const std::vector<int>& indexes)
{
	if (indexes.empty()) return;

	GlobalWidgets::browser->setProgressBarCount(indexes.size());

	for (auto idx : indexes) {

		if (idx < 0 || idx > 127) continue;

		Message msg = { 0xF0, 0x43, 0x00, 0x5C, 0x0F, 0x16, 0x11, (unsigned char)idx, 0x00 };

		for (auto value : getPatch(idx).rawData()) msg.push_back(value);
		
		AN1x::addCheckSum(msg);

		GlobalWidgets::browser->incrementProgressBar();

		MidiMaster::sendBulk(msg);


	}
}

void PatchMemory::setPatch(const AN1xPatch& p, int index) {

	getPatch(index) = p;

	GlobalWidgets::browser->setPatchToListView(index, p.getName(), p.getType());

}

void PatchMemory::patchRecieved(const AN1xPatch& patch)
{
	int recievedIdx = loadStack.top();
	loadStack.pop();

	setPatch(patch, recievedIdx);

	GlobalWidgets::browser->incrementProgressBar();

	if (loadStack.empty()) {
		return;
	}
	MidiMaster::requestVoice(loadStack.top());

}

void PatchMemory::loadAn1xMemPatch(int index)
{
	if (index < 0 || index > 127) return;

	MidiMaster::setCurrentPatch(getPatch(index), { PatchSource::SynthMemory, index });
}
