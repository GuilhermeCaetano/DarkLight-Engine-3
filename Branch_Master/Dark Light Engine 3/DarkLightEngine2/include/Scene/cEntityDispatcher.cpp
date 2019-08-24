// cEntityDispatcher.cpp

#include <Scene\cEntityDispatcher.h>
#include <Pools\cPoolManager.h>
#include <Entity\Components\sLocationComponent.h>

cEntityDispatcher::cEntityDispatcher()
{
	this->bLoop = false;
	this->bPressed = false;
	this->currentWave = 0;
	this->timeBetweenWaves = 5.0f;
	this->currentWaveTime = 0.0f;

	return;
}

cEntityDispatcher::~cEntityDispatcher()
{
	return;
}

void cEntityDispatcher::SetWave(std::vector<std::pair<std::string/*Entity Name*/, glm::vec3/*Entity Position*/>> newWave)
{
	this->vecWaves.push_back(newWave);
}

void cEntityDispatcher::GetWave()
{
	cPoolManager* pPool = cPoolManager::GetInstance();
	for (unsigned int wavePairIndex = 0; wavePairIndex != this->vecWaves[this->currentWave].size(); wavePairIndex++)
	{
		std::pair<std::string, glm::vec3> curPair = this->vecWaves[currentWave][wavePairIndex];
		// Try to find an Entity with the name on the pool that is not active
		sEntity* waveEntity = pPool->GetEntityByName(curPair.first, false);
		if (waveEntity != nullptr)
		{
			sLocationComponent* locationC = waveEntity->GetComponent<sLocationComponent>();
			locationC->SetWorldPosition(curPair.second);
			waveEntity->SetIsActive(true);
		}
	}
}

void cEntityDispatcher::UpdateEntityDispatcher(GLFWwindow* window, double deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_N) && !this->bPressed)
	{
		this->bPressed = true;
		this->currentWave++;
		if (this->currentWave >= this->vecWaves.size())
		{
			this->currentWave = 0;
		}

		this->GetWave();
	}
	else if(!glfwGetKey(window, GLFW_KEY_N))
	{
		this->bPressed = false;
	}

	/*if (this->currentWaveTime >= this->timeBetweenWaves)
	{
		this->GetWave();
		this->currentWaveTime = 0.0f;
		this->currentWave++;
		if (this->currentWave >= this->vecWaves.size())
		{
			this->currentWave = 0;
		}
	}
	else
	{
		this->currentWaveTime += (float)deltaTime;
	}*/
}
