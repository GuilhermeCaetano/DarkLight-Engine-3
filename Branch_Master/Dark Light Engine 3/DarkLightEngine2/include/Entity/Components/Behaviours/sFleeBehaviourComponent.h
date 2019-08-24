#ifndef _sFleeBehaviourComponent_HG_
#define _sFleeBehaviourComponent_HG_

#include <Entity\Components\Behaviours\iBehaviour.h>
#include <OpenGL\OpenGLHeaders.h>

struct sEntity;


struct sFleeBehaviourComponent : public iBehaviour
{
public:
	sFleeBehaviourComponent(unsigned long long entityID);
	~sFleeBehaviourComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "FLEEBEHAV_COMPONENT"; };

	eBehaviourTypes behaviourType;

	sEntity* agent;
	iComponent* target;

	glm::vec3 agentDirection;
	glm::vec3 targetDirection;
	glm::vec3 moveDirection;

	float distAgentTarget;
	float angleAgentTarget;

protected:
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

};


#endif // !_sFleeBehaviourComponent_HG_

