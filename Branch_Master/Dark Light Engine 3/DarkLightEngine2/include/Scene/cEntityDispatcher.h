#ifndef _cEntityDispatcher_HG_
#define _cEntityDispatcher_HG_

#include <Entity\sEntity.h>
#include <OpenGL\OpenGLHeaders.h>
#include <vector>


class cEntityDispatcher
{
public:
	cEntityDispatcher();
	~cEntityDispatcher();

	void GetWave();
	void SetWave(std::vector<std::pair<std::string/*Entity Name*/, glm::vec3/*Entity Position*/>> newWave);

	void UpdateEntityDispatcher(GLFWwindow* window, double deltaTime);

	bool bLoop;

	int currentWave;
	float timeBetweenWaves;
	float currentWaveTime;

	bool bPressed;

	std::vector<std::vector<std::pair<std::string/*Entity Name*/, glm::vec3/*Entity Position*/>>> vecWaves;
};



#endif // !_cEntityDispatcher_HG_

