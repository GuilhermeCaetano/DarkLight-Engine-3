// sMeshComponent.cpp

#include <Entity\Components\sMeshComponent.h>
#include <Engine\cEngine.hpp>


sMeshComponent::sMeshComponent(unsigned long long entityID)
{
	this->_parentID = entityID;
	this->animator = nullptr;
	this->Start();

	return;
}

sMeshComponent::~sMeshComponent()
{
	if (this->animator != nullptr)
	{
		delete this->animator;
		this->animator = nullptr;
	}
	/*if (this->meshInfo != nullptr)
	{
		delete this->meshInfo;
		this->meshInfo = nullptr;
	}*/

	return;
}

void sMeshComponent::Start()
{
	this->meshInfo = nullptr;
	this->meshName = "";
	this->nonUniformScale = glm::vec3(1.0f);

	this->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->materialSpecular = glm::vec4(1.0f);

	this->reflectionRate = 0.0f;
	this->refractionRate = 0.0f;
	this->blackThresholdToDiscardPixel = 0.0f;
	this->alphaOverrideOnBlackPixel = 1.0f;
	this->heightMapRatio = 0.0f;

	this->bIsVisible = true;
	this->bIsWireframe = false;
	this->bUseVertexColour = false;
	this->bIsAffectedByLights = false;
	this->bIsSelected = false;
	this->bUseHeightMap = false;
	this->bFlattenHeightMap = false;
	this->bUseAlphaMap = false;


	this->animator = nullptr;
}

bool sMeshComponent::SetMesh(const std::string& meshName)
{
	// First check if there was a definition already,
	// and if so, delete it from memory before creating a new one.

	if (this->meshInfo != nullptr)
	{
		delete this->meshInfo;
		this->meshInfo = nullptr;
	}

	//this->meshInfo = new sModelDrawInfo();
	//meshInfo->meshFileName = meshName;

	bool result = cEngine::FindMeshByName(meshName, &this->meshInfo);
	if (!result)
	{
		return false;
	}

	return true;
}

void sMeshComponent::SetAnimator()
{
	if (this->meshInfo->skinnedModelInfo != nullptr)
	{
		this->animator = new cAnimator(this->meshInfo);
	}
}

void sMeshComponent::_SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter)
{
	prettyWriter.Key("MeshName"); prettyWriter.String(this->meshName.c_str());
	prettyWriter.Key("IsSkinnedMesh"); prettyWriter.Bool(this->meshInfo->skinnedModelInfo != nullptr);
	prettyWriter.Key("NonUniformScale"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->nonUniformScale.x); prettyWriter.Double((double)this->nonUniformScale.y); prettyWriter.Double((double)this->nonUniformScale.z);
	prettyWriter.EndArray();
	prettyWriter.Key("MaterialDiffuse"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->materialDiffuse.r); prettyWriter.Double((double)this->materialDiffuse.g); prettyWriter.Double((double)this->materialDiffuse.b); prettyWriter.Double((double)this->materialDiffuse.a);
	prettyWriter.EndArray();
	prettyWriter.Key("MaterialSpecular"); prettyWriter.StartArray();
	prettyWriter.Double((double)this->materialSpecular.r); prettyWriter.Double((double)this->materialSpecular.g); prettyWriter.Double((double)this->materialSpecular.b); prettyWriter.Double((double)this->materialSpecular.a);
	prettyWriter.EndArray();
	prettyWriter.Key("ReflectionRate"); prettyWriter.Double((double)this->reflectionRate);
	prettyWriter.Key("RefractionRate"); prettyWriter.Double((double)this->refractionRate);
	prettyWriter.Key("BlackThresholdToDiscardPixel"); prettyWriter.Double((double)this->blackThresholdToDiscardPixel);
	prettyWriter.Key("AlphaOverrideOnBlackPixel"); prettyWriter.Double((double)this->alphaOverrideOnBlackPixel);
	prettyWriter.Key("HeightMapRatio"); prettyWriter.Double((double)this->heightMapRatio);
	prettyWriter.Key("IsVisible"); prettyWriter.Bool(this->bIsVisible);
	prettyWriter.Key("IsWireframe"); prettyWriter.Bool(this->bIsWireframe);
	prettyWriter.Key("IsAffectedByLights"); prettyWriter.Bool(this->bIsAffectedByLights);
	prettyWriter.Key("UseHeightMap"); prettyWriter.Bool(this->bUseHeightMap);
	prettyWriter.Key("FlattenHeightMap"); prettyWriter.Bool(this->bFlattenHeightMap);
	prettyWriter.Key("UseAlphaMap"); prettyWriter.Bool(this->bUseAlphaMap);
	prettyWriter.Key("Textures"); prettyWriter.StartArray();
	for (int i = 0; i != this->vecTextures.size(); i++)
	{
		prettyWriter.StartArray();
		prettyWriter.String(this->vecTextures[i].texName.c_str()); prettyWriter.Double(this->vecTextures[i].texStrength); prettyWriter.Int(this->vecTextures[i].texCachedTextureID);
		prettyWriter.EndArray();
	}
	prettyWriter.EndArray();
	prettyWriter.Key("HeightMapTexture"); prettyWriter.StartArray();
	prettyWriter.String(this->heightMapTexture.texName.c_str()); prettyWriter.Double(this->heightMapTexture.texStrength); prettyWriter.Int(this->heightMapTexture.texCachedTextureID);
	prettyWriter.EndArray();
	prettyWriter.Key("AlphaMapTexture"); prettyWriter.StartArray();
	prettyWriter.String(this->alphaMapTexture.texName.c_str()); prettyWriter.Double(this->alphaMapTexture.texStrength); prettyWriter.Int(this->alphaMapTexture.texCachedTextureID);
	prettyWriter.EndArray();

	if (this->animator != nullptr)
	{
		prettyWriter.Key("Animations"); prettyWriter.StartArray();
		for (std::map<std::string, cAnimator::sAnimationState*>::iterator mapiter = this->animator->mapAnimations.begin(); mapiter != this->animator->mapAnimations.end(); mapiter++)
		{
			prettyWriter.String(mapiter->second->name.c_str());
		}
		prettyWriter.EndArray();
	}
	
}

void sMeshComponent::_DeserializeFromJson(rapidjson::Value::ConstValueIterator vIter)
{
	for (rapidjson::Value::ConstMemberIterator innerIter = vIter->MemberBegin(); innerIter != vIter->MemberEnd(); innerIter++)
	{
		if (innerIter->name == "MeshName") { this->meshName = innerIter->value.GetString(); }
		else if (innerIter->name == "NonUniformScale") { this->nonUniformScale.x = (float)innerIter->value.GetArray()[0].GetDouble(); this->nonUniformScale.y = (float)innerIter->value.GetArray()[1].GetDouble(); this->nonUniformScale.z = (float)innerIter->value.GetArray()[2].GetDouble(); }
		else if (innerIter->name == "MaterialDiffuse") { this->materialDiffuse.r = (float)innerIter->value.GetArray()[0].GetDouble(); this->materialDiffuse.g = (float)innerIter->value.GetArray()[1].GetDouble(); this->materialDiffuse.b = (float)innerIter->value.GetArray()[2].GetDouble(); this->materialDiffuse.a = (float)innerIter->value.GetArray()[3].GetDouble(); }
		else if (innerIter->name == "MaterialSpecular") { this->materialSpecular.r = (float)innerIter->value.GetArray()[0].GetDouble(); this->materialSpecular.g = (float)innerIter->value.GetArray()[1].GetDouble(); this->materialSpecular.b = (float)innerIter->value.GetArray()[2].GetDouble(); this->materialSpecular.a = (float)innerIter->value.GetArray()[3].GetDouble(); }
		else if (innerIter->name == "ReflectionRate") { this->reflectionRate = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "RefractionRate") { this->refractionRate = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "BlackThresholdToDiscardPixel") { this->blackThresholdToDiscardPixel = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "AlphaOverrideOnBlackPixel") { this->alphaOverrideOnBlackPixel = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "HeightMapRatio") { this->heightMapRatio = (float)innerIter->value.GetDouble(); }
		else if (innerIter->name == "IsVisible") { this->bIsVisible = innerIter->value.GetBool(); }
		else if (innerIter->name == "IsWireframe") { this->bIsWireframe = innerIter->value.GetBool(); }
		else if (innerIter->name == "IsAffectedByLights") { this->bIsAffectedByLights = innerIter->value.GetBool(); }
		else if (innerIter->name == "UseHeightMap") { this->bUseHeightMap = innerIter->value.GetBool(); }
		else if (innerIter->name == "FlattenHeightMap") { this->bFlattenHeightMap = innerIter->value.GetBool(); }
		else if (innerIter->name == "UseAlphaMap") { this->bUseAlphaMap = innerIter->value.GetBool(); }
		else if (innerIter->name == "Textures")
		{
			for (unsigned int i = 0; i != innerIter->value.GetArray().Size(); i++)
			{
				sTextureInfo texInfo;
				texInfo.texName = innerIter->value.GetArray()[i].GetArray()[0].GetString();
				texInfo.texStrength = (float)innerIter->value.GetArray()[i].GetArray()[1].GetDouble();
				texInfo.texCachedTextureID = innerIter->value.GetArray()[i].GetArray()[2].GetInt();
				vecTextures.push_back(texInfo);
			}
		}
		else if (innerIter->name == "HeightMapTexture")
		{
			sTextureInfo texInfo;
			texInfo.texName = innerIter->value.GetArray()[0].GetString();
			texInfo.texStrength = (float)innerIter->value.GetArray()[1].GetDouble();
			texInfo.texCachedTextureID = innerIter->value.GetArray()[2].GetInt();
			this->heightMapTexture = texInfo;
		}
		else if (innerIter->name == "AlphaMapTexture")
		{
			sTextureInfo texInfo;
			texInfo.texName = innerIter->value.GetArray()[0].GetString();
			texInfo.texStrength = (float)innerIter->value.GetArray()[1].GetDouble();
			texInfo.texCachedTextureID = innerIter->value.GetArray()[2].GetInt();
			this->alphaMapTexture = texInfo;
		}
	}

	this->SetMesh(this->meshName);
}
