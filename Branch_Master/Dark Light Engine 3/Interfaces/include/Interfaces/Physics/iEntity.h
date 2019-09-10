#ifndef _iEntity_HG_
#define _iEntity_HG_

#include <Engine\sObject.h>
#include <Entity\Components\sLocationComponent.h>


struct iEntity : public sObject
{
	virtual ~iEntity() {};

	virtual bool GetIsActive() = 0;
	virtual void Start() = 0;

	virtual sLocationComponent* GetTransform() = 0;
};


#endif // !_iEntity_HG_

