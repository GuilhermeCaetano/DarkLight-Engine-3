// cAudioSystem.cpp

#include <Entity\Systems\cAudioSystem.h>
#include <Pools\cPoolManager.h>
#include <Entity\Components\sAudioComponent.h>
#include <iostream>

cAudioSystem::cAudioSystem()
{
	this->_basePath = "";
	this->bChooseCompressed = false;
	this->bUseCompressed = false;

	this->masterChannelGroup = nullptr;
	this->BGMsChannelGroup = nullptr;
	this->SFXsChannelGroup = nullptr;

	this->_InitAudioSystem();

	return;
}

cAudioSystem::~cAudioSystem()
{
	this->_ShutdownFMOD();

	return;
}

void cAudioSystem::_InitAudioSystem()
{
	audioListenerPosition = { 0.0f, 0.0f, 0.0f };
	audioListenerUp = { 0.0f, 1.0f, 0.0f };
	audioListenerForward = { 0.0f, 0.0f, 1.0f };
	audioListenerVelocity = { 0.0f, 0.0f, 0.0f };

	// Initialize FMOD
	this->_InitFMOD();

	// Create channel groups
	this->_CreateChannelGroups();

	// Ask to use compressed or uncompressed if we want
	if (this->bChooseCompressed)
	{
		this->ChooseCompressedSound();
	}
}

bool cAudioSystem::_InitFMOD()
{
	// Create the main system object
	this->_result = FMOD::System_Create(&this->_system);
	if (this->_result != FMOD_OK) this->_ErrorCallback();

	// Initializes the system object, and the msound device. 
	// Must be called at the start of the user's program
	this->_result = this->_system->init(512, FMOD_INIT_NORMAL, NULL);
	if (this->_result != FMOD_OK) this->_ErrorCallback();

	return true;
}

bool cAudioSystem::SetBasePath(std::string pathName)
{
	struct stat info;
	if (stat(pathName.c_str(), &info) != 0)
	{
		printf("Path %s couldn't be accessed!\n", pathName.c_str());
		return false;
	}
	else if (info.st_mode& S_IFDIR)
	{
		printf("Shader path set.\n");
		this->_basePath = pathName;
		return true;
	}

	return false;
}

void cAudioSystem::ChooseCompressedSound()
{
	// Ask and wait for the type of sound
	char compressed = 'd';
	printf("Use compressed sound? (y/n)\n");
	std::cin >> compressed;

	while (compressed != 'y' && compressed != 'Y' && compressed != 'n' && compressed != 'N')
	{
		printf("Bad Input!\n");
		std::cin >> compressed;
	}

	if (compressed == 'y' || compressed == 'Y')
	{
		this->bUseCompressed = true;
	}
	else if (compressed == 'n' || compressed == 'N')
	{
		this->bUseCompressed = false;
	}
}

void cAudioSystem::_CreateChannelGroups()
{
	this->_result = this->_system->createChannelGroup("Master", &this->masterChannelGroup);
	if (this->_result != FMOD_OK) this->_ErrorCallback(); // Check for errors
	this->_result = this->_system->createChannelGroup("BGMs", &this->BGMsChannelGroup);
	if (this->_result != FMOD_OK) this->_ErrorCallback(); // Check for errors
	this->_result = this->_system->createChannelGroup("SFXs", &this->SFXsChannelGroup);
	if (this->_result != FMOD_OK) this->_ErrorCallback(); // Check for errors

	this->_result = this->masterChannelGroup->addGroup(this->BGMsChannelGroup);
	this->_result = this->masterChannelGroup->addGroup(this->SFXsChannelGroup);
}

bool cAudioSystem::_CreateSound(std::string soundName, std::string soundType, bool is3D)
{
	// Create the whole path name, appending the path to the filename
	std::string wholePathname = this->_basePath;
	if (!this->bUseCompressed)
	{
		wholePathname.append("uncompressed/");
	}
	else
	{
		wholePathname.append("compressed/");
	}

	wholePathname.append(soundName);

	// Create sound
	FMOD::Sound* newSound;
	if (!is3D)
	{
		// simple non 3D sound
		this->_result = this->_system->createSound(wholePathname.c_str(), FMOD_DEFAULT, 0, &newSound);
		if (this->_result != FMOD_OK)
		{
			this->_ErrorCallback(); // Check for errors
			return false;
		}
	}
	else
	{
		// Create a 3D sound
		this->_result = _system->createSound(wholePathname.c_str(), FMOD_3D, 0, &newSound);
		if (this->_result != FMOD_OK)
		{
			this->_ErrorCallback(); // Check for errors
			return false;
		}
		//newsSound->schannelPosition = { 0.0f, 0.0f, 0.0f };
		//// Set 3D min and max distances. Max rarely needs to be changed, default is 10000
		//_result = newSound->set3DMinMaxDistance(distance.x, distance.y);
		//if (_result != FMOD_OK) ErrorCallback(); // Check for errors
		//newsSound->sMin3DDistance = distance.x;
		//newsSound->sMax3DDistance = distance.y;
	}
	//if (_result != FMOD_OK) ErrorCallback(); // Check for errors
	//newsSound->ssound = newSound;

	// Create channel for the new sound
	/*FMOD::Channel* newChannel;
	if (soundType == "BGM")
	{
		_result = _system->playSound(newSound, channelGroups[1]->channelGroup, true, &newChannel);
	}
	else if (soundType == "BGM_1")
	{
		_result = _system->playSound(newSound, channelGroups[3]->channelGroup, true, &newChannel);
	}
	else if (soundType == "BGM_2")
	{
		_result = _system->playSound(newSound, channelGroups[4]->channelGroup, true, &newChannel);
	}
	else
	{
		_result = _system->playSound(newSound, channelGroups[2]->channelGroup, true, &newChannel);
	}*/
	//if (_result != FMOD_OK) ErrorCallback(); // Check for errors
	//if (distance.x > 0.0f)
	//{
	//	_result = newChannel->set3DAttributes(&newsSound->schannelPosition, &newsSound->schannelVelocity);
	//	if (_result != FMOD_OK) ErrorCallback(); // Check for errors
	//}
	//newsSound->schannel = newChannel;
	
	// Set params
	//newsSound->sfilename = filename;
	//newsSound->sfriendlyName = friendlyName;
	//newsSound->sfileType = soundType;
	//newsSound->sVolume = 0.5f;
	//_result = newChannel->setVolume(0.5f);
	//if (_result != FMOD_OK) ErrorCallback(); // Check for errors
	////_result = newSound->getMusicSpeed(&newsSound->sSPeed);          // NOT SUPPORTED BY WAV OR OGG
	////if (_result != FMOD_OK) ErrorCallback(); // Check for errors
	//_result = newChannel->getPitch(&newsSound->sPitch);
	//if (_result != FMOD_OK) ErrorCallback(); // Check for errors
	//_result = newChannel->getFrequency(&newsSound->sFrequency);
	//if (_result != FMOD_OK) ErrorCallback(); // Check for errors
	//_result = newChannel->getPosition(&newsSound->sPosition, FMOD_TIMEUNIT_MS);
	//if (_result != FMOD_OK) ErrorCallback(); // Check for errors

	// Save new Sound in the appropriate array
	if (soundType == "BGM")
	{
		BGMs.push_back(newSound);
	}
	else if (soundType == "SFX")
	{
		SFXs.push_back(newSound);
	}

	return true;
}

bool cAudioSystem::LoadSounds()
{
	if (!this->bUseCompressed)
	{
		this->_CreateSound("BGM_FFX-SightOfSpira.wav", "BGM", false);
		this->_CreateSound("SFX_Footsteps_1.wav", "SFX", false);
		this->_CreateSound("SFX_Footsteps_2.wav", "SFX", false);
		this->_CreateSound("SFX_Footsteps_3.wav", "SFX", false);
		this->_CreateSound("SFX_Footsteps_4.wav", "SFX", false);
		this->_CreateSound("SFX_Footsteps_5.wav", "SFX", false);
		this->_CreateSound("SFX_Footsteps_6.wav", "SFX", false);
		return true;
	}
	else
	{
		this->_CreateSound("BGM_FFX-SightOfSpira.ogg", "BGM", false);
		return true;
	}

	return false;
}

void cAudioSystem::UpdateSystem(double deltaTime)
{
	cPoolManager* tempPool = cPoolManager::GetInstance();
	for (unsigned int i = 0; i != tempPool->vecStaticEntities.size(); i++)
	{
		sEntity* entity = tempPool->vecStaticEntities[i];
		sAudioComponent* audioC = entity->GetComponent<sAudioComponent>();
		if (audioC != nullptr)
		{
			if (audioC->bPlayOnStart && !audioC->bIsPlaying || audioC->GetShouldPlaySound() && !audioC->bIsPlaying)
			{
				this->PlaySound(audioC);
				if (!audioC->bLoop)
				{
					audioC->SetSoundShouldPlay(false);
					audioC->bIsPlaying = false;
				}
			}
		}
	}
}

void cAudioSystem::PlaySound(iComponent* audioComponent)
{
	FMOD::Sound* sound = nullptr;
	FMOD::Channel* newChannel;
	FMOD::ChannelGroup* channelGroup = nullptr;
	sAudioComponent* audioC = static_cast<sAudioComponent*>(audioComponent);

	char soundName[500];

	for (unsigned int i = 0; i != BGMs.size(); i++)
	{
		this->_result = BGMs[i]->getName(soundName, sizeof(soundName));
		if (_result != FMOD_OK) this->_ErrorCallback();
		if (BGMs[i] == audioC->sound || soundName == audioC->filename)
		{
			channelGroup = this->BGMsChannelGroup;
			sound = BGMs[i];
		}
	}

	if (channelGroup == nullptr)
	{
		for (unsigned int i = 0; i != SFXs.size(); i++)
		{
			this->_result = SFXs[i]->getName(soundName, sizeof(soundName));
			if (_result != FMOD_OK) this->_ErrorCallback();
			if (SFXs[i] == audioC->sound || soundName == audioC->filename)
			{
				channelGroup = this->SFXsChannelGroup;
				sound = SFXs[i];
			}
		}
	}

	bool isPlaying = false;
	this->_result = this->_system->playSound(sound, channelGroup, false, &newChannel);
	if (this->_result != FMOD_OK) this->_ErrorCallback();
	this->_result = newChannel->setVolume(audioC->volume);
	//if (this->_result != FMOD_OK) this->_ErrorCallback();
	//this->_result = sound->setMusicSpeed(audioC->speed);
	if (this->_result != FMOD_OK) this->_ErrorCallback();
	this->_result = newChannel->setPitch(audioC->pitch);
	if (this->_result != FMOD_OK) this->_ErrorCallback();
	this->_result = newChannel->setPan(audioC->pan);
	if (this->_result != FMOD_OK) this->_ErrorCallback();
	if (audioC->frequency != 0.0f)
	{
		this->_result = newChannel->setFrequency(audioC->frequency);
		if (this->_result != FMOD_OK) this->_ErrorCallback();
	}
	newChannel->setPaused(false);
	audioC->bIsPlaying = true;
}

bool cAudioSystem::ScheduleEntity(sEntity* entity, iComponent* component)
{
	return false;
}

bool cAudioSystem::UnscheduleEntity(sEntity* entity, iComponent* component)
{
	return false;
}


void cAudioSystem::_ErrorCallback()
{
	printf("FMOD ERROR: %d : %s!\n", this->_result, FMOD_ErrorString(_result));
}

bool cAudioSystem::_ShutdownFMOD()
{
	// Relese the sounds from the BGMs and SFXs properly
	for (int i = 0; i != this->BGMs.size(); i++)
	{
		this->_result = this->BGMs[i]->release();
		if (this->_result != FMOD_OK)
		{
			this->_ErrorCallback();
			return false;
		}
	}
	for (int i = 0; i != this->SFXs.size(); i++)
	{
		this->_result = this->SFXs[i]->release();
		if (this->_result != FMOD_OK)
		{
			this->_ErrorCallback();
			return false;
		}
	}

	if (this->_system)
	{
		// Close FMOD system
		this->_result = this->_system->close();
		if (_result != FMOD_OK)
		{
			this->_ErrorCallback();
			return false;
		}

		// Release FMOD system memory
		this->_result = _system->release();
		if (_result != FMOD_OK)
		{
			this->_ErrorCallback();
			return false;
		}
	}

	return true;
}
