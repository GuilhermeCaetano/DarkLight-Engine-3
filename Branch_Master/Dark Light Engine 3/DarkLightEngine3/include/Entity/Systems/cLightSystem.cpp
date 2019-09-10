// cLightSystem.cpp

#include <Entity\Systems\cLightSystem.h>
#include <Engine\cECSPoolContainer.h>
#include <Entity\Components\sLightComponent.h>
#include <IO\cIOManager.h>

cLightSystem::cLightSystem()
{
	this->_currentLightComponent = NULL;
	this->lightSelectionControl = 0;
	this->_bIsLightUnilocSet = false;

	return;
}

cLightSystem::~cLightSystem()
{
	return;
}

void cLightSystem::LoadUniformLocations(GLuint shaderProgramID)
{
	this->_numberOfLightsOnX_UniLoc = glGetUniformLocation(shaderProgramID, "numberOfLightsOnX");
	std::vector<void*> vecLightContainers = cECSPoolContainer::GetInstance()->GetPoolContainerMultiple(sLightComponent::GetStaticType());
	//cPoolManager* tempPool = cPoolManager::GetInstance();
	for (unsigned int index = 0; index != vecLightContainers.size(); index++)
	{
		sMemoryChunk::sContainer* lightContainer = static_cast<sMemoryChunk::sContainer*>(vecLightContainers[index]);
		for (unsigned int lightIndex = 0; lightIndex < lightContainer->numInitializedElements; lightIndex++)
		{
			sLightComponent* lightComponent = nMemory::GetArithmeticArrayElement<sLightComponent>(lightIndex, lightContainer->dynamicArray, lightContainer->objSize);
			std::string begin = "theLights[";
			begin += std::to_string(index);
			lightComponent->position_UniLoc = glGetUniformLocation(shaderProgramID, (begin + "].position").c_str());
			lightComponent->diffuse_UniLoc = glGetUniformLocation(shaderProgramID, (begin + "].diffuse").c_str());
			lightComponent->atten_UniLoc = glGetUniformLocation(shaderProgramID, (begin + "].atten").c_str());
			lightComponent->direction_UniLoc = glGetUniformLocation(shaderProgramID, (begin + "].direction").c_str());
			lightComponent->param1_UniLoc = glGetUniformLocation(shaderProgramID, (begin + "].param1").c_str());
			lightComponent->param2_UniLoc = glGetUniformLocation(shaderProgramID, (begin + "].param2").c_str());
		}
	}
}

void cLightSystem::CopyLightValuesToShader(GLuint shaderProgramID)
{
	if (!this->_bIsLightUnilocSet)
	{
		this->_bIsLightUnilocSet = true;
		this->LoadUniformLocations(shaderProgramID);
	}

	//cPoolManager* tempPool = cPoolManager::GetInstance();
	int numLights = 0;
	std::vector<void*> vecLightContainers = cECSPoolContainer::GetInstance()->GetPoolContainerMultiple(sLightComponent::GetStaticType());
	for (unsigned int i = 0; i < vecLightContainers.size(); i++)
	{
		sMemoryChunk::sContainer* lightContainer = static_cast<sMemoryChunk::sContainer*>(vecLightContainers[i]);
		numLights += lightContainer->numInitializedElements;
	}

	glUniform3i(this->_numberOfLightsOnX_UniLoc, (GLuint)numLights, 0, 0);
	for (unsigned int index = 0; index != vecLightContainers.size(); index++)
	{
		sMemoryChunk::sContainer* lightContainer = static_cast<sMemoryChunk::sContainer*>(vecLightContainers[index]);
		for (unsigned int lightIndex = 0; lightIndex < lightContainer->numInitializedElements; lightIndex++)
		{
			sLightComponent* lightComponent = nMemory::GetArithmeticArrayElement<sLightComponent>(lightIndex, lightContainer->dynamicArray, lightContainer->objSize);
			glUniform4f(lightComponent->position_UniLoc, lightComponent->position.x, lightComponent->position.y, lightComponent->position.z, 1.0f);
			glUniform4f(lightComponent->diffuse_UniLoc, lightComponent->diffuse.x, lightComponent->diffuse.y, lightComponent->diffuse.z, 1.0f);
			glUniform4f(lightComponent->atten_UniLoc, lightComponent->attenuation.x, lightComponent->attenuation.y, lightComponent->attenuation.z, lightComponent->attenuation.w);
			glUniform4f(lightComponent->direction_UniLoc, lightComponent->direction.x, lightComponent->direction.y, lightComponent->direction.z, lightComponent->direction.w);
			glUniform4f(lightComponent->param1_UniLoc, lightComponent->param1.x, lightComponent->param1.y, lightComponent->param1.z, 1.0f);
			glUniform4f(lightComponent->param2_UniLoc, lightComponent->param2.x, 0.0f, 0.0f, 0.0f);
		}
	}
}

void cLightSystem::TurnOnLight(int index, bool turnedOn)
{
	/*cPoolManager* tempPool = cPoolManager::GetInstance();
	sLightComponent* lightComponent = static_cast<sLightComponent*>(tempPool->vecLightComponents[index]);
	lightComponent->param2.x = float(turnedOn);*/
}

void cLightSystem::ProcessKeys(GLFWwindow* window)
{
	if (cIOManager::GetEditorState() == cIOManager::Light)
	{
		/*cPoolManager* tempPool = cPoolManager::GetInstance();
		sLightComponent* lightComponent = static_cast<sLightComponent*>(tempPool->vecLightComponents[this->lightSelectionControl]);
		this->_currentLightComponent = lightComponent;
		
		this->_ControlLightSelection(window);
		this->_ControlLightControlType(window);
		this->_ControlLightMovement(window);
		this->_ControlLightColour(window);
		this->_ControlLightAttenutation(window);
		this->_ControlLightSpotAngles(window);
		this->_ControlLightParams(window);*/
	}
}

void cLightSystem::UpdateSystem(double deltaTime)
{
}

bool cLightSystem::ScheduleEntity(sEntity* entity, iComponent* component)
{
	return false;
}

bool cLightSystem::UnscheduleEntity(sEntity* entity, iComponent* component)
{
	return false;
}

void cLightSystem::_Cleanup()
{
}

void cLightSystem::_ControlLightSelection(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_TAB) && !glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
	{
		/*cPoolManager* tempPool = cPoolManager::GetInstance();
		this->lightSelectionControl++;
		if (this->lightSelectionControl >= tempPool->vecLightComponents.size())
		{
			this->lightSelectionControl = 0;
		}*/

		printf("EditorState: LightSelec %d\n", this->lightSelectionControl);
	}
}

void cLightSystem::_ControlLightControlType(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_I))
	{
		printf("EditorState: LightType(Directional)\n");
		this->_currentLightComponent->SetLightType(sLightComponent::DIRECTIONAL_LIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_O))
	{
		printf("EditorState: LightType(Point)\n");
		this->_currentLightComponent->SetLightType(sLightComponent::POINT_LIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_P))
	{
		printf("EditorState: LightType(Spot)\n");
		this->_currentLightComponent->SetLightType(sLightComponent::SPOT_LIGHT);
	}

	if (glfwGetKey(window, GLFW_KEY_1))
	{
		printf("EditorState: LightMovement\n");
		this->_controlTypeIndex = 0;
	}
	else if (glfwGetKey(window, GLFW_KEY_2))
	{
		printf("EditorState: LightColour(r)\n");
		this->_controlTypeIndex = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_3))
	{
		printf("EditorState: LightColour(g)\n");
		this->_controlTypeIndex = 2;
	}
	else if (glfwGetKey(window, GLFW_KEY_4))
	{
		printf("EditorState: LightColour(b)\n");
		this->_controlTypeIndex = 3;
	}
	else if (glfwGetKey(window, GLFW_KEY_5))
	{
		printf("EditorState: LightAttenuation(const)\n");
		this->_controlTypeIndex = 4;
	}
	else if (glfwGetKey(window, GLFW_KEY_6))
	{
		printf("EditorState: LightAttenuation(lin)\n");
		this->_controlTypeIndex = 5;
	}
	else if (glfwGetKey(window, GLFW_KEY_7))
	{
		printf("EditorState: LightAttenuation(quad)\n");
		this->_controlTypeIndex = 6;
	}
	else if (glfwGetKey(window, GLFW_KEY_8))
	{
		printf("EditorState: LightSpotAngles\n");
		this->_controlTypeIndex = 7;
	}
	else if (glfwGetKey(window, GLFW_KEY_9))
	{
		printf("EditorState: Params\n");
		this->_controlTypeIndex = 8;
	}
}

void cLightSystem::_ControlLightMovement(GLFWwindow* window)
{
	if (this->_controlTypeIndex == 0)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { this->_currentLightComponent->position.z += 0.5f; }
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { this->_currentLightComponent->position.z -= 0.5f; }
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { this->_currentLightComponent->position.x -= 0.5f; }
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { this->_currentLightComponent->position.x += 0.5f; }
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { this->_currentLightComponent->position.y += 0.5f; }
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { this->_currentLightComponent->position.y -= 0.5f; }
	}
}

void cLightSystem::_ControlLightColour(GLFWwindow* window)
{
	switch (this->_controlTypeIndex)
	{
		case 1:
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { this->_currentLightComponent->diffuse.r = ((this->_currentLightComponent->diffuse.r + 0.01f) >= 1.0f ? 1.0f : this->_currentLightComponent->diffuse.r + 0.01f); }
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { this->_currentLightComponent->diffuse.r = ((this->_currentLightComponent->diffuse.r - 0.01f) <= 0.0f ? 0.0f : this->_currentLightComponent->diffuse.r - 0.01f); }
		break;
		case 2:
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { this->_currentLightComponent->diffuse.g = ((this->_currentLightComponent->diffuse.g + 0.01f) >= 1.0f ? 1.0f : this->_currentLightComponent->diffuse.g + 0.01f); }
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { this->_currentLightComponent->diffuse.g = ((this->_currentLightComponent->diffuse.g - 0.01f) <= 0.0f ? 0.0f : this->_currentLightComponent->diffuse.g - 0.01f); }
		break;
		case 3:
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { this->_currentLightComponent->diffuse.b = ((this->_currentLightComponent->diffuse.b + 0.01f) >= 1.0f ? 1.0f : this->_currentLightComponent->diffuse.b + 0.01f); }
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { this->_currentLightComponent->diffuse.b = ((this->_currentLightComponent->diffuse.b - 0.01f) <= 0.0f ? 0.0f : this->_currentLightComponent->diffuse.b - 0.01f); }
		break;
	}
}

void cLightSystem::_ControlLightAttenutation(GLFWwindow* window)
{
	switch (this->_controlTypeIndex)
	{
		case 4:
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { this->_currentLightComponent->attenuation.x = ((this->_currentLightComponent->attenuation.x + 0.01f) >= 10.0f ? 10.0f : this->_currentLightComponent->attenuation.x + 0.01f); }
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { this->_currentLightComponent->attenuation.x = ((this->_currentLightComponent->attenuation.x - 0.01f) <= 0.0f ? 0.0f : this->_currentLightComponent->attenuation.x - 0.01f); }
		break;																				  
		case 5:																				  
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { this->_currentLightComponent->attenuation.y = ((this->_currentLightComponent->attenuation.y + 0.001f) >= 10.0f ? 10.0f : this->_currentLightComponent->attenuation.y + 0.001f); }
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { this->_currentLightComponent->attenuation.y = ((this->_currentLightComponent->attenuation.y - 0.001f) <= 0.0f ? 0.0f : this->_currentLightComponent->attenuation.y - 0.001f); }
		break;																				  
		case 6:																				  
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { this->_currentLightComponent->attenuation.z = ((this->_currentLightComponent->attenuation.z + 0.00001f) >= 10.0f ? 10.0f : this->_currentLightComponent->attenuation.z + 0.00001f); }
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { this->_currentLightComponent->attenuation.z = ((this->_currentLightComponent->attenuation.z - 0.00001f) <= 0.0f ? 0.0f : this->_currentLightComponent->attenuation.z - 0.00001f); }
		break;
	}
}

void cLightSystem::_ControlLightSpotAngles(GLFWwindow* window)
{
	if (this->_controlTypeIndex == 7)
	{
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { this->_currentLightComponent->param1.y = ((this->_currentLightComponent->param1.y + 0.1f) >= 45.0f ? 45.0f : this->_currentLightComponent->param1.y + 0.1f); }
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { this->_currentLightComponent->param1.y = ((this->_currentLightComponent->param1.y - 0.1f) <= 0.0f ? 0.0f : this->_currentLightComponent->param1.y - 0.1f); }
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { this->_currentLightComponent->param1.z = ((this->_currentLightComponent->param1.z + 0.1f) >= 45.0f ? 45.0f : this->_currentLightComponent->param1.z + 0.1f); }
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { this->_currentLightComponent->param1.z = ((this->_currentLightComponent->param1.z - 0.1f) <= 0.0f ? 0.0f : this->_currentLightComponent->param1.z - 0.1f); }
	}
}

void cLightSystem::_ControlLightParams(GLFWwindow* window)
{
	if (this->_controlTypeIndex == 8)
	{
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { this->_currentLightComponent->param2.x = 1.0f; }
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { this->_currentLightComponent->param2.x = 0.0f; }
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { this->_currentLightComponent->bisWireframeOn = true; }
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { this->_currentLightComponent->bisWireframeOn = false; }
	}
}
