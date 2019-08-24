#ifndef _sSkinnedMeshComponent_HG_
#define _sSkinnedMeshComponent_HG_

#include <Entity\Components\sMeshComponent.h>
#include <Animation\Assimp\scene.h>
#include <array>
#include <map>


struct sSkinnedMeshComponent : public sMeshComponent
{
protected:
	static const int VERTMAXNUMBONES = 4;

	struct sVertexBoneData
	{
		std::array<unsigned int, VERTMAXNUMBONES> ids;
		std::array<float, VERTMAXNUMBONES> weights;

		void AddBoneData(unsigned int BoneID, float Weight);
	};

	struct sBoneInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;
		glm::mat4 ObjectBoneTransformation;
	};

	void _LoadBones(const aiMesh* Mesh, std::vector<sVertexBoneData> &Bones);

public:
	sSkinnedMeshComponent(unsigned long long entityID);
	~sSkinnedMeshComponent();
	
	unsigned int numberOfBones;
	unsigned int numberOfVertices;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	std::map<std::string /*BoneName*/, unsigned int /*BoneIndex*/> mapBoneNameToBoneIndex;	//mMapping;
	std::vector<sBoneInfo> vecBoneInfo;
	std::vector<sVertexBoneData> vecVertexBoneData;	//Bones;
	glm::mat4 mGlobalInverseTransformation;

	// HACK: for now these maps will hold the animations, 
	// but maybe we should add an animator component later!
	std::map< std::string /*animationfile*/, const aiScene* > mapAnimationNameTo_pScene;

	// HACK: Also should be in an animator
	float GetAnimationTotalTime(std::string animationName);
	
};


#endif // !_sSkinnedMeshComponent_HG_
