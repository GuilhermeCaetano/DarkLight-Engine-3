// cAnimator.cpp

#include <Animation\cAnimator.h>


cAnimator::cAnimator(sModelDrawInfo* meshInfo)
{
	this->currentAnimation = nullptr;
	this->currentMesh = meshInfo;

	this->currentIndex = 0;

	this->bIsActive = true;

	return;
}

cAnimator::~cAnimator()
{
	// Clean up
	for (std::map<std::string, sAnimationState*>::iterator mapIter = this->mapAnimations.begin(); mapIter != this->mapAnimations.end(); mapIter++)
	{
		delete mapIter->second;
		mapIter->second = nullptr;
	}

	this->mapAnimations.clear();

	return;
}

glm::mat4 AIMatrixToGLMMatrix(const aiMatrix4x4 &mat)
{
	return glm::mat4(mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4);
}

// Returns true if time had to be reset
// (for checking to see if the animation has finished or not)
bool cAnimator::sAnimationState::IncrementTime(bool bResetToZero /*=true*/)
{
	if (this->bHold)
	{
		return false;
	}

	bool bDidWeReset = false;

	this->currentTime += 0.04f;
	if ( this->currentTime >= (this->totalTime / this->frameStepTime) )
	{
		this->currentTime = 0.0f;
		bDidWeReset = true;
		bWaitForLoop = false;
	}

	return bDidWeReset;
}

cAnimator::sAnimationState* cAnimator::FindAnimation(const std::string& animationName)
{
	std::map<std::string, sAnimationState*>::iterator mapIter = this->mapAnimations.find(animationName);
	if (mapIter != this->mapAnimations.end())
	{
		// Found it
		return mapIter->second;
	}

	return nullptr;
}

void cAnimator::AddAnimation(const std::string& animName)
{
	// Find animation info from Vao
	sAnimationInfo animInfo;
	//nEngineGlobals::FindAnimationByName(animName, animInfo);

	const aiScene* scene = animInfo.pAIScene;

	if(this->mapAnimations.size() != 0)
	{
		// Check if animation was already inserted
		sAnimationState* anim = this->FindAnimation(animInfo.friendlyName);
		if (anim != nullptr)
		{
			// Already inserted, bail
			return;
		}
	}

	// Create new animation
	sAnimationState* newAnim = new sAnimationState(animInfo.friendlyName, scene->mAnimations[0]->mDuration, scene->mAnimations[0]->mTicksPerSecond, animInfo);
	if (this->mapAnimations.size() == 0)
	{
		// Add as current animation if empty
		this->currentAnimation = newAnim;
	}

	this->mapAnimations[newAnim->name] = newAnim;
}

void cAnimator::SetCurrentAnimation(const std::string& animationName, bool waitForLoop)
{
	// Check if the animation is already running
	if (animationName == this->currentAnimation->name)
	{
		return;
	}

	this->currentAnimation->currentTime = 0.0f;

	// Check if animation exists
	sAnimationState* anim = this->FindAnimation(animationName);
	if (anim != nullptr)
	{
		this->currentAnimation = anim;
		this->currentAnimation->bWaitForLoop = waitForLoop;
	}
}

// Call a temporary animation and go back to previous animation
void cAnimator::CallAnimation(const std::string& animationName)
{
	// Check if animation is already running
	if (animationName == this->currentAnimation->name)
	{
		return;
	}

	this->currentAnimation->currentTime = 0.0f;
	
	// Save current animation
	sAnimationState* oldAnim = this->currentAnimation;

	// Get new animation
	sAnimationState* newAnim = this->FindAnimation(animationName);

	// Check if animation exists
	if (newAnim == nullptr)
	{
		return;
	}

	// Set animation on the queue
	this->vecAnimationQueue.push_back(newAnim);
	this->vecAnimationQueue.push_back(oldAnim);

	// Set current animation to play
	this->currentAnimation = newAnim;
}

void cAnimator::QueueAnimation(const std::string& animationName)
{
	// Get new animation
	sAnimationState* newAnim = this->FindAnimation(animationName);

	if (newAnim == nullptr)
	{
		return;
	}

	this->vecAnimationQueue.push_back(newAnim);
}

void cAnimator::CheckAnimationState()
{
	if (this->vecAnimationQueue.size() > 0)
	{
		this->currentIndex = ((this->currentIndex + 1) >= this->vecAnimationQueue.size() ? 0 : this->currentIndex + 1);
		this->SetCurrentAnimation(this->vecAnimationQueue[this->currentIndex]->name);
	}
}

void cAnimator::UpdateSystem()
{
	this->AnimationBoneTransform(0.0f, this->currentAnimation->name);
	bool reset = this->currentAnimation->IncrementTime();

	if (reset)
	{
		CheckAnimationState();
	}
}

void cAnimator::AnimationBoneTransform(float TimeInSeconds, std::string animationName)  //, std::vector<glm::mat4> &FinalTransformation,
	//std::vector<glm::mat4> &Globals, std::vector<glm::mat4> &Offsets)
{
	if (this->currentAnimation->bHold)
	{
		return;
	}

	glm::mat4 Identity(1.0f);

	const aiScene* pScene = this->currentAnimation->animInfo.pAIScene;

	/*float TicksPerSecond = static_cast<float>(pScene->mAnimations[0]->mTicksPerSecond != 0 ?
		pScene->mAnimations[0]->mTicksPerSecond : 25.0);*/

	float TimeInTicks = this->currentAnimation->currentTime * this->currentAnimation->frameStepTime;                   //TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, this->currentAnimation->totalTime);

	// use the "animation" file to look up these nodes
	// (need the matOffset information from the animation file)
	this->ReadNodeHeirarchy(AnimationTime, animationName, pScene->mRootNode, Identity);

	sSkinnedModelInfo* skinnedMesh = this->currentMesh->skinnedModelInfo;
	skinnedMesh->vecFinalTransformation.resize(skinnedMesh->numBones);
	skinnedMesh->vecBoneGlobals.resize(skinnedMesh->numBones);
	skinnedMesh->vecOffsets.resize(skinnedMesh->numBones);

	for (unsigned int BoneIndex = 0; BoneIndex < skinnedMesh->numBones; BoneIndex++)
	{
		skinnedMesh->vecFinalTransformation[BoneIndex] = skinnedMesh->vecBoneInfo[BoneIndex].FinalTransformation;
		skinnedMesh->vecBoneGlobals[BoneIndex] = skinnedMesh->vecBoneInfo[BoneIndex].ObjectBoneTransformation;
		skinnedMesh->vecOffsets[BoneIndex] = skinnedMesh->vecBoneInfo[BoneIndex].BoneOffset;
	}
}

void cAnimator::ReadNodeHeirarchy(float AnimationTime, std::string& animationName, const aiNode* pNode, const glm::mat4 &ParentTransformMatrix)
{
	aiString NodeName(pNode->mName.data);

	const aiScene* pScene = this->currentAnimation->animInfo.pAIScene;

	// Original version picked the "main scene" animation...
	const aiAnimation* pAnimation = pScene->mAnimations[0];

	// Search for the animation we want
	std::map< std::string, sAnimationState* >::iterator itAnimation = this->mapAnimations.find(animationName);		// Animations

	// Did we find it? 
	if (itAnimation != this->mapAnimations.end())
	{
		// Yes, there is an animation called that...
		// ...replace the animation with the one we found
		pAnimation = reinterpret_cast<const aiAnimation*>(itAnimation->second->animInfo.pAIScene->mAnimations[0]);
	}

	// Transformation of the node in bind pose
	glm::mat4 NodeTransformation = AIMatrixToGLMMatrix(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = this->FindNodeAnimationChannel(pAnimation, NodeName);

	if (pNodeAnim)
	{
		// Get interpolated scaling
		glm::vec3 scale;
		this->CalcGLMInterpolatedScaling(AnimationTime, pNodeAnim, scale);
		glm::mat4 ScalingM = glm::scale(glm::mat4(1.0f), scale);

		// Get interpolated rotation (quaternion)
		glm::quat ori;
		this->CalcGLMInterpolatedRotation(AnimationTime, pNodeAnim, ori);
		glm::mat4 RotationM = glm::mat4_cast(ori);

		// Get interpolated position 
		glm::vec3 pos;
		this->CalcGLMInterpolatedPosition(AnimationTime, pNodeAnim, pos);
		glm::mat4 TranslationM = glm::translate(glm::mat4(1.0f), pos);


		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}
	//else
	//{
	//	// If there's no bone corresponding to this node, then it's something the animator used to 
	//	//	help make or position the model or animation. The point is that it DOESN'T change
	//	//	based on the animation frame, so take it just as is
	//	NodeTransformation = AIMatrixToGLMMatrix( pNode->mTransformation );
	//}

	glm::mat4 ObjectBoneTransformation = ParentTransformMatrix * NodeTransformation;

	sSkinnedModelInfo* skinnedMesh = this->currentMesh->skinnedModelInfo;
	std::map<std::string, unsigned int>::iterator it = skinnedMesh->mapBoneNameToBoneIndex.find(std::string(NodeName.data));
	if (it != skinnedMesh->mapBoneNameToBoneIndex.end())
	{
		unsigned int BoneIndex = it->second;
		skinnedMesh->vecBoneInfo[BoneIndex].ObjectBoneTransformation = ObjectBoneTransformation;
		skinnedMesh->vecBoneInfo[BoneIndex].FinalTransformation =
											skinnedMesh->mGlobalInverseTransformation * ObjectBoneTransformation *
											skinnedMesh->vecBoneInfo[BoneIndex].BoneOffset;
	}
	else
	{
		int breakpoint = 0;
	}

	for (unsigned int ChildIndex = 0; ChildIndex != pNode->mNumChildren; ChildIndex++)
	{
		this->ReadNodeHeirarchy(AnimationTime, animationName,
			pNode->mChildren[ChildIndex], ObjectBoneTransformation);
	}

}


const aiNodeAnim* cAnimator::FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString boneName)
{
	for (unsigned int ChannelIndex = 0; ChannelIndex != pAnimation->mNumChannels; ChannelIndex++)
	{
		if (pAnimation->mChannels[ChannelIndex]->mNodeName == boneName)
		{
			return pAnimation->mChannels[ChannelIndex];
		}
	}
	return 0;
}

void cAnimator::CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::quat &out)
{
	if (pNodeAnim->mNumRotationKeys == 1)
	{
		out.w = pNodeAnim->mRotationKeys[0].mValue.w;
		out.x = pNodeAnim->mRotationKeys[0].mValue.x;
		out.y = pNodeAnim->mRotationKeys[0].mValue.y;
		out.z = pNodeAnim->mRotationKeys[0].mValue.z;
		return;
	}

	unsigned int RotationIndex = this->FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	if (Factor < 0.0f) Factor = 0.0f;
	if (Factor > 1.0f) Factor = 1.0f;
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

	glm::quat StartGLM = glm::quat(StartRotationQ.w, StartRotationQ.x, StartRotationQ.y, StartRotationQ.z);
	glm::quat EndGLM = glm::quat(EndRotationQ.w, EndRotationQ.x, EndRotationQ.y, EndRotationQ.z);

	out = glm::slerp(StartGLM, EndGLM, Factor);

	out = glm::normalize(out);

	return;
}

void cAnimator::CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3 &out)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		out.x = pNodeAnim->mPositionKeys[0].mValue.x;
		out.y = pNodeAnim->mPositionKeys[0].mValue.y;
		out.z = pNodeAnim->mPositionKeys[0].mValue.z;
		return;
	}

	unsigned int PositionIndex = this->FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	if (Factor < 0.0f) Factor = 0.0f;
	if (Factor > 1.0f) Factor = 1.0f;
	const aiVector3D& StartPosition = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& EndPosition = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	glm::vec3 start = glm::vec3(StartPosition.x, StartPosition.y, StartPosition.z);
	glm::vec3 end = glm::vec3(EndPosition.x, EndPosition.y, EndPosition.z);

	out = (end - start) * Factor + start;

	return;
}

void cAnimator::CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3 &out)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		out.x = pNodeAnim->mScalingKeys[0].mValue.x;
		out.y = pNodeAnim->mScalingKeys[0].mValue.y;
		out.z = pNodeAnim->mScalingKeys[0].mValue.z;
		return;
	}

	unsigned int ScalingIndex = this->FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	if (Factor < 0.0f) Factor = 0.0f;
	if (Factor > 1.0f) Factor = 1.0f;
	const aiVector3D& StartScale = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& EndScale = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	glm::vec3 start = glm::vec3(StartScale.x, StartScale.y, StartScale.z);
	glm::vec3 end = glm::vec3(EndScale.x, EndScale.y, EndScale.z);
	out = (end - start) * Factor + start;

	return;
}

unsigned int cAnimator::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int RotationKeyIndex = 0; RotationKeyIndex != pNodeAnim->mNumRotationKeys - 1; RotationKeyIndex++)
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[RotationKeyIndex + 1].mTime)
		{
			return RotationKeyIndex;
		}
	}

	return 0;
}

unsigned int cAnimator::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int PositionKeyIndex = 0; PositionKeyIndex != pNodeAnim->mNumPositionKeys - 1; PositionKeyIndex++)
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[PositionKeyIndex + 1].mTime)
		{
			return PositionKeyIndex;
		}
	}

	return 0;
}

unsigned int cAnimator::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int ScalingKeyIndex = 0; ScalingKeyIndex != pNodeAnim->mNumScalingKeys - 1; ScalingKeyIndex++)
	{
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[ScalingKeyIndex + 1].mTime)
		{
			return ScalingKeyIndex;
		}
	}

	return 0;
}

void cAnimator::HoldAnimation(bool isHolding)
{
	this->currentAnimation->bHold = isHolding;
}
