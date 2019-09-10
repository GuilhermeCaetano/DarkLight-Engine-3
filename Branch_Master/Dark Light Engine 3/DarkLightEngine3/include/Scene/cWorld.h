#ifndef cWorld_HG_
#define cWorld_HG_

#include <vector>
#include <unordered_map>

class cTestScene;
class iLevel;


class cWorld
{
public:
	cWorld();
	~cWorld();

	struct sLevelContainer
	{
		sLevelContainer() {};
		~sLevelContainer() {};

		unsigned int _numLevels;
		std::unordered_map<std::string, iLevel*> _mapLevelsByName;
	};

	iLevel* GetCurrentLevel();
	sLevelContainer* GetActiveLevels();

	bool HasActiveScene();

	bool AddLevel(iLevel* level);
	bool RemoveLevel(const std::string& levelName);

	bool PrepareLevelStreaming(const std::vector<std::string>& levelsToStream);

	void Awake();
	void Start();

	void UpdateTick();

	void End();

	std::string name;

private:
	void _Cleanup();

	iLevel* _currentLevel;
	sLevelContainer* _activeLevels;
	sLevelContainer* _allLevels;
	std::vector<iLevel*> _vecStreamLevels;

};


#endif // !cWorld_HG_

