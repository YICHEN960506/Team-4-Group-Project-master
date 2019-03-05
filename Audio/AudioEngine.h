#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#pragma comment(lib,"fmod_vc.lib")

using namespace std;

namespace Audio{
	struct Vector3 {
		float x;
		float y;
		float z;

		Vector3(float nx, float ny, float nz) { x = nx; y = ny; z = nz; }
	};
}


//该Implementation结构将包含我们对FMOD API的大部分调用。
//我们将这些调用和实际的音频引擎类本身分开来尝试防止任何奇怪的错误弹出。
//该结构将包含用于初始化和关闭FMOD引擎的代码，以及用于保存FMOD的Studio和低级系统对象的实例。
//实施还将包含我们在项目中播放的所有声音和事件的地图。
//除了所有对象都链接到键之外，映射与数组或向量类似。
//在这种情况下，我们的事件/声音的文件名将是将返回声音或事件的键。
//结构最后要做的就是调用FMOD更新来更新所有事件和声音的状态。
struct Implementation {
	Implementation();
	~Implementation();

	void Update();

	FMOD::Studio::System* mpStudioSystem;
	FMOD::System* mpSystem;

	int mnNextChannelId;

	typedef map<string, FMOD::Sound*> SoundMap;
	typedef map<int, FMOD::Channel*> ChannelMap;
	typedef map<string, FMOD::Studio::EventInstance*> EventMap;
	typedef map<string, FMOD::Studio::Bank*> BankMap;
	BankMap mBanks;
	EventMap mEvents;
	SoundMap mSounds;
	ChannelMap mChannels;
};

//struct Channel
//{
//	Channel(Implementation& tImplementation, int nSoundId, const AudioEngine::SoundDefinition& tSoundDefinition, const Vector3& vPosition, float fVolumedB);
//
//	enum class State
//	{
//		INITIALIZE,TOPLAY,LOADING,PLAYING,STOPPING,STOPPED,VIRTUALIZING,VIRTUAL,DEVIRTUALIZE,
//	};
//
//	Implementation& mImplementation;
//	FMOD::Channel*mpChannel = nullptr;
//	int mSoundId;
//	Vector3 mvPosition;
//	float mfVolumedB = 0.0f;
//	float mfSoundVolume = 0.0f;
//	State meState = State::INITIALIZE;
//	bool mbStopRequsted = false;
//	AudioFader mStopFader;
//	AudioFader mVirtualizeFader;
//
//	void Update(float fTimeDeltaSeconds);
//	void UpdateChannelParameters();
//	bool ShouldBeVirtual(bool bAllowOneShotVirtuals)const;
//	bool IsPlaying()const;
//	float GetVolumedB()const;
//};
//引擎类将调用Implementationstruct来启动，停止和更新FMOD。
//引擎还将处理基本的事情，如加载，播放，停止和更新声音和事件的信息。
class AudioEngine {
public:
	static void Init();
	static void Update();
	static void Shutdown();
	static int ErrorCheck(FMOD_RESULT result);



	void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	void LoadEvent(const std::string& strEventName);
	void LoadSound(const string &strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
	void UnLoadSound(const string &strSoundName);
	void Set3dListenerAndOrientation(const Audio::Vector3& vPosition, const Audio::Vector3& vLook, const Audio::Vector3& vUp);
	int PlaySounds(const string &strSoundName, const Audio::Vector3& vPos = Audio::Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f);
	void PlayEvent(const string &strEventName);
	void StopChannel(int nChannelId);
	void StopEvent(const string &strEventName, bool bImmediate = false);
	void GetEventParameter(const string &strEventName, const string &strEventParameter, float* parameter);
	void SetEventParameter(const string &strEventName, const string &strParameterName, float fValue);
	void StopAllChannels();
	void SetChannel3dPosition(int nChannelId, const Audio::Vector3& vPosition);
	void SetChannelVolume(int nChannelId, float fVolumedB);
	bool IsPlaying(int nChannelId) const;
	bool IsEventPlaying(const string &strEventName) const;
	float dbToVolume(float dB);
	float VolumeTodB(float volume);
	FMOD_VECTOR VectorToFmod(const Audio::Vector3& vPosition);
	//void Distance
};

#endif
