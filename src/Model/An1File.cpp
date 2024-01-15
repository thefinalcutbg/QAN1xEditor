#include "An1File.h"
#include <exception>
#include <qdebug.h>

constexpr int fileSize = 226230;

An1File::An1File(const std::vector<unsigned char> bytes) : m_data(bytes)
{
	if (bytes.size() != fileSize)
		throw std::exception("File corrupted");
}

AN1xPatch An1File::getPatch(int index)
{
	if (index < 0 || index > 127) return AN1xPatch();

	constexpr int dataStart = 9014; //where the first common parameter of the first patch is located
	constexpr int FreeEG3_4size = 768;

	constexpr int fileCommonSize = AN1xPatch::CommonSize + AN1xPatch::SceneSize*2 - FreeEG3_4size; //FreeEG 3 and 4 missing!
	constexpr int firstPatchSeqBegin = 150325;

	AN1xPatch result;

	int commonOffset = dataStart + (index * fileCommonSize);

	for (int i = 0; i < fileCommonSize; i++)
	{
		result.rawData()[i] = m_data[i + commonOffset];
	}

	int sceneOffset = commonOffset + fileCommonSize;

	for (int i = AN1xPatch::CommonSize; i < AN1xPatch::CommonSize + AN1xPatch::SceneSize*2; i++)
	{
		result.rawData()[i] = m_data[i + sceneOffset];
	}

	int seqOffset = firstPatchSeqBegin + AN1xPatch::SeqSize;

	for(int i = AN1xPatch::PatchSize - AN1xPatch::SeqSize; i < AN1xPatch::PatchSize; i++) 
	{
		result.rawData()[i] = m_data[i + seqOffset];
	}

	return result;
}
