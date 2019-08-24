#ifndef _sLaserGunComponent_HG_
#define _sLaserGunComponent_HG_

#include <Entity\Components\iComponent.h>


struct sLaserGunComponent : public iComponent
{
	sLaserGunComponent(unsigned long long entityID);
	~sLaserGunComponent();
};




#endif // !_sLaserGunComponent_HG_

