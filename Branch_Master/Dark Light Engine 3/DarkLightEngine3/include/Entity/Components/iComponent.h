#ifndef _iComponent_HG_
#define _iComponent_HG_

#include <Engine\sObject.h>


struct iComponent : public sObject
{
public:
	virtual ~iComponent() {};
	
	virtual void Start() = 0;

};


#endif // !_iComponent_HG_
