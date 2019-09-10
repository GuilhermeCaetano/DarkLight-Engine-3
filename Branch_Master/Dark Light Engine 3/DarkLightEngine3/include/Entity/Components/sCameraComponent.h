#ifndef _sCameraComponent_HG_
#define _sCameraComponent_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Entity\Components\iComponent.h>


struct sEntity;
struct sCameraComponent : public iComponent
{
public:
	sCameraComponent(unsigned long long entityID);
	~sCameraComponent();

	enum eFollowTypes
	{
		SIMPLE,
		SMOOTH,
		ZELDA
	};

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "CAMERA_COMPONENT"; };

	glm::vec3 GetLookAtInWorldSpace(); // Get look at direction in world space
	glm::vec3 GetCameraDirection(); // Get camera current forawd direction
	glm::vec3 GetUpVector(); // Get camera current up vector
	glm::quat GetOrientation();

	void SetTarget(sEntity* newTarget);
	void SetTarget(sEntity* newTarget, glm::vec3 offset, float followDistance, float maxSpeed);
	void SetLookAtOffset(const glm::vec3& offset);

	eFollowTypes GetCameraFollowType();
	void SetCameraFollowType(eFollowTypes followType);

	// Updates to orient the axis related to the camera current rotation
	void UpdateAtFromOrientation();
	void UpdateForwardFromOrientation();
	void UpdateUpFromOrientation();
	void UpdateRightFromOrientation();

	glm::vec3 eyePosition; // The actual camera position
	glm::vec3 forward;
	glm::vec3 up;       // Current up vector
	glm::vec3 right;
	glm::quat qOrientation; // Current camera quaternion rotation

	float movementSpeed;
	bool bCanControl;

	sEntity* target;
	glm::vec3 targetOffset;
	glm::vec3 lookAtOffset;
	glm::vec3 followVelocity;

	float fullSpeedDistance;
	float maxFollowSpeed;
	bool bFollowTarget;
	glm::vec3 _lookAt;			// The direction towards what the camera is looking
protected:
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

	eFollowTypes _followType;

	// Save parameters. These won't change, they serve as a base to make the alterations
	glm::vec3 _forwardDirection; // (0, 0, 1)
	glm::vec3 _upDirection;	// (0, 1, 0)
	glm::vec3 _rightDirection; // (1, 0, 0)

};


#endif // !_sCameraComponent_HG_
