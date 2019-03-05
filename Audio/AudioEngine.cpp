#include "AudioEngine.h"

//这是我们将初始化基础FMOD系统的地方，这将允许我们播放声音。
//ErrorCheck是我们检查所有FMOD是否调用成功的方式。
//FMOD_STUDIO_INIT_LIVEUPDATE可以用FMOD Studio链接到游戏并且实时混合音频。
Implementation::Implementation() {
	mpStudioSystem = NULL;
	AudioEngine::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
	AudioEngine::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

	mpSystem = NULL;
	AudioEngine::ErrorCheck(mpStudioSystem->getLowLevelSystem(&mpSystem));
}

//关闭FMOD
Implementation::~Implementation() {
	AudioEngine::ErrorCheck(mpStudioSystem->unloadAll());
	AudioEngine::ErrorCheck(mpStudioSystem->release());
}

//检查一个频道是否已经停止播放，如果已经通知播放，就销毁，以便清理一个要使用的频道。
//我们只需调用FMOD系统上的更新功能来更新事件声音
void Implementation::Update() {
	vector<ChannelMap::iterator> pStoppedChannels;//检查声音是否完成。
	for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
	{
		bool bIsPlaying = false;
		it->second->isPlaying(&bIsPlaying);
		if (!bIsPlaying)
		{
			pStoppedChannels.push_back(it);
		}
	}
	for (auto& it : pStoppedChannels)//从频道地图中删除已完成的频道。
	{
		mChannels.erase(it);
	}
	AudioEngine::ErrorCheck(mpStudioSystem->update());
}

Implementation* sgpImplementation = nullptr;

//创建Implementtation并且调用其更新
void AudioEngine:: Init()
{
	sgpImplementation = new Implementation;
}


void AudioEngine::Update() 
{
	sgpImplementation->Update();
}

//接受文件名和有关的流，循环以及它是否是3D声音的一些参数，然后加载声音并将其存储在我们的声音映射中。
void AudioEngine::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
{
	auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);//检查声音是否加载
	if (tFoundIt != sgpImplementation->mSounds.end())
		return;

	FMOD_MODE eMode = FMOD_DEFAULT;
	eMode |= b3d ? FMOD_3D : FMOD_2D;
	eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* pSound = nullptr;
	AudioEngine::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));//加载声音。
	if (pSound) 
	{
		sgpImplementation->mSounds[strSoundName] = pSound;//后备存储中添加声音。
	}

}

//卸载声音释放内存。输入文件名，在声音图中查找并且释放声音。
void AudioEngine::UnLoadSound(const std::string& strSoundName)
{
	auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);//检查声音是否加载
	if (tFoundIt == sgpImplementation->mSounds.end())
		return;

	AudioEngine::ErrorCheck(tFoundIt->second->release());//卸载声音。
	sgpImplementation->mSounds.erase(tFoundIt);//后备存储中删除声音。
}

//首先看看声音图中是否有声音。
//如果没有，加载它。
//如果仍然找不到那么这就意味着出了问题，无法发挥声音。
//如果发现声音很好，那么创建一个新的声道来控制声音并播放声音，但开始声音暂停。
//这样在设置参数时就不会弹出音频。
//如果频道设置正确，那么我们更新所有可能的参数，
//如音量和位置，然后取消暂停声音。最后返回稍后引用的频道ID封装。
int AudioEngine::PlaySounds(const string& strSoundName, const Audio::Vector3& vPosition, float fVolumedB)
{
	int nChannelId = sgpImplementation->mnNextChannelId++;
	auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);//检查声音是否加载。
	if (tFoundIt == sgpImplementation->mSounds.end())//
	{
		LoadSound(strSoundName);//如果没有，就加载声音。
		tFoundIt = sgpImplementation->mSounds.find(strSoundName);
		if (tFoundIt == sgpImplementation->mSounds.end())
		{
			return nChannelId; 
		}//加载出现错误，提前退出。
	}
	FMOD::Channel* pChannel = nullptr;//播放声音。
	AudioEngine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
	if (pChannel)
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);
		if (currMode & FMOD_3D) {
			FMOD_VECTOR position = VectorToFmod(vPosition);
			AudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
		}
		AudioEngine::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
		AudioEngine::ErrorCheck(pChannel->setPaused(false));
		sgpImplementation->mChannels[nChannelId] = pChannel;
	}
	return nChannelId;
}

void AudioEngine::StopChannel(int nChannelId) 
{

}

//允许设置声音的位置
void AudioEngine::SetChannel3dPosition(int nChannelId, const Audio::Vector3& vPosition)
{
	auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
	if (tFoundIt == sgpImplementation->mChannels.end())
		return;

	FMOD_VECTOR position = VectorToFmod(vPosition);
	AudioEngine::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

//允许设置声音的音量
void AudioEngine::SetChannelVolume(int nChannelId, float fVolumedB)
{
	auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
	if (tFoundIt == sgpImplementation->mChannels.end())
		return;

	AudioEngine::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}

//加载bank。ban存储每个事件的所有声音和信息。和加载声音一样。
void AudioEngine::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
	auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
	if (tFoundIt != sgpImplementation->mBanks.end())
		return;
	FMOD::Studio::Bank* pBank;
	AudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
	if (pBank) {
		sgpImplementation->mBanks[strBankName] = pBank;
	}
}

//加载时间。存储在bank里面的每个事件都必须单独加载，这样有助于节省内存。
void AudioEngine::LoadEvent(const std::string& strEventName) {
	auto tFoundit = sgpImplementation->mEvents.find(strEventName);
	if (tFoundit != sgpImplementation->mEvents.end())
		return;
	FMOD::Studio::EventDescription* pEventDescription = NULL;
	AudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
	if (pEventDescription) {
		FMOD::Studio::EventInstance* pEventInstance = NULL;
		AudioEngine::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
		if (pEventInstance) {
			sgpImplementation->mEvents[strEventName] = pEventInstance;
		}
	}
}

//播放事件。看看事件有没有被加载，如果没有就让其播放。
void AudioEngine::PlayEvent(const string &strEventName) {
	auto tFoundit = sgpImplementation->mEvents.find(strEventName);
	if (tFoundit == sgpImplementation->mEvents.end()) {
		LoadEvent(strEventName);
		tFoundit = sgpImplementation->mEvents.find(strEventName);
		if (tFoundit == sgpImplementation->mEvents.end())
			return;
	}
	tFoundit->second->start();
}

//做同样的事情来停止事件。除非我们不关心事件是否加载。
void AudioEngine::StopEvent(const string &strEventName, bool bImmediate) {
	auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	if (tFoundIt == sgpImplementation->mEvents.end())
		return;

	FMOD_STUDIO_STOP_MODE eMode;
	eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
	AudioEngine::ErrorCheck(tFoundIt->second->stop(eMode));
}

//查看某个事件是否正在播放。取得事件的回放状态。
bool AudioEngine::IsEventPlaying(const string &strEventName) const {
	auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	if (tFoundIt == sgpImplementation->mEvents.end())
		return false;

	FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
	if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
		return true;
	}
	return false;
}

//动态获取和设置事件参数。
void AudioEngine::GetEventParameter(const string &strEventName, const string &strParameterName, float* parameter) {
	auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	if (tFoundIt == sgpImplementation->mEvents.end())
		return;

	FMOD::Studio::ParameterInstance* pParameter = NULL;
	AudioEngine::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
	AudioEngine::ErrorCheck(pParameter->getValue(parameter));
}

void AudioEngine::SetEventParameter(const string &strEventName, const string &strParameterName, float fValue) {
	auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	if (tFoundIt == sgpImplementation->mEvents.end())
		return;

	FMOD::Studio::ParameterInstance* pParameter = NULL;
	AudioEngine::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
	AudioEngine::ErrorCheck(pParameter->setValue(fValue));
}

//线性音量转换为dB并且从Vector3转换成FMOD的Vector3
FMOD_VECTOR AudioEngine::VectorToFmod(const Audio::Vector3& vPosition) {
	FMOD_VECTOR fVec;
	fVec.x = vPosition.x;
	fVec.y = vPosition.y;
	fVec.z = vPosition.z;
	return fVec;
}

float  AudioEngine::dbToVolume(float dB)
{
	return powf(10.0f, 0.05f * dB);
}

float  AudioEngine::VolumeTodB(float volume)
{
	return 20.0f * log10f(volume);
}

//FMOD错误检查
int AudioEngine::ErrorCheck(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		cout << "FMOD ERROR " << result << endl;
		return 1;
	}
	// cout << "FMOD all good" << endl;
	return 0;
}

//清理所有内容（删除Implementation）。
void AudioEngine::Shutdown() {
	delete sgpImplementation;
}