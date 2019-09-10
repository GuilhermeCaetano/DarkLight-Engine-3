#ifndef _sChaseBehaviourComponent_HG_
#define _sChaseBehaviourComponent_HG_

#include <Entity\Components\Behaviours\iBehaviour.h>
#include <OpenGL\OpenGLHeaders.h>

struct sEntity;


struct sPursueBehaviourComponent : public iBehaviour
{
public:
	sPursueBehaviourComponent(unsigned long long entityID);
	~sPursueBehaviourComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "PURSUEBEHAV_COMPONENT"; };

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


#endif // !_sChaseBehaviourComponent_HG_

