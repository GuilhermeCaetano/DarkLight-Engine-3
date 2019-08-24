#ifndef _iBehaviour_HG_
#define _iBehaviour_HG_

#include <Entity\Components\iComponent.h>


enum eBehaviourTypes
{
	SIMPLE,
	BOO,
	CLOSEST
};

struct iBehaviour : public iComponent
{
public:
	virtual ~iBehaviour() {};

	std::string BehaviourEnumToString(eBehaviourTypes type)
	{
		if (type == SIMPLE) { return "SIMPLE"; }
		else if (type == BOO) { return "BOO"; }
		else if (type == CLOSEST) { return "CLOSEST"; }
	}

	eBehaviourTypes BehaviourStringToBehaviourEnum(std::string type)
	{
		if (type == "SIMPLE") { return SIMPLE; }
		else if (type == "BOO") { return BOO; }
		else if (type == "CLOSEST") { return CLOSEST; }
	}

};



#endif // !_iBehaviour_HG_

