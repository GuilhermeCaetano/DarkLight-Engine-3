#ifndef _cAnimationSystem_HG_
#define _cAnimationSystem_HG_

#include "iSystem.h"
#include <Entity\sEntity.h>
#include <Entity\Components\iComponent.h>
#include <Animation\Assimp\scene.h>
#include <Animation\Assimp\Importer.hpp>
#include <vector>
#include <OpenGL\OpenGLHeaders.h>

class cAnimationSystem : public iSystem
{
public:
	cAnimationSystem();
	~cAnimationSystem();

	// 
	void BoneTransform(float timeInSeconds, std::string animationName, std::vector<glm::mat4>& finalTransformation,
					   std::vector<glm::mat4>& globals, std::vector<glm::mat4>& offsets);

	void CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, aiQuaternion& out);
	void CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out);
	void CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out);

	void CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::quat& out);
	void CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out);
	void CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out);

	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	const aiNodeAnim* FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString nodeOrBoneName);

	void ReadNodeHeirarchy(float AnimationTime,
		std::string animationName,		// Now with more "choose animation"
		const aiNode* pNode,
		const glm::mat4 &parentTransformMatrix);

	// Updates the system
	virtual void UpdateSystem(double deltaTime);

	// Schedules and unschedule specific components
	// in order to save searching memory
	virtual bool ScheduleEntity(sEntity* entity, iComponent* component);
	virtual bool UnscheduleEntity(sEntity* entity, iComponent* component);

	const aiScene* pScene;
	Assimp::Importer mImporter;
};



#endif // !_cAnimationSystem_HG_
