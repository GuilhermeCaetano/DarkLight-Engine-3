#ifndef cWorldManager_HG_
#define cWorldManager_HG_

#include <string>

class cWorld;


class cWorldManager
{
public:
	cWorldManager();
	~cWorldManager();

	bool CreateWorld(const std::string& worldName);
	bool DestroyWorld(const std::string& worldName);

	cWorld* GetMainWorld();

private:
	cWorld* _mainWorld;

};



#endif // !cSceneController_HG_

