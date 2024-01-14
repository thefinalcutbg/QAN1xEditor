#include "PatchMemory.h"
#include "An1xPatch.h"
#include "MidiMaster.h"
#include "View/Browser.h"
#include <stack>
#include <qdebug.h>
std::array<An1xPatch, 128> s_patches;

std::stack<int> sendStack;
std::stack<int> loadStack;

Browser* browser{ nullptr };

void PatchMemory::loadFromAn1x(const std::vector<int>& indexes)
{
	loadStack = {};

	if (indexes.empty()) return;

	for (int i = indexes.size() - 1; i >= 0; i--) {
		loadStack.push(indexes[i]);
	}

	MidiMaster::requestVoice(loadStack.top());
}

void PatchMemory::sendToSAn1x(const std::vector<int>& indexes)
{

}

void PatchMemory::setBrowser(Browser* b)
{
	browser = b;
}

void PatchMemory::patchRecieved(const An1xPatch& patch)
{
	int recievedIdx = loadStack.top();
	loadStack.pop();

	s_patches[recievedIdx] = patch;
	
	if (browser) {
		browser->setPatchName(recievedIdx, patch.getName());
	}

	if (loadStack.empty()) return;

	MidiMaster::requestVoice(loadStack.top());

}

void PatchMemory::loadAn1xMemPatch(int index)
{
	if (index < 0 || index > 127) return;

	MidiMaster::setCurrentPatch(s_patches[index]);
}
