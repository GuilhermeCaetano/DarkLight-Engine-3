#ifndef _cAnimator_HG_
#define _cAnimator_HG_

#include <string>
#include <vector>
#include <map>

#include <Mesh\Assimp\cAssimpMeshLoader.h>
#include <Mesh\sModelDrawInfo.h>

#include <Windows.h>
#include <process.h>


class cAnimator
{
public:
	cAnimator(sModelDrawInfo* meshInfo);
	~cAnimator();

	struct sAnimationState
	{
		sAnimationState(const std::string& newName, float newTotalTime, float newStepTime, sAnimationInfo info)
		{
			name = newName;
			currentTime = 0.0f;
			totalTime = newTotalTime;
			frameStepTime = newStepTime;
			animInfo = info;

			bHold = false;
			bWaitForLoop = false;
		}

		std::string name;
		float currentTime;		// Time (frame) in current animation
		float totalTime;		// Total time animation goes
		float frameStepTime;	// Number of seconds to 'move' the animation	
		sAnimationInfo animInfo;

		// Flags
		bool bLoop;
		bool bHold;
		bool bWaitForLoop;
		std::string nextAnimation;

		// Returns true if time had to be reset
		// (for checking to see if the animation has finished or not)
		// TODO: Deal with running the animation backwards, perhaps?? 
		bool IncrementTime(bool bResetToZero = true);
	};

	
	bool bIsActive;
	
	sAnimationState* FindAnimation(const std::string& animationName);

	void AddAnimation(const std::string& animName);
	void SetCurrentAnimation(const std::string& animationName, bool waitForLoop = false);
	void CallAnimation(const std::string& animationName);
	void QueueAnimation(const std::string& animationName);

	void HoldAnimation(bool isHolding);

	void CheckAnimationState();
	void UpdateSystem();

	//HACK: will be in a system later
	void AnimationBoneTransform(float TimeInSeconds, std::string animationName); /*, std::vector<glm::mat4> &FinalTransformation,
									std::vector<glm::mat4> &Globals, std::vector<glm::mat4> &Offsets);*/
	void ReadNodeHeirarchy(float AnimationTime, std::string& animationName, const aiNode* pNode, const glm::mat4 &parentTransformMatrix);
	const aiNodeAnim* FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString nodeOrBoneName);
	void CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::quat& out);
	void CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out);
	void CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	std::map<std::string/*Animation Name*/, sAnimationState*/*Animation*/> mapAnimations;
	std::vector<sAnimationState*> vecAnimationQueue;
	sAnimationState* currentAnimation;
	unsigned int currentIndex;

	sModelDrawInfo* currentMesh;
};

#endif 

