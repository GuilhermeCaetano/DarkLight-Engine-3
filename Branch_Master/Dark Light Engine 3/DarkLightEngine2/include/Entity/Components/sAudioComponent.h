#ifndef _sAudioComponent_HG_
#define _sAudioComponent_HG_

#include <Entity\Components\iComponent.h>
#include <OpenGL\OpenGLHeaders.h>

#include <FMOD\fmod.hpp>
#include <FMOD\fmod_errors.h>


struct sAudioComponent : public iComponent
{
public:
	sAudioComponent(unsigned long long entityID);
	~sAudioComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "AUDIO_COMPONENT"; };

	void SetSoundToPlay(const std::string& filename);

	void SetSoundShouldPlay(bool soundShouldPlay);
	bool GetShouldPlaySound();

	glm::vec2 minMax3DDistance;

	std::string filename;
	std::string fileType;

	float volume;
	float pitch;
	float pan;
	float frequency;

	FMOD::Sound* sound;

	bool bPlayOnStart;
	bool bIsPlaying;
	bool bLoop;

	// New for 3D sounds
	//FMOD::Channel* schannel;
	//FMOD_VECTOR schannelPosition = { 0.0f, 0.0f, 0.0f };
	//FMOD_VECTOR schannelVelocity = { 0.0f, 0.0f, 0.0f };

protected:
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

	bool _bShouldPlay;

};


#endif // !_sAudioComponent_HG_

