#include "An1xPatch.h"

using namespace AN1x;

const unsigned char* An1xPatch::getParameterAddress(AN1x::ParamType type, unsigned char parameter) const
{
	unsigned char* paramAddress = nullptr;

	static constexpr int dataOffsets[5]{
		0,
		0,
		CommonSize,
		CommonSize + SceneSize,
		CommonSize + (SceneSize * 2)
	};

	if (type == ParamType::System) {
		return &m_system[parameter];
	}
	else
	{
		int offset = dataOffsets[static_cast<int>(type)];

		return &m_data[offset + parameter];
	}
	return nullptr;
}

unsigned char* An1xPatch::getParameterAddress(AN1x::ParamType type, unsigned char parameter)
{
	return const_cast<unsigned char*>(const_cast<const An1xPatch*>(this)->getParameterAddress(type, parameter));
}

std::vector<unsigned char> An1xPatch::setParameter(ParamType type, unsigned char parameter, int value)
{

	auto paramAddress = getParameterAddress(type, parameter);

	auto msg = getHeader(type);

	msg.push_back(parameter);

	value += getOffset(type, parameter);

	if (AN1x::isTwoByteParameter(type, parameter)) {
		msg.push_back(value / 128);
		msg.push_back(value % 128);

		paramAddress[0] = value / 128;
		paramAddress[1] = value % 128;

	}
	else {
		msg.push_back(value);
		paramAddress[0] = value;
	}

	msg.push_back(0xF7);

	return msg;
}

bool An1xPatch::setBulkPatch(const std::vector<unsigned char>& bulkMessage)
{
	constexpr static int msgSize = 1953;

	if (bulkMessage.size() != msgSize) return false;

	constexpr static int headerSize = 9;

	for (int i = 0; i < PatchSize; i++)
	{
		m_data[i] = bulkMessage[i + headerSize];
	}

	return true;
}

int An1xPatch::getParameter(AN1x::ParamType type, unsigned char param) const
{
	if (AN1x::isNull(type, param)) return 0;

	auto param_ptr = getParameterAddress(type, param);

	int value = param_ptr[0];

	if (AN1x::isTwoByteParameter(type, param))
	{
		value = param_ptr[0] * 128;
		value += param_ptr[1];
	}

	value -= AN1x::getOffset(type, param);

	return value;

}

std::vector<int> An1xPatch::getTrackData() const
{
	auto param_ptr = getParameterAddress(AN1x::ParamType::Common, AN1x::FreeEgData);

	std::vector<int> result;

	static constexpr int allTracksSize = 8 * 192;

	for (int i = 0;  i < allTracksSize; i++)
	{
		bool negative = param_ptr[i] == 0;
		
		i++;

		int value = param_ptr[i];

		if (negative) value -= 128;

		result.push_back(value);


	}

	return result;
}
