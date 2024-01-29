#pragma once
#include <vector>
#include "An1xPatch.h"

class An1File
{

	static constexpr int fileSize = 226230;

	static constexpr int dataStart = 9014; //where the first common parameter of the first patch is located
	static constexpr int fileFreeEGsize = 768;

	static constexpr int fileCommonAndSceneSize = AN1xPatch::CommonSize + AN1xPatch::SceneSize * 2 - fileFreeEGsize; //FreeEG parameters are stored in 1 byte instead of 2
	static constexpr int fileCommonSize = AN1xPatch::CommonSize - fileFreeEGsize;
	static constexpr int firstPatchSeqBegin = 150326;

	static constexpr int firstCommentbegin = 159286;

	std::vector<unsigned char> m_data;

	int getCommonOffset(int index) const;
	int getFreeEGOffset(int index) const;
	int getSceneOffset(int index) const;
	int getSequencerOffset(int index) const;
	int getCommentOffset(int index) const;

public:

	const std::string filename;

	An1File(const std::vector<unsigned char> bytes, const std::string& filename);
	An1File();
	void setPatch(const AN1xPatch& p, int index);
	AN1xPatch getPatch(int index) const;
	std::string getComment(int index) const;
	constexpr int patchCount() const { return 128; }
	std::vector<unsigned char> getFileData() const;
};