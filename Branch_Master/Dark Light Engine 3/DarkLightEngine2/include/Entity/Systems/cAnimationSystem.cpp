// cAnimationSystem.cpp

#include <Entity\Systems\cAnimationSystem.h>


cAnimationSystem::cAnimationSystem()
{
	return;
}

cAnimationSystem::~cAnimationSystem()
{
	return;
}

void cAnimationSystem::BoneTransform(float TimeInSeconds, std::string animationName, std::vector<glm::mat4> &FinalTransformation,
									 std::vector<glm::mat4> &Globals, std::vector<glm::mat4> &Offsets)
{
	glm::mat4 Identity(1.0f);

	float TicksPerSecond = static_cast<float>(this->pScene->mAnimations[0]->mTicksPerSecond != 0 ? this->pScene->mAnimations[0]->mTicksPerSecond : 25.0);

	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)this->pScene->mAnimations[0]->mDuration);

	// use the "animation" file to look up these nodes
	// (need the matOffset information from the animation file)
	this->ReadNodeHeirarchy(AnimationTime, animationName, this->pScene->mRootNode, Identity);

	/*FinalTransformation.resize(this->mNumBones);
	Globals.resize(this->mNumBones);
	Offsets.resize(this->mNumBones);

	for (unsigned int BoneIndex = 0; BoneIndex < this->mNumBones; BoneIndex++)
	{
		FinalTransformation[BoneIndex] = this->mBoneInfo[BoneIndex].FinalTransformation;
		Globals[BoneIndex] = this->mBoneInfo[BoneIndex].ObjectBoneTransformation;
		Offsets[BoneIndex] = this->mBoneInfo[BoneIndex].BoneOffset;
	}*/
}

void cAnimationSystem::CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, aiQuaternion& out)
{
	if (pNodeAnim->mNumRotationKeys == 1)
	{
		out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = this->FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
	out = out.Normalize();
}

void cAnimationSystem::CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = this->FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& StartPosition = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& EndPosition = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	out = (EndPosition - StartPosition) * Factor + StartPosition;
}

void cAnimationSystem::CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = this->FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& StartScale = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& EndScale = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	out = (EndScale - StartScale) * Factor + StartScale;
}

void cAnimationSystem::CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::quat& out)
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
}

void cAnimationSystem::CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out)
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
}

void cAnimationSystem::CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out)
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
}

unsigned int cAnimationSystem::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
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

unsigned int cAnimationSystem::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
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

unsigned int cAnimationSystem::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
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

const aiNodeAnim* cAnimationSystem::FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString nodeOrBoneName)
{
	/*for (unsigned int ChannelIndex = 0; ChannelIndex != pAnimation->mNumChannels; ChannelIndex++)
	{
		if (pAnimation->mChannels[ChannelIndex]->mNodeName == boneName)
		{
			return pAnimation->mChannels[ChannelIndex];
		}
	}
	*/
	return 0;
}

void cAnimationSystem::ReadNodeHeirarchy(float AnimationTime, std::string animationName, const aiNode* pNode, const glm::mat4 &parentTransformMatrix)
{
	//aiString NodeName(pNode->mName.data);

	//// Original version picked the "main scene" animation...
	//const aiAnimation* pAnimation = this->pScene->mAnimations[0];

	//// Search for the animation we want... 
	//std::map< std::string, const aiScene* >::iterator itAnimation
	//	= mapAnimationNameTo_pScene.find(animationName);

	//// Did we find it? 
	//if (itAnimation != mapAnimationNameTo_pScene.end())
	//{
	//	// Yes, there is an animation called that...
	//	// ...replace the animation with the one we found
	//	pAnimation = reinterpret_cast<const aiAnimation*>(itAnimation->second->mAnimations[0]);
	//}


	////aiMatrix4x4 NodeTransformation;

	//// Transformation of the node in bind pose
	//glm::mat4 NodeTransformation = AIMatrixToGLMMatrix(pNode->mTransformation);

	//const aiNodeAnim* pNodeAnim = this->FindNodeAnimationChannel(pAnimation, NodeName);

	//if (pNodeAnim)
	//{
	//	// Get interpolated scaling
	//	glm::vec3 scale;
	//	this->CalcGLMInterpolatedScaling(AnimationTime, pNodeAnim, scale);
	//	glm::mat4 ScalingM = glm::scale(glm::mat4(1.0f), scale);

	//	// Get interpolated rotation (quaternion)
	//	glm::quat ori;
	//	this->CalcGLMInterpolatedRotation(AnimationTime, pNodeAnim, ori);
	//	glm::mat4 RotationM = glm::mat4_cast(ori);

	//	// Get interpolated position 
	//	glm::vec3 pos;
	//	this->CalcGLMInterpolatedPosition(AnimationTime, pNodeAnim, pos);
	//	glm::mat4 TranslationM = glm::translate(glm::mat4(1.0f), pos);


	//	// Combine the above transformations
	//	NodeTransformation = TranslationM * RotationM * ScalingM;
	//}
	////else
	////{
	////	// If there's no bone corresponding to this node, then it's something the animator used to 
	////	//	help make or position the model or animation. The point is that it DOESN'T change
	////	//	based on the animation frame, so take it just as is
	////	NodeTransformation = AIMatrixToGLMMatrix( pNode->mTransformation );
	////}

	//glm::mat4 ObjectBoneTransformation = ParentTransformMatrix * NodeTransformation;

	//std::map<std::string, unsigned int>::iterator it = this->m_mapBoneNameToBoneIndex.find(std::string(NodeName.data));
	//if (it != this->m_mapBoneNameToBoneIndex.end())
	//{
	//	unsigned int BoneIndex = it->second;
	//	this->mBoneInfo[BoneIndex].ObjectBoneTransformation = ObjectBoneTransformation;
	//	this->mBoneInfo[BoneIndex].FinalTransformation = this->mGlobalInverseTransformation
	//		* ObjectBoneTransformation
	//		* this->mBoneInfo[BoneIndex].BoneOffset;
	//	//this->mBoneInfo[BoneIndex].FinalTransformation = GlobalTransformation 
	//	//	                                             * this->mBoneInfo[BoneIndex].BoneOffset;

	//}
	//else
	//{
	//	int breakpoint = 0;
	//}

	//for (unsigned int ChildIndex = 0; ChildIndex != pNode->mNumChildren; ChildIndex++)
	//{
	//	this->ReadNodeHeirarchy(AnimationTime, animationName,
	//		pNode->mChildren[ChildIndex], ObjectBoneTransformation);
	//}
}

void cAnimationSystem::UpdateSystem(double deltaTime)
{
}

bool cAnimationSystem::ScheduleEntity(sEntity* entity, iComponent* component)
{
	return true;
}

bool cAnimationSystem::UnscheduleEntity(sEntity* entity, iComponent* component)
{
	return true;
}
