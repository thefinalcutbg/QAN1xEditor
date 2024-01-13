#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>

namespace AN1x
{
	enum Layer : unsigned char{ Single, Unison, Dual, DualUnison, Split, SplitUnison };
	enum Pan : unsigned char{ Off, Alt, Round };
	enum VariFx : unsigned char { Chorus1, Chorus2, Flagner, Symphonic, Phaser, AutoPAN, RotarySpeaker, PitchChange, AuralExciter, Compressor, WahWah, Distortion, Overdrive, AmpSimulator };
	enum Delay : unsigned char { LCR, LR, Echo, Cross, TempoDelay };
	enum Reverb : unsigned char { Hall1, Hall2, Room1, Room2, Room3, Stage1, Stage2, Plate };
	
	enum class ParamType{
		System,
		Common,
		Scene1,
		Scene2,
		StepSq,
		Unknown
	};

	enum SystemParam
	{
		MasterTune,
		NullGlobal1,
		KeyboardTranspose,
		VelocityCurve,
		FixedVelocity,
		EffectBypass,
		TransmitChannel,
		ArpSeqTransmitChannel,
		RecieveChannel1,
		RecieveChannel2,
		MidiDeviceNumber,
		MidiLocal,
		SceenCtrlNumber,
		MWCtrlNumber,
		FVCtrlNumber,
		FCCtrlNumber,
		FSCtrlNumber,
		RibXCtrlNumber,
		RibZCtrlNumber,
		AssignKnob1Number,
		//x7 more 
		SystemReserved = 0x1B,
		SystemMaxSize
	};

	enum CommonParam
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
		FreeEGKbdTrack,
		FreeEgTrackParam1,
		FreeEgTrackSceneSw1,
		FreeEgTrackParam2,
		FreeEgTrackSceneSw2,
		FreeEgTrackParam3,
		FreeEgTrackSceneSw3,
		FreeEgTrackParam4,
		FreeEgTrackSceneSw4,
		FreeEgData,
		CommonMaxSize
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

	enum SeqParam
	{
		SeqBaseUnit = 0x00,
		SeqLength,
		SeqLoopType,
		SeqCtrlChange,
		SeqNull1,
		SeqNull2,
		SeqNote1 = 0x06,
		//15 more
		SeqVelocity1 = 0x16,
		//15 more
		SeqGateTime1 = 0x26,
		//15 more
		SeqCtrlChange1 = 0x36,
		//15 more
		StepSequencerMaxSize = 0x46
	};

	unsigned char getScene(bool isScene2);


	std::vector<unsigned char> getHeader(ParamType p);

	int getOffset(ParamType t, int p);
	bool isNull(ParamType t, int p);
	bool isTwoByteParameter(ParamType t, int p);

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
	const char* getMatrixCommonParamName(int value);
	const char* getFreeEGParameters(int value);
	const char* getFreeEGLength(int value);

	//generates MIDI message for getting perticular voice
	std::vector<unsigned char> voiceRequest(int voiceNumber);

}