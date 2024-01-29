#include "An1File.h"
#include <exception>
#include <QFile>

int An1File::getCommonOffset(int index) const
{
	return dataStart + (index * fileCommonAndSceneSize);
}

int An1File::getFreeEGOffset(int index) const
{
	return getCommonOffset(index) + AN1x::CommonParam::FreeEgData;
}

int An1File::getSceneOffset(int index) const
{
	return getCommonOffset(index) + AN1x::CommonParam::FreeEgData + fileFreeEGsize;
}

int An1File::getSequencerOffset(int index) const
{
	return  firstPatchSeqBegin + AN1xPatch::SeqSize * index;
}

int An1File::getCommentOffset(int index) const
{
	return firstCommentbegin + 256 * index;
}

An1File::An1File(const std::vector<unsigned char> bytes, const std::string& filename) : m_data(bytes), filename(filename)
{
	if (bytes.size() != fileSize)
        throw std::exception();
}

An1File::An1File() 
{
	QFile inputFile(":/resources/AN1xEdit.an1");
	inputFile.open(QIODevice::ReadOnly);
	auto data = inputFile.readAll();

	m_data.reserve(data.size());

	for (int i = 0; i < data.size(); i++)
	{
		m_data.push_back(data[i]);
	}
}

void An1File::setPatch(const AN1xPatch& p, int index)
{
	for (int i = 0; i < AN1x::FreeEgData; i++)
	{
		m_data[getCommonOffset(index) + i] = p.rawData()[i];
	}

	for (int i = 0; i < fileFreeEGsize*2; i += 2)
	{
		int negative = p.rawData()[AN1x::FreeEgData + i];
		int value = p.rawData()[AN1x::FreeEgData + i + 1];

		int fileValue = negative ? value-128 : value;

		m_data[getFreeEGOffset(index) + (i) / 2] = fileValue;
	}

	for (int i = 0;i < 2*AN1x::SceneSize; i++)
	{
		m_data[getSceneOffset(index) + i] = p.rawData()[i+AN1x::CommonSize];
	}

	for (int i = 0; i < AN1x::SeqencerSize; i++)
	{
		m_data[getSequencerOffset(index) + i] = p.rawData()[i+AN1x::CommonSize + 2*AN1x::SceneSize];
	}

}

AN1xPatch An1File::getPatch(int index) const
{
	if (index < 0 || index > 127) return AN1xPatch();
	
	AN1xPatch result;

	//getting common parameters

	for (int i = 0; i < AN1x::CommonParam::FreeEgData; i++)
	{
		result.rawData()[i] = m_data[i + getCommonOffset(index)];
	}

	//getting free eg tracks
	int freeEgOffest = getFreeEGOffset(index);

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
	for (int i = 0; i < AN1xPatch::SceneSize*2; i++)
	{
		int patchAddress = AN1xPatch::CommonSize + i;
		int fileAddress = getSceneOffset(index) + i;

		result.rawData()[patchAddress] = m_data[fileAddress];
	}

	//getting sequencer
	for(int i = 0; i < AN1xPatch::SeqSize; i++)
	{
		int patchAddress = AN1xPatch::PatchSize - AN1xPatch::SeqSize + i;
		int fileAddress = getSequencerOffset(index) + i;

		result.rawData()[patchAddress] = m_data[fileAddress];
	}

	return result;
}

std::string An1File::getComment(int index) const
{
	if (index < 0 || index > 127) return "";

	return std::string{ (const char*)&m_data[getCommentOffset(index)], 256 };
}

std::vector<unsigned char> An1File::getFileData() const
{
	return m_data;
}
