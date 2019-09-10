// sSkinnedMeshComponent.cpp

#include <Entity\Components\Mesh\sSkinnedMeshComponent.h>
#include <Animation\nAssimpHelper.h>


sSkinnedMeshComponent::sSkinnedMeshComponent(unsigned long long entityID) : sMeshComponent(entityID)
{
	this->numberOfBones = 0;
	this->numberOfVertices = 0;
	this->numberOfTriangles = 0;
	this->numberOfIndices = 0;

	return;
}

sSkinnedMeshComponent::~sSkinnedMeshComponent()
{
	return;
}

void sSkinnedMeshComponent::sVertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
	for (unsigned int i = 0; i < sizeof(this->ids) / sizeof(this->ids[0]); i++)
	{
		if (this->weights[i] == 0.0f)
		{
			this->ids[i] = (float)BoneID;
			this->weights[i] = Weight;
			return;
		}
	}
}

void sSkinnedMeshComponent::_LoadBones(const aiMesh* Mesh, std::vector<sVertexBoneData>& vecBones)
{
	for (unsigned int boneIndex = 0; boneIndex != Mesh->mNumBones; boneIndex++)
	{
		unsigned int BoneIndex = 0;
		std::string BoneName(Mesh->mBones[boneIndex]->mName.data);

		//	std::map<std::string /*BoneName*/, unsigned int /*BoneIndex*/> mMapping;
		// 	std::vector<sBoneInfo> mInfo;

		std::map<std::string, unsigned int>::iterator it = this->mapBoneNameToBoneIndex.find(BoneName);
		if (it == this->mapBoneNameToBoneIndex.end())
		{
			BoneIndex = this->numberOfBones;
			this->numberOfBones++;
			sBoneInfo bi;
			this->vecBoneInfo.push_back(bi);

			this->vecBoneInfo[BoneIndex].BoneOffset = nAssimpHelper::AIMatrixToGLMMatrix(Mesh->mBones[boneIndex]->mOffsetMatrix);
			this->mapBoneNameToBoneIndex[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = it->second;
		}

		for (unsigned int WeightIndex = 0; WeightIndex != Mesh->mBones[boneIndex]->mNumWeights; WeightIndex++)
		{
			unsigned int VertexID = /*mMeshEntries[MeshIndex].BaseVertex +*/ Mesh->mBones[boneIndex]->mWeights[WeightIndex].mVertexId;
			float Weight = Mesh->mBones[boneIndex]->mWeights[WeightIndex].mWeight;
			this->vecVertexBoneData[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}
