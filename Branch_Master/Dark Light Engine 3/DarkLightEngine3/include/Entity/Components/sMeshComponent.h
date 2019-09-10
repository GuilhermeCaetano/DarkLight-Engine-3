#ifndef _sMeshComponent_HG_
#define _sMeshComponent_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Entity\Components\iComponent.h>
#include <Mesh\sModelDrawInfo.h>
#include <string>
#include <vector>

#include <Animation\cAnimator.h>


// Textures
struct sTextureInfo
{
	std::string texName = "";
	int texCachedTextureID = 0;
	float texStrength = 0.0f;
};

struct sMeshComponent : public iComponent
{
public:
	sMeshComponent(unsigned long long entityID);
	~sMeshComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "MESH_COMPONENT"; };

	bool SetMesh(const std::string& meshName);

	// Name and settings
	sModelDrawInfo* meshInfo;
	std::string meshName;
	glm::vec3 nonUniformScale;

	glm::vec4 materialDiffuse;
	glm::vec4 materialSpecular;

	float reflectionRate;
	float refractionRate;
	float blackThresholdToDiscardPixel;  // Minimum black colour for discarding pixels
	float alphaOverrideOnBlackPixel;  // How much alpha to scale on black pixels only, if we want it
	float heightMapRatio;

	bool bIsVisible;
	bool bIsWireframe;
	bool bUseVertexColour;
	bool bIsAffectedByLights;
	bool bIsSelected;
	bool bUseHeightMap;
	bool bFlattenHeightMap;
	bool bUseAlphaMap;

	std::vector<sTextureInfo> vecTextures;
	sTextureInfo heightMapTexture;
	sTextureInfo alphaMapTexture;

	//HACK: Make an animator component later
	void SetAnimator();
	cAnimator* animator;

protected:
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

};


#endif // !_sMeshComponent_HG_

