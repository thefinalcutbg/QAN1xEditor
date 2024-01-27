#include "An1File.h"
#include <exception>

constexpr int fileSize = 226230;

An1File::An1File(const std::vector<unsigned char> bytes, const std::string& filename) : m_data(bytes), filename(filename)
{
	if (bytes.size() != fileSize)
		throw std::exception("File corrupted");
}

AN1xPatch An1File::getPatch(int index) const
{
	if (index < 0 || index > 127) return AN1xPatch();

	constexpr int dataStart = 9014; //where the first common parameter of the first patch is located
	constexpr int fileFreeEGsize = 768;

	constexpr int fileCommonAndSceneSize = AN1xPatch::CommonSize + AN1xPatch::SceneSize*2 - fileFreeEGsize; //FreeEG parameters are stored in 1 byte instead of 2
	constexpr int fileCommonSize = AN1xPatch::CommonSize - fileFreeEGsize;
	constexpr int firstPatchSeqBegin = 150326;
	
	AN1xPatch result;

	//getting common parameters
	int commonOffset = dataStart + (index * fileCommonAndSceneSize);

	for (int i = 0; i < AN1x::CommonParam::FreeEgData; i++)
	{
		result.rawData()[i] = m_data[i + commonOffset];
	}

	//getting free eg tracks
	int freeEgOffest = commonOffset + AN1x::FreeEgData;

	int fileIdx;
	int patchIdx;

	for (
		fileIdx = freeEgOffest, 
		patchIdx = AN1x::FreeEgData; 
		fileIdx < freeEgOffest + fileCommonSize; fileIdx++, patchIdx +=2)
	{
		int value = m_data[fileIdx];

		bool isNegative = value < 128;

		result.rawData()[patchIdx] = isNegative ? 0 : 1;
		result.rawData()[patchIdx + 1] = isNegative ? value : value - 128;
		
	}

	//getting scene
	int sceneOffset = commonOffset + AN1xPatch::CommonSize - fileFreeEGsize;
	
	for (int i = 0; i < AN1xPatch::SceneSize*2; i++)
	{
		int patchAddress = AN1xPatch::CommonSize + i;
		int fileAddress = sceneOffset + i;

		result.rawData()[patchAddress] = m_data[fileAddress];
	}

	//getting sequencer
	int seqOffset = firstPatchSeqBegin + AN1xPatch::SeqSize*index;

	for(int i = 0; i < AN1xPatch::SeqSize; i++)
	{
		int patchAddress = AN1xPatch::PatchSize - AN1xPatch::SeqSize + i;
		int fileAddress = seqOffset + i;

		result.rawData()[patchAddress] = m_data[fileAddress];
	}

	return result;
}

std::string An1File::getComment(int index) const
{
	if (index < 0 || index > 127) return "";

	constexpr int firstCommentbegin = 159286;

	int commentOffset = firstCommentbegin + 256 * index;

	return std::string{ (const char*)&m_data[commentOffset], 256 };
}
