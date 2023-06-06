#pragma once
#include <map>
#include <set>
#include <string>

namespace AN1x
{
	enum Layer : unsigned char{ Single, Unison, Dual, DualUnison, Split, SplitUnison };
	enum Pan : unsigned char{ Off, Alt, Round };
	enum VariFx : unsigned char { Chorus1, Chorus2, Flagner, Symphonic, Phaser, AutoPAN, RotarySpeaker, PitchChange, AuralExciter, Compressor, WahWah, Distortion, Overdrive, AmpSimulator };
	enum Delay : unsigned char { LCR, LR, Echo, Cross, TempoDelay };
	enum Reverb : unsigned char { Hall1, Hall2, Room1, Room2, Room3, Stage1, Stage2, Plate };
	
	enum CommonParam : unsigned char
	{
		Name1 = 0x00,
		Name2,
		Name3,
		Name4,
		Name5,
		Name6,
		Name7,
		Name8,
		Name9,
		Name10,
		VoiceCategory,
		SceneSelect,
		LayerMode,
		LayerPan,
		LayerSeparation,
		Detune,
		Tempo,
		NullCommon1,
		SplitPoint,
		Portamento,
		MatrixSrc1,
		MatrixParam1,
		MatrixDepth1,
		MatrixSrc2,
		MatrixParam2,
		MatrixDepth2,
		VariFXType,
		reserved,
		FxParam1,
		NullCommon3,
		FxParam2,
		NullCommon4,
		FxParam3,
		NullCommon5,
		FxParam4,
		NullCommon6,
		FxParam5,
		NullCommon7,
		FxParam6,
		NullCommon8,
		LowFq,
		LowGain,
		MidFq,
		MidGain,
		MidRes,
		HiFq,
		HiGain,
		DlyRevConnection,
		DlyType,
		DlyReturn,
		DlyParam1,
		NullCommon9,
		DlyParam2,
		NullCommon10,
		DlyParam3,
		NullCommon11,
		DlyParam4,
		NullCommon12,
		DlyParam5,
		NullCommon13,
		DlyParam6,
		NullCommon14,
		DlyParam7,
		NullCommon15,
		RevType,
		RevReturn,
		RevParam1,
		NullCommon16,
		RevParam2,
		NullCommon17,
		RevParam3,
		NullCommon18,
		RevParam4,
		NullCommon19,
		RevParam5,
		NullCommon20,
		RevParam6,
		NullCommon21,
		RevParam7,
		NullCommon22,
		ArpSeqOnOff,
		ArpSeqSelect,
		ArpTypeSeqNo,
		ArpSeqKbdMode,
		ArpSeqHold,
		ArpSeqScene,
		ArpSubdiv,
		Swing,
		Velocity,
		NullCommon23,
		Gate,
		NullCommon24,
		FreeEGTrigger,
		FreeEGLoopType,
		FreeEGLength,
		CommonParamSize
	};

	enum SceneParam : unsigned char
	{
		PolyMode = 0x00,
		PitchUpRange,
		PitchDownRange,
		PEGDecay,
		PEGDepth,
		PEGSwitch,
		PortamentoMode,
		PortamentoTime,
		LFOResetMode,
		LFO1Wave,
		LFO1Speed,
		NullScene1,
		LFODelay,
		LFO2Speed,
		NullScene2,
		FMAlgorithm,
		SyncPitch,
		SyncPitchDepth,
		SyncPitchSrc,
		SyncPitchModSwitch,
		FMDepth,
		FMSrc1,
		FMSrc2,
		VCO1Wave,
		VCO1Pitch,
		VCO1Fine,
		VCO1Edge,
		VCO1PW,
		VCO1PWMDepth,
		VCO1PWMSrc,
		VCO1PitchModDepth,
		NullScene3,
		VCO2Wave,
		VCO2Pitch,
		VCO2Fine,
		VCO2Edge,
		VCO2PW,
		VCO2PWMDepth,
		VCO2PWMSrc,
		VCO2PitchModDepth,
		NullScene4,
		MixerVCO1Level,
		MixerVCO2Level,
		MixerRingModLevel,
		MixerNoiseLevel,
		FilterAttack,
		FilterDecay,
		FilterSustain,
		FilterRelease,
		HPFCutoff,
		FilterType,
		FilterCutoff,
		FilterResonance,
		FEGDepth,
		NullScene5,
		FilterVelSens,
		FilterKeyTrack,
		FilterModDepth,
		AmpAttack,
		AmpDecay,
		AmpSustain,
		AmpRelease,
		Feedback,
		Volume,
		AmpVelSens,
		AmpModDepth,
		VariFxDW,
		reserve,
		CtrlMtrxSource1,
		CtrlMtrxParam1,
		CtrlMtrxDepth1,
		//repeat last 3 15 more times
		CtrlMtrxDepth16 = 0x73,
		SceneParametersMaxSize
	};

	unsigned char getScene(bool isScene2);


	//int getOffset(CommonParam p);
	int getOffset(SceneParam p);
	int getOffset(CommonParam p);

	bool isNull(SceneParam p);
	bool isNull(CommonParam p);

	bool isTwoByteParameter(SceneParam p);
	bool isTwoByteParameter(CommonParam p);

	const char* getFrequencyByValue(int value);
	const char* getLFOFreqByValue(int value);
	const char* getChorusTypeByValue(int value);
	int compressorAttack(int value);
	int compressorRelease(int value);
	const char* compressorRatio(int value);
	const char* wahCutoffFreq(int value);
	const char* reverbTime(int value);
	std::string getMatrixSourceName(int value);
	const char* getMatrixParamName(int value);
}