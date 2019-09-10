#ifndef _cLevel_HG_
#define _cLevel_HG_

#include "iLevel.h"


class cLevel : public iLevel
{
public:
	cLevel(unsigned int levelID, const std::string& levelName);
	~cLevel();

	virtual void Awake();
	virtual void Start();

	virtual void UpdateTick();

	virtual void End();

};


#endif // !_cLevel_HG_

