// sEntity.cpp

#include <Entity\sEntity.h>


unsigned long long sEntity::_nextID = sEntity::_startID;

sEntity::sEntity()
{
	this->Start();

	return;
}

sEntity::~sEntity()
{
	
}

void sEntity::Start()
{
	// All entities will inevitably have at least a location component
	this->transform = this->AddComponent<sLocationComponent>();

	// Get next ID
	this->_instanceID = this->_nextID++;

	// Setup default params
	this->_name = "Default";
	this->_tag = "Default";
}

void sEntity::SetTag(const std::string& tag)
{
	this->_tag = tag;
}

std::string sEntity::GetTag()
{
	return this->_tag;
}

unsigned long long sEntity::GetInstanceID()
{
	return this->_instanceID;
}

void sEntity::AddChildObject(sEntity* childEntity)
{
	this->_vecChilds.push_back(childEntity);
}

std::vector<sEntity*>* sEntity::GetChildObjects()
{
	return &this->_vecChilds;
}

unsigned int sEntity::GetNumChilds()
{
	return this->_vecChilds.size();
}

sEntity* sEntity::GetChildAt(unsigned int index)
{
	return this->_vecChilds[index];
}

inline sLocationComponent* sEntity::GetTransform()
{
	return this->transform;
}

void sEntity::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	prettyWriter.Key("InstanceID"); prettyWriter.Int64(this->GetInstanceID());
	prettyWriter.Key("IsActive"); prettyWriter.Bool(this->_bIsActive);
	prettyWriter.Key("Name"); prettyWriter.String(this->_name.c_str());
	prettyWriter.Key("Tag"); prettyWriter.String(this->_tag.c_str());

	prettyWriter.Key("Components"); prettyWriter.StartArray();
	for (std::unordered_map<std::string, iComponent*>::iterator mapiter = this->_mapComponents.begin(); mapiter != this->_mapComponents.end(); mapiter++)
	{
		// Check component type
		prettyWriter.StartObject();
		mapiter->second->SerializeToJson(prettyWriter);
		prettyWriter.EndObject();
	}
	prettyWriter.EndArray();
}

void sEntity::_DeserializeFromJson(rapidjson::Value::ConstValueIterator entityIter)
{
	// Inside each entity we load the values inside the entity member
	for (rapidjson::Value::ConstMemberIterator mIter = entityIter->MemberBegin(); mIter != entityIter->MemberEnd(); mIter++)
	{
		if (mIter->name == "InstanceID") this->_instanceID = mIter->value.GetInt64();
		else if (mIter->name == "IsActive") this->_bIsActive = mIter->value.GetBool();
		else if (mIter->name == "Name") this->_name = mIter->value.GetString();
		else if (mIter->name == "Tag") this->_tag = mIter->value.GetString();
	}
}

