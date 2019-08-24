#ifndef _cAudioSystem_HG_
#define _cAudioSystem_HG_

#include "iSystem.h"
#include <FMOD\fmod.hpp>
#include <FMOD\fmod_errors.h>

#include <OpenGL\OpenGLHeaders.h>
#include <Entity\Components\iComponent.h>
#include <vector>

class cAudioSystem : public iSystem
{
public:
	cAudioSystem();
	~cAudioSystem();

	bool SetBasePath(std::string pathName);
	bool LoadSounds();

	void PlaySound(iComponent* audioComponent);
	void PlaySoundAt(int index, const std::string& soundType, glm::vec3 position, glm::vec3 velocity);
	void PlayOneShot();

	void ProcessAudioKeys(GLFWwindow* window);

	// Updates the system
	virtual void UpdateSystem(double deltaTime);

	// Schedules and unschedule specific components
	// in order to save searching memory
	virtual bool ScheduleEntity(sEntity* entity, iComponent* component);
	virtual bool UnscheduleEntity(sEntity* entity, iComponent* component);

	bool bChooseCompressed;
	bool bUseCompressed;

	FMOD::ChannelGroup* masterChannelGroup;
	FMOD::ChannelGroup* BGMsChannelGroup;
	FMOD::ChannelGroup* SFXsChannelGroup;

	std::vector<FMOD::Sound*> BGMs;
	std::vector<FMOD::Sound*> SFXs;

	//std::vector<sSound*> GetBGMs();
	//std::vector<sSound*> GetSFXs();

	/*FMOD::ChannelGroup* masterGroupChannel = 0;
	FMOD::ChannelGroup* BGMGroupChannel = 0;
	FMOD::ChannelGroup* SFXGroupChannel = 0;

	FMOD::ChannelGroup* tempGroupChannel_1 = 0;
	FMOD::ChannelGroup* tempGroupChannel_2 = 0;*/

	// Play a sound by the name
	

	// Test for new 3D sounds
	FMOD_VECTOR audioListenerPosition;
	FMOD_VECTOR audioListenerVelocity;
	FMOD_VECTOR audioListenerForward;
	FMOD_VECTOR audioListenerUp;

	//void UpdateAudioListener(cMeshObject* audioListener);

	//cSoundVoice* _pSV;

private:
	FMOD_RESULT _result; // Checks for the program's state
	FMOD::System* _system;

	std::string _basePath;

	// Begin and shutdown FMOD
	void _InitAudioSystem();
	bool _InitFMOD();
	bool _ShutdownFMOD();

	// Check for errors
	void _ErrorCallback();

	void ChooseCompressedSound();

	void _CreateChannelGroups();
	bool _CreateSound(std::string soundName, std::string soundType, bool is3D);
};


#endif // !_cAudioSystem_HG_

