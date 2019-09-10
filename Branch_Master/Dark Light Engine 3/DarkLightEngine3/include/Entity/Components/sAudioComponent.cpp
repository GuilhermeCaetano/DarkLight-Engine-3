// sAudioComponent.cpp

#include <Entity\Components\sAudioComponent.h>


sAudioComponent::sAudioComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->Start();

	return;
}

sAudioComponent::~sAudioComponent()
{
	return;
}

void sAudioComponent::Start()
{
	this->filename = "";
	this->fileType = "";

	this->volume = 1.0f;
	this->pitch = 1.0f;
	this->pan = 0.0f;
	this->frequency = 0.0f;

	this->bPlayOnStart = false;
	this->_bShouldPlay = false;
	this->bIsPlaying = false;
	this->bLoop = false;

	this->sound = nullptr;

	this->minMax3DDistance = glm::vec2(1.0, 10.0f);
}

void sAudioComponent::SetSoundToPlay(const std::string& filenameToPlay)
{
	this->filename = filenameToPlay;
	this->_bShouldPlay = true;
}

void sAudioComponent::SetSoundShouldPlay(bool soundShouldPlay)
{
	this->_bShouldPlay = soundShouldPlay;
}

bool sAudioComponent::GetShouldPlaySound()
{
	return this->_bShouldPlay;
}

void sAudioComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	prettyWriter.Key("FileName"); prettyWriter.String(this->filename.c_str());
	prettyWriter.Key("FileType"); prettyWriter.String(this->fileType.c_str());
	prettyWriter.Key("Volume"); prettyWriter.Double((double)this->volume);
	prettyWriter.Key("Pitch"); prettyWriter.Double((double)this->pitch);
	prettyWriter.Key("Pan"); prettyWriter.Double((double)this->pan);
	prettyWriter.Key("Frequency"); prettyWriter.Double((double)this->frequency);
	prettyWriter.Key("MinMax3DDistance"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->minMax3DDistance.x); prettyWriter.Double((double)this->minMax3DDistance.y);
	prettyWriter.EndArray();
	prettyWriter.Key("PlayOnStart"); prettyWriter.Bool(this->bPlayOnStart);
	prettyWriter.Key("Loop"); prettyWriter.Bool(this->bLoop);
}

void sAudioComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{
	for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
	{
		if (innerIter->name == "FileName") { this->filename = innerIter->value.GetString(); }
		else if (innerIter->name == "FileType") { this->fileType = innerIter->value.GetString(); }
		else if (innerIter->name == "Volume") { this->volume = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "Pitch") { this->pitch = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "Pan") { this->pan = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "Frequency") { this->frequency = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "MinMax3DDistance") 
		{
			this->minMax3DDistance.x = (float)innerIter->value.GetArray()[0].GetDouble();
			this->minMax3DDistance.y = (float)innerIter->value.GetArray()[1].GetDouble();
		}
		else if (innerIter->name == "PlayOnStart") { this->bPlayOnStart = innerIter->value.GetBool(); }
		else if (innerIter->name == "Loop") { this->bLoop = innerIter->value.GetBool(); }
	}
}
