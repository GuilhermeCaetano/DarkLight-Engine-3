#ifndef _sBtRaycastHitInfo_HG_
#define _sBtRaycastHitInfo_HG_

#include "BulletCollision\CollisionDispatch\btCollisionObject.h"
#include <string>


struct sBtRaycastHitInfo
{
	bool hasHit;
	const btCollisionObject* collisionObject;
	btVector3 contactPoint;
	btVector3 contactNormal;
};


#endif // !_sBtRaycastHitInfo_HG_

