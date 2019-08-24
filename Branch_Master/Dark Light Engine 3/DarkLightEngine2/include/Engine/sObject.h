#ifndef _sObject_HG_
#define _sObject_HG_

#include <string>
#include <rapidjson\document.h>
#include <rapidjson\filewritestream.h>
#include <rapidjson\prettywriter.h>


/* sObject
 - The base abstract class for all ECS objects
 - Low level engine stats and works
 - Should not be used in game code
*/

struct sObject
{
public:
	virtual ~sObject()
	{
		this->_BaseDestroy();
	}

	// Flags for the Engine
	inline bool GetIsInitialized() { return this->_bIsInitialized; };
	inline unsigned long long GetParentID() { return this->_parentID; };
	
	// This must be overriden on the derived components, otherwise it will return an error!
	inline static std::string GetStaticType() { return "ERROR"; };

	virtual void Destroy() { delete this; };
	virtual void Destroy(float waitTime) {};

	void operator delete(void* object) {};

	// Write and read component information from a json file
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
	{
		prettyWriter.Key("ECSType"); prettyWriter.String(this->GetStaticType().c_str());
		prettyWriter.Key("ParentID"); prettyWriter.Int64(this->GetParentID());

		this->_SerializeToJson(prettyWriter);
	}

	void DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
	{
		for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
		{
			if (innerIter->name == "ParentID") { this->_parentID = innerIter->value.GetInt64(); }
		}

		this->_DeserializeFromJson(vIter);
		this->_BaseInit();
	}

	bool _bIsInitialized = false;

protected:
	sObject()
	{
		this->_BaseInit();
	}

	// New and Delete operators are being overloaded so
	// that memory allocation is the engine's responsibility.
	void* operator new(size_t size, void* memory) {};
	

	// Engine setup and destroy needed for all objects
	void _BaseInit() 
	{
		this->_bIsInitialized = true;
		this->_bIsActive = true;
	}

	void _BaseDestroy()
	{
		this->_bIsInitialized = false;
		this->_bIsActive = false;
	}

	// Sets the ID of whoever owns the object. This can be 0, which means no one.
	void _SetParentID(unsigned long long ID) { this->_parentID = ID; };

	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter) = 0;
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter) = 0;

	// Common data for all objects
	unsigned long long _parentID = 0;
	
	bool _bIsActive = false;

};


#endif // !_sObject_HG_

