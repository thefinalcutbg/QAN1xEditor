#include "PatchMemory.h"

#include <stack>

#include "An1xPatch.h"
#include "MidiMaster.h"
#include "View/Browser.h"
#include "View/GlobalWidgets.h"
#include "An1File.h"

constexpr std::array<int, 128> orderInit() {

    decltype(orderInit()) result{};

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

std::stack<int> sendStack;

void PatchMemory::sendToAn1x(const std::vector<int>& indexes)
{
    sendStack = {};

	if (indexes.empty()) return;

    for (int i = indexes.size() - 1; i >= 0; i--) {
        sendStack.push(indexes[i]);
    }

	GlobalWidgets::browser->setProgressBarCount(indexes.size());

    patchSent();

}

void PatchMemory::initPatches(const std::vector<int>& indexes)
{
	for (auto index : indexes)
	{
		auto& p = getPatch(index);
		p = AN1xPatch();
		GlobalWidgets::browser->setPatchToListView(index, p.getName(), p.getType());
	}
}

void PatchMemory::setPatch(const AN1xPatch& p, int index) {

	getPatch(index) = p;

	GlobalWidgets::browser->setPatchToListView(index, p.getName(), p.getType());

}

void PatchMemory::loadAn1File(const An1File& file)
{
	for (int i = 0; i < 128; i++)
	{
		auto &p = getPatch(i);

		p = file.getPatch(i);

		GlobalWidgets::browser->setPatchToListView(i, p.getName(), p.getType());

	}
}

void PatchMemory::patchRecieved(const AN1xPatch& patch)
{
	if (loadStack.empty()) return;

	int recievedIdx = loadStack.top();
	loadStack.pop();

	setPatch(patch, recievedIdx);

	GlobalWidgets::browser->incrementProgressBar();

	if (loadStack.empty()) {
		return;
	}

	MidiMaster::requestVoice(loadStack.top());

}

void PatchMemory::patchSent()
{
    if (sendStack.empty()) return;

    auto index = sendStack.top();

    sendStack.pop();

    GlobalWidgets::browser->incrementProgressBar();

    MidiMaster::sendBulk(getPatch(index), index);
}

void PatchMemory::loadAn1xMemPatch(int index)
{
	if (index < 0 || index > 127) return;

	MidiMaster::setCurrentPatch(getPatch(index), { PatchSource::SynthMemory, index });
}
