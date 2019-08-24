#ifndef _cCameraSystem_HG_
#define _cCameraSystem_HG_

#include "iSystem.h"
#include <OpenGL\OpenGLHeaders.h>
#include <IO\cIOManager.h>

#include <vector>

class cCameraSystem : public iSystem
{
public:
	cCameraSystem();
	~cCameraSystem();

	void SetCameraUniformLocations(GLuint shaderProgramID);
	void UpdateCameraView(iComponent* cameraComponent);
	void SetCallbacks(GLFWwindow* window);

	glm::vec3 GetLookAtInWorldSpace(); // Get look at direction in world space
	glm::vec3 GetCameraDirection(); // Get camera current forawd direction
	glm::vec3 GetUpVector(); // Get camera current up vector
	glm::quat GetOrientation();

	// Updates to orient the axis related to the camera current rotation
	void UpdateAtFromOrientation();
	void UpdateUpFromOrientation();

	void SetCameraFollowTarget(struct sLocationComponent* location);

	// Updates the system
	virtual void UpdateSystem(double deltaTime);

	// Schedules and unschedule specific components
	// in order to save searching memory
	virtual bool ScheduleEntity(sEntity* entity, iComponent* component);
	virtual bool UnscheduleEntity(sEntity* entity, iComponent* component);

	void ProcessKeys(GLFWwindow* window, cIOManager* pIOManager);

private:
	struct sCameraComponent* _currentCamera;

	GLuint _cameraLocation;
	GLuint _matViewLocation;

	unsigned int _followIndex;
	std::vector<struct sLocationComponent*> _vecFollowTargets;

	void _MoveForward_Z(float amount);
	void _MoveLeftRight_X(float amount);
	void _MoveUpDown_Y(float amount);

	void _Pitch_UpDown(float angleDegrees);	// around X
	void _Yaw_LeftRight(float angleDegrees);	// around Y
	void _Roll_CW_CCW(float angleDegrees);	// around Z

	void _SimpleCameraFollow(iComponent* cameraC, double deltaTime);
	void _SmoothCameraFollow(iComponent* cameraC, double deltaTime);
	void _ZeldaCameraFollow(iComponent* iCameraC, double deltaTime);

	void _ProcessKeyboard(GLFWwindow* window, cIOManager* pIOManager);
	void _ProcessMouse(GLFWwindow* window, cIOManager* pIOManager);

	void _FollowNextTarget();
};



#endif // !_cCameraSystem_HG_
