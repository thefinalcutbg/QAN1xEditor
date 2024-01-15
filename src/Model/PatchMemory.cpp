#include "PatchMemory.h"
#include "An1xPatch.h"
#include "MidiMaster.h"
#include "View/Browser.h"
#include <stack>
#include <qdebug.h>
std::array<AN1xPatch, 128> s_patches;

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

		for (auto value : s_patches[idx].rawData()) msg.push_back(value);
		
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

	s_patches[recievedIdx] = patch;
	
	if (browser) {
		browser->setPatchName(recievedIdx, patch.getName());
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

	MidiMaster::setCurrentPatch(s_patches[index]);

	
}
