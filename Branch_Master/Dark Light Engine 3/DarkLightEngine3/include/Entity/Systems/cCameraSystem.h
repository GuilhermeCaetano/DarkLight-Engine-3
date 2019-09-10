#ifndef _cCameraSystem_HG_
#define _cCameraSystem_HG_

#include "iSystem.h"
#include <OpenGL\OpenGLHeaders.h>
#include <Entity\sEntity.h>
#include <Entity\Components\iComponent.h>
#include <vector>


class cCameraSystem : public iSystem
{
public:
	cCameraSystem();
	~cCameraSystem();

	void SetupSystem(GLuint shaderProgramID);

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

	void SetCameraFollowTarget(sEntity* entity);

	// Updates the system
	virtual void UpdateSystem(double deltaTime);

	// Schedules and unschedule specific components
	// in order to save searching memory
	virtual bool ScheduleEntity(sEntity* entity, iComponent* component);
	virtual bool UnscheduleEntity(sEntity* entity, iComponent* component);

	void ProcessKeys(iComponent* cameraC);

protected:
	virtual void _Cleanup();

private:
	//struct sCameraComponent* _currentCamera;

	GLuint _cameraLocation;
	GLuint _matViewLocation;

	unsigned int _followIndex;
	std::vector<sEntity*> _vecFollowTargets;

	void _MoveForward_Z(iComponent* cameraC, float amount);
	void _MoveLeftRight_X(iComponent* cameraC, float amount);
	void _MoveUpDown_Y(iComponent* cameraC, float amount);

	void _Pitch_UpDown(iComponent* cameraC, float angleDegrees);	// around X
	void _Yaw_LeftRight(iComponent* cameraC, float angleDegrees);	// around Y
	void _Roll_CW_CCW(iComponent* cameraC, float angleDegrees);	// around Z

	void _SimpleCameraFollow(iComponent* cameraC, double deltaTime);
	void _SmoothCameraFollow(iComponent* cameraC, double deltaTime);
	void _ZeldaCameraFollow(iComponent* iCameraC, double deltaTime);

	void _ProcessKeyboard(iComponent* cameraC);
	void _ProcessMouse(iComponent* cameraC);

	void _FollowNextTarget();
};



#endif // !_cCameraSystem_HG_
