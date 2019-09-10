#ifndef _cLightSystem_HG_
#define _cLightSystem_HG_

#include "iSystem.h"
#include <Entity\sEntity.h>
#include <Entity\Components\iComponent.h>
#include <OpenGL\OpenGLHeaders.h>


class cLightSystem : public iSystem
{
public:
	cLightSystem();
	~cLightSystem();

	int lightSelectionControl;

	void LoadUniformLocations(GLuint shaderProgram);
	void CopyLightValuesToShader(GLuint shaderProgramID);

	void TurnOnLight(int index, bool turnedOn);

	void ProcessKeys(GLFWwindow* window);

	// Updates the system
	virtual void UpdateSystem(double deltaTime);

	// Schedules and unschedule specific components
	// in order to save searching memory
	virtual bool ScheduleEntity(sEntity* entity, iComponent* component);
	virtual bool UnscheduleEntity(sEntity* entity, iComponent* component);

protected:
	virtual void _Cleanup();

private:
	void _ControlLightSelection(GLFWwindow* window);
	void _ControlLightControlType(GLFWwindow* window);
	void _ControlLightMovement(GLFWwindow* window);
	void _ControlLightColour(GLFWwindow* window);
	void _ControlLightAttenutation(GLFWwindow* window);
	void _ControlLightSpotAngles(GLFWwindow* window);
	void _ControlLightParams(GLFWwindow* window);

	struct sLightComponent* _currentLightComponent;
	int _numberOfLightsOnX_UniLoc;

	int _controlTypeIndex = 0;
	bool _bIsLightUnilocSet;
	
};



#endif // !_cLightSystem_HG_

