#ifndef _sIdleBehaviourComponent_HG_
#define _sIdleBehaviourComponent_HG_

#include <Entity\Components\Behaviours\iBehaviour.h>
#include <OpenGL\OpenGLHeaders.h>

struct sEntity;


struct sIdleBehaviourComponent : public iBehaviour
{
public:
	sIdleBehaviourComponent(unsigned long long entityID);
	~sIdleBehaviourComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "IDLEBEHAV_COMPONENT"; };

	sEntity* agent;

	bool idle;
	float idleTime;

protected:
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

};


#endif // !_sIdleBehaviourComponent_HG_

