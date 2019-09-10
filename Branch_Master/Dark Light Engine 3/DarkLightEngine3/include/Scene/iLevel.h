#ifndef _iLevel_HG_
#define _iLevel_HG_

#include <string>
#include <vector>

class sEntity;
class iComponent;


// Abstract class for level streaming
class iLevel
{
protected:
	enum eLevelStates
	{
		UNLOADED,
		UNLOADING,
		LOADING,
		LOADED
	};

public:
	~iLevel() {};

	std::string GetLevelName() { return this->_levelName; };
	unsigned int GetLevelID() { return this->_levelID; };

	virtual void Awake() = 0;
	virtual void Start() = 0;

	virtual void UpdateTick() = 0;

	virtual void End() = 0;

	std::vector<sEntity*> vecEntities;
	std::vector<iComponent*> vecLights;

	eLevelStates levelState;
	bool bIsLevelLoading;

protected:
	std::string _levelName;
	eLevelStates _nextState;
	unsigned int _levelID;

};


#endif // !_iLevel_HG_

