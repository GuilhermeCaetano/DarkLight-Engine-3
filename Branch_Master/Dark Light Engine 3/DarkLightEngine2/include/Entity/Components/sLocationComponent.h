#ifndef _sLocationComponent_HG_
#define _sLocationComponent_HG_

#include <Entity\Components\iComponent.h>
#include <OpenGL\OpenGLHeaders.h>
#include <string>


struct sLocationComponent : public iComponent
{
public:
	sLocationComponent(unsigned long long entityID);
	~sLocationComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "LOCATION_COMPONENT"; };

	// Positional elements
	virtual glm::vec3 GetWorldPosition();
	virtual void SetWorldPosition(const glm::vec3& position);
	virtual glm::quat GetWorldRotation();
	virtual void SetWorldRotation(const glm::quat& rotation);

	// Returns directional elements
	virtual glm::vec3 GetForwardVector();
	virtual glm::vec3 GetUpVector();
	virtual glm::vec3 GetRightVector();

	// Returns world related elements
	virtual glm::vec3 GetLastPosition();
	virtual glm::vec3 GetRespawnPosition();

protected:
	void _CheckVectorOrientation();

	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

	glm::vec3 _position;
	glm::quat _qOrientation;

	glm::vec3 _frontOfObject;
	glm::vec3 _rightOfObject;
	glm::vec3 _upOfObject;

	glm::vec3 _forward;
	glm::vec3 _right;
	glm::vec3 _up;

	glm::vec3 _lastPosition;
	glm::vec3 _respawnPosition;
	float _safeDistancePosition;

	bool _bChangedPosition;
	bool _bChangedRotation;

};


#endif // !_sLocationComponent_HG_

