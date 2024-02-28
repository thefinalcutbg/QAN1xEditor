#include "An1x.h"
#include <array>


unsigned char AN1x::getScene(bool isScene2)
{
	return isScene2 ? 0x11 : 0x10;
}

Message AN1x::getHeader(ParamType p)
{

	switch (p)
	{
		case ParamType::System: return { 0xF0, 0x43, 0x10, 0x5C, 0x00, 0x00 };
		case ParamType::Common: return { 0xF0, 0x43, 0x10, 0x5C, 0x10, 0x00 };
		case ParamType::Scene1: return { 0xF0, 0x43, 0x10, 0x5C, 0x10, 0x10 };
		case ParamType::Scene2: return { 0xF0, 0x43, 0x10, 0x5C, 0x10, 0x11 };
		case ParamType::StepSq: return { 0xF0, 0x43, 0x10, 0x5C, 0x10, 0x0E };
        default: break;
	}

	return {};
}

int AN1x::getOffset(ParamType t, int parameter)
{

	switch (t)
	{
		case ParamType::System:
		{
			static const std::map<int, int> parameterToOffset{
				//{MasterTune, 341},
				{KeyboardTranspose, 64}

			};

			if (parameterToOffset.count(parameter))
			{
				return parameterToOffset.at(parameter);
			}
		}
			break;

		case ParamType::Scene1:
		case ParamType::Scene2:
		{
			static const std::map<int, int> parameterToOffset{
			{PEGSwitch, 1},
			{SyncPitchModSwitch, 1},
			{PEGDepth, 64},
			{PEGDecay, 64},
			{SyncPitch, 64},
			{SyncPitchDepth, 64},
			{FMDepth, 64},
			{VCO1Pitch, 64},
			{VCO2Pitch, 64},
			{VCO1Fine, 64},
			{VCO2Fine, 64 },
			{VCO1PWMDepth, 64},
			{VCO2PWMDepth, 64},
			{AmpVelSens, 64},
			{AmpModDepth, 64},
			{FilterModDepth, 64},
			{FilterVelSens, 64},
			{FilterKeyTrack, 32},
			{FilterResonance, 25},
			{LFO1Speed, -1},
			{LFO2Speed, -1},
			{VCO1PitchModDepth, 128},
			{VCO2PitchModDepth, 128},
			{FEGDepth, 128}
			};

			if (parameterToOffset.count(parameter))
			{
				return parameterToOffset.at(parameter);
			}

			break;
		}
		case ParamType::Common:
		{
			static const std::map<int, int> parameterToOffset{
				{SceneSelect, 1},
				{FreeEGKbdTrack, 64},
				{FreeEGLength, 2},
				{ArpSeqScene, 1}
			};

			if (parameterToOffset.count(parameter))
			{
				return parameterToOffset.at(parameter);
			}
			
		}
		break;
        default: break;
	}

	return 0;
}

bool AN1x::isNull(ParamType t, int p)
{
	static const std::set<int> s_commonNull
	{
		AN1x::CommonParam::NullCommon1,
		AN1x::CommonParam::reserved,
		AN1x::CommonParam::NullCommon3,
		AN1x::CommonParam::NullCommon4,
		AN1x::CommonParam::NullCommon5,
		AN1x::CommonParam::NullCommon6,
		AN1x::CommonParam::NullCommon7,
		AN1x::CommonParam::NullCommon8,
		AN1x::CommonParam::NullCommon9,
		AN1x::CommonParam::NullCommon10,
		AN1x::CommonParam::NullCommon11,
		AN1x::CommonParam::NullCommon12,
		AN1x::CommonParam::NullCommon13,
		AN1x::CommonParam::NullCommon14,
		AN1x::CommonParam::NullCommon15,
		AN1x::CommonParam::NullCommon16,
		AN1x::CommonParam::NullCommon17,
		AN1x::CommonParam::NullCommon18,
		AN1x::CommonParam::NullCommon19,
		AN1x::CommonParam::NullCommon20,
		AN1x::CommonParam::NullCommon21,
		AN1x::CommonParam::NullCommon22,
		AN1x::CommonParam::NullCommon23,
		AN1x::CommonParam::NullCommon24
	};

	static const std::set<int> s_sceneNull
	{
		AN1x::SceneParam::NullScene1,
		AN1x::SceneParam::NullScene2,
		AN1x::SceneParam::NullScene3,
		AN1x::SceneParam::NullScene4,
		AN1x::SceneParam::NullScene5,
		AN1x::SceneParam::reserve
	};

	switch (t)
	{
		case ParamType::System:
			return p == AN1x::NullCommon1;
		case ParamType::Scene1:
		case ParamType::Scene2:
			return s_sceneNull.count(p);
		case ParamType::Common:
			return s_commonNull.count(p);
		case ParamType::StepSq:
			return p == AN1x::SeqNull1 || p == AN1x::SeqNull2;
        default: break;
	}
	return false;
}

bool AN1x::isTwoByteParameter(ParamType t, int p)
{
	switch (t)
	{
		case ParamType::System:
			return p == AN1x::MasterTune;
		case ParamType::Common:
			return isNull(ParamType::Common, (AN1x::CommonParam)(p + 1)) && p + 1 != CommonParam::reserved;
		case ParamType::Scene1:
		case ParamType::Scene2:
			return isNull(ParamType::Scene1, (AN1x::SceneParam)(p + 1)) && p + 1 != SceneParam::reserve;
		case ParamType::StepSq:
			return false; //everything is 1 byte
        default: break;
	}

	return false;
}

const char* unknown = "???";

const char* AN1x::getFrequencyByValue(int value)
{
	if (value < 0 || value > 60) return unknown;

	static std::array<const char*, 61> fq{
		"THRU(20 Hz)",
		"22 Hz",
		"25 Hz",
		"28 Hz",
		"32 Hz",
		"36 Hz",
		"40 Hz",
		"45 Hz",
		"50 Hz",
		"56 Hz",
		"63 Hz",
		"70 Hz",
		"80 Hz",
		"90 Hz",
		"100 Hz",
		"110 Hz",
		"125 Hz",
		"140 Hz",
		"160 Hz",
		"180 Hz",
		"200 Hz",
		"225 Hz",
		"250 Hz",
		"280 Hz",
		"315 Hz",
		"355 Hz",
		"400 Hz",
		"450 Hz",
		"500 Hz",
		"560 Hz",
		"630 Hz",
		"700 Hz",
		"800 Hz",
		"900 Hz",
		"1.0 kHz",
		"1.1 kHz",
		"1.2 kHz",
		"1.4 kHz",
		"1.6 kHz",
		"1.8 kHz",
		"2.0 kHz",
		"2.2 kHz",
		"2.5 kHz",
		"2.8 kHz",
		"3.2 kHz",
		"3.6 kHz",
		"4.0 kHz",
		"4.5 kHz",
		"5.0 kHz",
		"5.6 kHz",
		"6.3 kHz",
		"7.0 kHz",
		"8.0 kHz",
		"9.0 kHz",
		"10.0 kHz",
		"11.0 kHz",
		"12.0 kHz",
		"14.0 kHz",
		"16.0 kHz",
		"18.0 kHz",
		"THRU(20.0 kHz)"
	};

	return fq[value];
}

const char* AN1x::getLFOFreqByValue(int value)
{
	if (value < 0 || value > 159) return unknown;

	static std::array<const char*, 160> fq{
		"0 Hz",
		"0.08 Hz","0.17 Hz","0.25 Hz","0.34 Hz","0.42 Hz","0.51 Hz","0.59 Hz","0.67 Hz","0.76 Hz","0.84 Hz",
		"0.93 Hz","1.01 Hz","1.09 Hz","1.18 Hz","1.26 Hz","1.35 Hz","1.43 Hz","1.52 Hz","1.6 Hz" ,"1.68 Hz",
		"1.77 Hz","1.85 Hz","1.94 Hz","2.02 Hz","2.1 Hz" ,"2.19 Hz","2.27 Hz","2.36 Hz","2.44 Hz","2.53 Hz",
		"2.61 Hz","2.69 Hz","2.78 Hz","2.86 Hz","2.95 Hz","3.03 Hz","3.12 Hz","3.2 Hz" ,"3.28 Hz","3.37 Hz",
		"3.45 Hz","3.54 Hz","3.62 Hz","3.7 Hz" ,"3.79 Hz","3.87 Hz","3.96 Hz","4.04 Hz","4.13 Hz","4.21 Hz",
		"4.29 Hz","4.38 Hz","4.46 Hz","4.55 Hz","4.63 Hz","4.71 Hz","4.8 Hz" ,"4.88 Hz","4.97 Hz","5.05 Hz",
		"5.14 Hz","5.22 Hz","5.3 Hz" ,"5.39 Hz","5.47 Hz","5.56 Hz","5.64 Hz","5.72 Hz","5.81 Hz","5.89 Hz",
		"5.98 Hz","6.06 Hz","6.15 Hz","6.23 Hz","6.31 Hz","6.4 Hz" ,"6.48 Hz","6.57 Hz","6.65 Hz","6.74 Hz",
		"6.82 Hz","6.9 Hz" ,"6.99 Hz","7.07 Hz","7.16 Hz","7.24 Hz","7.32 Hz","7.41 Hz","7.49 Hz","7.58 Hz",
		"7.66 Hz","7.75 Hz","7.83 Hz","7.91 Hz","8 Hz"   ,"8.08 Hz","8.17 Hz","8.25 Hz","8.33 Hz","8.42 Hz",
		"8.5 Hz" ,"8.59 Hz","8.67 Hz","8.76 Hz","8.84 Hz","8.92 Hz","9.01 Hz","9.09 Hz","9.18 Hz","9.26 Hz",
		"9.68 Hz","10.11 Hz","10.61 Hz","11.44 Hz","12.11 Hz","12.78 Hz","13.45 Hz","14.13 Hz","14.8 Hz","15.47 Hz",
		"16.14 Hz","16.82 Hz","17.49 Hz","18.16 Hz","18.83 Hz","19.51 Hz","20.18 Hz","20.85 Hz","21.52 Hz","22.2 Hz",
		"22.87 Hz","23.54 Hz","24.21 Hz", "24.89 Hz","25.56 Hz","26.23 Hz","26.9 Hz","27.58 Hz","28.25 Hz","28.92 Hz",
		"29.59","30.27 Hz","30.94 Hz","31.61 Hz","32.28 Hz","32.96 Hz","33.63 Hz","34.3 Hz","34.97 Hz","35.65 Hz",
		"36.32","36.99 Hz","37.67 Hz","38.34 Hz","39.01 Hz","39.68 Hz","40.36 Hz","41.03 Hz","41.7 Hz"
	};

	return fq[value];
}

const char* AN1x::getChorusTypeByValue(int value)
{
	if (value < 0 || value > 16) return unknown;

	static std::array<const char*, 16> chorus{
		"Off",
		"1xSoft",
		"1xMiddle",
		"1xHard",
		"2xSoft",
		"2xMiddle",
		"2xHard",
		"4xSoft",
		"4xMiddle",
		"4xHard",
		"8xSoft",
		"8xMiddle",
		"8xHard",
		"Rnd Soft",
		"Rnd Middle",
		"Rnd Hard",
	};

	return chorus[value];
}

int AN1x::compressorAttack(int value)
{
	if (value < 0 || value > 19) return -1;

	static int values[] = { 1,2,3,4,5,6,7,8,9,10,12,14,16,18,20,23,26,30,35,40 };

	return values[value];
}

int AN1x::compressorRelease(int value)
{
	if (value < 0 || value > 15) return -1;

	static int values[] = { 10,15,25,35,45,55,65,75,85,100,115,140,170,230,340,680 };

	return values[value];
}

const char* AN1x::compressorRatio(int value)
{
	if (value < 0 || value > 7) return unknown;

	static std::array<const char*, 16> values{ "1.0","1.5","2.0","3.0","5.0","7.0","10.0","20.0" };

	return values[value];
}

const char* AN1x::wahCutoffFreq(int value)
{
		if (value < 0 || value > 39) return unknown;

		static std::array<const char*, 40> frq{
			"20",
			"32","45","63","80","100","125","160","200","250","280",
			"315","350","400","500","560","630","700","800","1.0k","1.2k",
			"1.4k","1.6k","1.8k","2.0k","2.2k","2.5k","2.8k","3.2k","3.6k","4.0k",
			"5.0k","5.6k","6.3k","7.0k","8.0k","9.0k","10.0k","12.0k","14.0k"
		};

		return frq[value];

}

const char* AN1x::reverbTime(int value)
{
	if (value < 0 || value > 69) return unknown;

	static std::array<const char*, 70> time{
		"0.3","0.4","0.5","0.6","0.7","0.8","0.9",
		"1.0","1.1","1.2","1.3","1.4","1.5","1.6","1.7","1.8","1.9",
		"2.0","2.1","2.2","2.3","2.4","2.5","2.6","2.7","2.8","2.9",
		"3.0","3.1","3.2","3.3","3.4","3.5","3.6","3.7","3.8","3.9",
		"4.0","4.1","4.2","4.3","4.4","4.5","4.6","4.7","4.8","4.9",
		"5.0","5.5","6.0","6.5","7.0","7.5","8.0","8.5","9.0","9.5",
		"10.0","11.0","12.0","13.0","14.0","15.0","16.0","17.0","18.0","19.0","20.0",
		"25.0","30.0"
	};

	return time[value];
}

std::string AN1x::getMatrixSourceName(int value)
{
	if (value < 0 || value > 114) return "???";

	if (!value) return "Off";

	if (value < 96) return std::to_string(value);

	static const char* named[11] =
	{
		"Aftertouch",
		"Pitch Bend",
		"Key Track",
		"Velocity",
		"Key Random",
		"Mod Wheel",
		"Foot Volume",
		"Foot Control",
		"Foot Switch",
		"Ribbon X",
		"Ribbon Y"
	};

	if (value > 95 && value < 107) return named[value-96];

	return std::string("Assign Knob ") + std::to_string(value - 106);
}

const char* AN1x::getMatrixParamName(int value)
{
	if (value < 0 || value > 45) return unknown;

	static std::array<const char*, 46> values{ 
		"Off",
		"Scene Tune",
		"Pitch Up",
		"Pitch Down",
		"PEG Decay",
		"PEG Depth",
		"Portamento Time",
		"LFO1 Speed",
		"LFO1 Delay",
		"LFO2 Speed",
		"Sync Pitch",
		"Sync Depth",
		"FM Depth",
		"VCO1 Pitch",
		"VCO1 Fine",
		"VCO1 Edge",
		"VCO1 Pulse Width",
		"VCO1 PW Depth",
		"VCO1 LFO Mod Depth",
		"VCO2 Pitch",
		"VCO2 Fine",
		"VCO2 Edge",
		"VCO2 Pulse Width",
		"VCO2 PW Depth",
		"VCO2 LFO Mod Depth",
		"VCO1 Level",
		"VCO2 Level",
		"Ring Modulation",
		"Nouse Level",
		"FEG Attack",
		"FEG Decay",
		"FEG Sustain",
		"FEG Release",
		"HPF Cutoff",
		"VCF Cutoff",
		"Resonance",
		"FEG Depth",
		"VCF LFO Mod Depth",
		"AEG Attack",
		"AEG Decay",
		"AEG Sustain",
		"AEG Release",
		"VCA Feedback",
		"VCA Volume",
		"VCA LFO Mod Depth",
		"Vari FX D:W"
	};

	return values[value];
}

const char* AN1x::getMatrixCommonParamName(int value)
{
	if (value < 0 || value > 5) return unknown;

	static std::array<const char*, 6> values{
		"Off",
		"Volume",
		"Pan",
		"Variable FX",
		"Delay",
		"Reverb"
	};

	return values[value];
}

const char* AN1x::getFreeEGParameters(int value)
{
	if (value < 0 || value > 56) return unknown;

	static std::array<const char*, 57> values{
			"Off",
			"PEG Decay",
			"PEG Depth",
			"PEG Switch",
			"Portamento Time",
			"LFO1 Wave",
			"LFO1 Speed",
			"LFO1 Delay",
			"LFO2 Speed",
			"Sync Algorithm",
			"Sync Pitch",
			"Sync Pitch Depth",
			"Synch Pitch Source",
			"Sync PMod",
			"FM Depth",
			"FM Source 1",
			"FM Source 2",
			"VCO1 Wave",
			"VCO1 Pitch",
			"VCO1 Fine",
			"VCO1 Edge",
			"VCO1 Pulse Width",
			"VCO1 PW Mod Depth",
			"VCO1 PW Mod Source",
			"VCO1 LFO Mod Depth",
			"VCO2 Wave",
			"VCO2 Pitch",
			"VCO2 Fine",
			"VCO2 Edge",
			"VCO2 Pulse Width",
			"VCO2 PW Mod Depth",
			"VCO2 PW Mod Source",
			"VCO2 LFO Mod Depth",
			"VCO1 Level",
			"VCO2 Level",
			"Ring Modulation",
			"Noise Level",
			"FEG Attack",
			"FEG Decay",
			"FEG Sustain",
			"FEG Release",
			"HPF Cutoff",
			"VCF Type",
			"VCF Cutoff",
			"Resonance",
			"FEG Depth",
			"FEG Vel.Sensitivity",
			"FEG Key Track",
			"VCF LFO Mod Depth",
			"AEG Attack",
			"AEG Decay",
			"AEG Sustain",
			"AEG Release",
			"VCA Feedback",
			"VCA Volume",
			"AEG Vel.Sensitivity",
			"VCA LFO Mod Depth"
	};

	return values[value];
}

const char* AN1x::getFreeEGLength(int value)
{
	if (value < 0 || value > 94) return unknown;

	static std::array<const char*, 95> values{
			"1/2 bar",
			"1 bar",
			"3/2 bar",
			"2 bars",
			"3 bars",
			"4 bars",
			"6 bars",
			"8 bars",
			"1.0 second","1.1 second","1.2 second","1.3 second","1.4 second","1.5 second","1.6 second","1.7 second","1.8 second","1.9 second",
			"2.0 seconds","2.1 seconds","2.2 seconds","2.3 seconds","2.4 seconds","2.5 seconds","2.6 seconds","2.7 seconds","2.8 seconds","2.9 seconds",
			"3.0 seconds","3.1 seconds","3.2 seconds","3.3 seconds","3.4 seconds","3.5 seconds","3.6 seconds","3.7 seconds","3.8 seconds","3.9 seconds",
			"4.0 seconds","4.1 seconds","4.2 seconds","4.3 seconds","4.4 seconds","4.5 seconds","4.6 seconds","4.7 seconds","4.8 seconds","4.9 seconds",
			"5.0 seconds","5.1 seconds","5.2 seconds","5.3 seconds","5.4 seconds","5.5 seconds","5.6 seconds","5.7 seconds","5.8 seconds","5.9 seconds",
			"6.0 seconds","6.1 seconds","6.2 seconds","6.3 seconds","6.4 seconds","6.5 seconds","6.6 seconds","6.7 seconds","6.8 seconds","6.9 seconds",
			"7.0 seconds","7.1 seconds","7.2 seconds","7.3 seconds","7.4 seconds","7.5 seconds","7.6 seconds","7.7 seconds","7.8 seconds","7.9 seconds",
			"8.0 seconds","8.5 seconds","9.0 seconds","9.5 seconds","10.0 seconds","10.5 seconds","11.0 seconds","11.5 seconds","12.0 seconds","12.5 seconds",
			"13.0 seconds","13.5 seconds","14.0 seconds","14.5 seconds","15.0 seconds","15.5 seconds","16.0 seconds"
	};

	return values[value];
}

std::vector<unsigned char> AN1x::voiceRequest(int voiceNumber)
{
	if (voiceNumber < 0 || voiceNumber > 127) return {};

	unsigned char penultimateByte = 0;

	if (voiceNumber <= 111) {
		penultimateByte = 111 - voiceNumber;
	}
	else {
		penultimateByte = 127 - voiceNumber + 112;
	}

	return { 0xF0, 0x43, 0x20, 0x5C, 0x11, (unsigned char)voiceNumber, 00, penultimateByte, 0xF7 };
}


void AN1x::addCheckSum(std::vector<unsigned char>& message)
{
	unsigned char sum = 0x00;

    for (size_t i = 4; i < message.size(); i++) {

		sum += message[i]; //intended arithmetic overflow
	}

	unsigned char checkSum = (~sum + 1) & 0x7F;

	message.push_back(checkSum);

	message.push_back(0xF7);
}
