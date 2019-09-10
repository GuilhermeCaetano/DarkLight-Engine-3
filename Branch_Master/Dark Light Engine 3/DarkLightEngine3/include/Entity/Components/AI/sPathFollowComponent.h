#ifndef _sPathFollowComponent_HG_
#define _sPathFollowComponent_HG_

#include <Entity\Components\iComponent.h>


struct sPathFollowComponent : public iComponent
{
public:
	sPathFollowComponent(unsigned long long entityID);
	~sPathFollowComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "PATHFOLLOW_COMPONENT"; };

	// Calculate
	void AddMap();

protected:
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

};


#endif // !_sPathFollowComponent_HG_

