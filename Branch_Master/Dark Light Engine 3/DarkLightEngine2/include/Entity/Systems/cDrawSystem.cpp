// cDrawSystem.cpp

#include <Entity\Systems\cDrawSystem.h>
#include <Pools\cPoolManager.h>
#include <Mesh\sModelDrawInfo.h>
#include <IO\cIOManager.h>
#include <Math\Sort.h>
#include <Math\nQuaternionHelper.h>
#include <Scene\nSceneHelper.h>

#include <Entity\Components\sMeshComponent.h>
#include <Entity\Components\sLocationComponent.h>
#include <Entity\Components\sCameraComponent.h>
#include <Entity\Components\sLightComponent.h>
#include <Entity\Components\sPointsColliderComponent.h>
#include <SimplePhysics\Physics\sSimpleSoftBodyComponent.h>
#include <Entity\Components\AI\sNeuralNetworkComponent.h>
#include <AABB\cAABBManager.h>
#include <Debug\cDebugRenderer.h>
#include <Interfaces\Physics\iRigidbody.h>


cDrawSystem::cDrawSystem()
{
	this->_pLightHelper = new cLightHelper();
	this->_cameraMatProjection = glm::mat4x4(1.0f); // Identity

	this->_bTextureUniformLocationsLoaded = false;
	this->_tex_Uniloc[8] = { -1 };
	this->_texBW_UniLoc[2] = { -1 };

	this->_dayMix = 1.0f;

	this->_currentSelectedIndex = 5;
	this->_bIsChanging = false;

	this->_bNoLighting = false;
	this->_bNoTextures = false;
	this->_bNoSkybox = false;
	this->_bAllWireframe = false;
	this->_bDrawBoundingBoxes = true;
	this->_bUseBloom = false;
	this->_bUseCelShade = false;
	this->_bUseNightVision = false;

	this->time = 0.0f;
	this->_renderPassNum = 1.0f;
	
	this->_bIsFBO = false;
	this->_pFBO = nullptr;
	this->_pFBO2 = nullptr;

	this->_curWidth = 1280;
	this->_curHeight = 720;

	this->_pDebugRenderer = nullptr;

	return;
}

cDrawSystem::~cDrawSystem()
{
	delete this->_pLightHelper;

	return;
}

void cDrawSystem::SetupSystem(GLuint shaderProgramID, cVaoMeshManager* pVaoMeshManager, cTextureManager* pTextureManager, cAABBManager* pAABBManager)
{
	this->_pVaoMeshManager = pVaoMeshManager;
	this->_pTextureManager = pTextureManager;
	this->_pAABBManager = pAABBManager;

	this->_SetUniformLocations(shaderProgramID);
}

void cDrawSystem::SetDebugRenderer(cDebugRenderer* pDebugRenderer)
{
	this->_pDebugRenderer = pDebugRenderer;
}

glm::mat4 cDrawSystem::GetCameraProjection()
{
	return this->_cameraMatProjection;
}

void cDrawSystem::SetCameraProjection(float fov, float ratio, float nearClip, float farClip)
{
	// Set main projection matrix, the camera one
	this->_cameraMatProjection = glm::perspective(fov, ratio, nearClip, farClip);
}

void cDrawSystem::SetFBO(cFBO* pFBO)
{
	if (this->_pFBO == nullptr)
	{
		this->_pFBO = pFBO;
	}
	else
	{
		this->_pFBO2 = pFBO;
	}
}

void cDrawSystem::_SetUniformLocations(GLuint shaderProgramID)
{
	// Set FBO Uniform Locations
	this->_renderPassNumber_UniLoc = glGetUniformLocation(shaderProgramID, "renderPassNumber");

	// Set Mesh Uniform Locations
	this->_matModelTrans_UniLoc = glGetUniformLocation(shaderProgramID, "matModel");
	this->_matModelInvTrans_UniLoc = glGetUniformLocation(shaderProgramID, "matModelInvTrans");
	this->_matView_UniLoc = glGetUniformLocation(shaderProgramID, "matView");
	this->_matProj_UniLoc = glGetUniformLocation(shaderProgramID, "matProj");
	this->_eye_UniLoc = glGetUniformLocation(shaderProgramID, "eyeLocation");
	this->_useVertexColour_UniLoc = glGetUniformLocation(shaderProgramID, "useVertexColour");
	this->_bIsLightAffecting_UniLoc = glGetUniformLocation(shaderProgramID, "isLightAffecting");
	this->_bIsSelected_UniLoc = glGetUniformLocation(shaderProgramID, "isSelected");
	this->_objectDiffuse_UniLoc = glGetUniformLocation(shaderProgramID, "objectDiffuse");
	this->_objectSpecular_UniLoc = glGetUniformLocation(shaderProgramID, "objectSpecular");
	this->_reflectionRate_UniLoc = glGetUniformLocation(shaderProgramID, "reflectionRate");
	this->_refractionRate_UniLoc = glGetUniformLocation(shaderProgramID, "refractionRate");

	// Set Skinned Mesh Uniform Locations
	this->_bIsSkinnedMesh_Uniloc = glGetUniformLocation(shaderProgramID, "bIsSkinnedMesh");
	this->_numBonesUsed_Uniloc = glGetUniformLocation(shaderProgramID, "numBonesUsed");
	this->_bones_Uniloc = glGetUniformLocation(shaderProgramID, "bones");

	// Set Texture Uniform Locations
	if (!this->_bTextureUniformLocationsLoaded)
	{
		this->_tex_Uniloc[0] = glGetUniformLocation(shaderProgramID, "texture00");
		this->_tex_Uniloc[1] = glGetUniformLocation(shaderProgramID, "texture01");
		this->_tex_Uniloc[2] = glGetUniformLocation(shaderProgramID, "texture02");
		this->_tex_Uniloc[3] = glGetUniformLocation(shaderProgramID, "texture03");
		this->_tex_Uniloc[4] = glGetUniformLocation(shaderProgramID, "texture04");
		this->_tex_Uniloc[5] = glGetUniformLocation(shaderProgramID, "texture05");
		this->_tex_Uniloc[6] = glGetUniformLocation(shaderProgramID, "texture06");
		this->_tex_Uniloc[7] = glGetUniformLocation(shaderProgramID, "texture07");

		this->_texBW_UniLoc[0] = glGetUniformLocation(shaderProgramID, "texBlendWeights[0]");
		this->_texBW_UniLoc[1] = glGetUniformLocation(shaderProgramID, "texBlendWeights[1]");

		this->_bTextureUniformLocationsLoaded = true;
	}

	this->_texPass1OutputTexture_UniLoc = glGetUniformLocation(shaderProgramID, "texPass1OutputTexture");

	// Set Height Map Uniform Locations
	this->_heightTex_UniLoc = glGetUniformLocation(shaderProgramID, "heightMapTexture");
	this->_bUseHeightMap_UniLoc = glGetUniformLocation(shaderProgramID, "bUseHeightMap");
	this->_bFlattenHeightMap_UniLoc = glGetUniformLocation(shaderProgramID, "bFlatten");
	this->_heightMapRatio_UniLoc = glGetUniformLocation(shaderProgramID, "heightMapRatio");

	// Set Alpha Map Uniform Locations
	this->_alphaTex_UniLoc = glGetUniformLocation(shaderProgramID, "alphaMapTexture");
	this->_bUseAlphaMap_UniLoc = glGetUniformLocation(shaderProgramID, "bUseAlphaMap");
	this->_blackThreshold_UniLoc = glGetUniformLocation(shaderProgramID, "blackThreshold");
	this->_alphaOverride_UniLoc = glGetUniformLocation(shaderProgramID, "alphaTextureOverride");

	// Set Skybox Uniform Locations
	this->_skyBoxDayCubeMap_UniLoc = glGetUniformLocation(shaderProgramID, "textureSkyBoxDay");
	this->_skyBoxNightCubeMap_UniLoc = glGetUniformLocation(shaderProgramID, "textureSkyBoxNight");
	this->_dayMix_UniLoc = glGetUniformLocation(shaderProgramID, "dayMix");
	this->_bUseSkyBoxTexture_UniLoc = glGetUniformLocation(shaderProgramID, "useSkyBoxTexture");

	// Set Shader Aspects Uniform Locations
	this->_bNoLighting_UniLoc = glGetUniformLocation(shaderProgramID, "bNoLighting");
	this->_bNoTextures_UniLoc = glGetUniformLocation(shaderProgramID, "bNoTextures");
	this->_bNoSkybox_UniLoc = glGetUniformLocation(shaderProgramID, "bNoSkybox");
	this->_bUseBloom_UniLoc = glGetUniformLocation(shaderProgramID, "bUseBloom");
	this->_bUseCelShade_UniLoc = glGetUniformLocation(shaderProgramID, "bUseCelShade");
	this->_bUseNightVision_UniLoc = glGetUniformLocation(shaderProgramID, "bUseNightVision");
}

// Matrix creation
glm::mat4 ObjectMatrix(glm::mat4x4& mat, sLocationComponent* locationComponent, sMeshComponent* meshComponent)
{
	// Identity (4x4 - 1) matrix
	mat = glm::mat4x4(1.0f);

	// Calculate translation
	glm::mat4 matMove = glm::translate(glm::mat4(1.0f), locationComponent->GetWorldPosition());
	mat = mat * matMove;

	// Get current quaternion rotation and calculate modified rotation
	glm::quat qRotation = locationComponent->GetWorldRotation();
	glm::mat4 matRotation = glm::mat4(qRotation);
	mat = mat * matRotation;

	// Calculate the inverse transpose before scaling
	glm::mat4 matModelInvTrans = glm::inverse(glm::transpose(mat));

	// Calculate scaling
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), meshComponent->nonUniformScale);
	mat = mat * matScale;

	return matModelInvTrans;
}

// Matrix creation
glm::mat4 ObjectMatrix(glm::mat4x4& mat, glm::vec3 position, sEntity* entity, sMeshComponent* meshComponent)
{
	// Identity (4x4 - 1) matrix
	mat = glm::mat4x4(1.0f);

	// Calculate translation
	glm::mat4 matMove = glm::translate(glm::mat4(1.0f), position);
	mat = mat * matMove;

	// Get current quaternion rotation and calculate modified rotation
	glm::quat qRotation = entity->transform->GetWorldRotation();
	glm::mat4 matRotation = glm::mat4(qRotation);
	mat = mat * matRotation;

	// Calculate the inverse transpose before scaling
	glm::mat4 matModelInvTrans = glm::inverse(glm::transpose(mat));

	// Calculate scaling
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), meshComponent->nonUniformScale);
	mat = mat * matScale;

	return matModelInvTrans;
}

void cDrawSystem::DrawObjects(GLuint shaderProgramID)
{
	cPoolManager* tempPool = cPoolManager::GetInstance();

	// Objects need a mesh in order to be drawn
	std::vector<sEntity*> alphaDrawEntities;
	
	sMeshComponent* meshC = nullptr;
	sEntity* worldCamera = cPoolManager::GetInstance()->GetEntityByName("WorldCamera");
	sCameraComponent* cameraC = worldCamera->GetComponent<sCameraComponent>();
	for (unsigned int i = 0; i != tempPool->vecStaticEntities.size(); i++)
	{
		sEntity* entity = tempPool->vecStaticEntities[i];
		meshC = entity->GetComponent<sMeshComponent>();

		// Draw all childs too!
		for (unsigned int j = 0; j < entity->GetNumChilds(); j++)
		{
			sEntity* child = entity->GetChildAt(j);
			sMeshComponent* childMeshC = child->GetComponent<sMeshComponent>();

			if (childMeshC == nullptr || !childMeshC->bIsVisible)
			{
				continue;
			}

			if (childMeshC->materialDiffuse.a >= 1.0f && !childMeshC->bUseAlphaMap)
			{
				this->DrawOneObject(child, shaderProgramID);
			}
			else
			{
				alphaDrawEntities.push_back(child);
			}
		}

		if (meshC == nullptr || !meshC->bIsVisible)
		{
			continue;
		}

		if (meshC->materialDiffuse.a >= 1.0f && !meshC->bUseAlphaMap)
		{
			this->DrawOneObject(entity, shaderProgramID);
		}
		else
		{
			alphaDrawEntities.push_back(entity);
		}
	}

	// Now draw everything that has alpha in order
	BubbleSort(alphaDrawEntities, cameraC->eyePosition);
	for (unsigned int i = 0; i < alphaDrawEntities.size(); i++)
	{
		float cameraDistance = fabs(glm::distance(cameraC->eyePosition, alphaDrawEntities[i]->transform->GetWorldPosition()));
		meshC = alphaDrawEntities[i]->GetComponent<sMeshComponent>();
		DrawCall(shaderProgramID, alphaDrawEntities[i], alphaDrawEntities[i]->transform->GetWorldPosition(), cameraDistance);
	}

	// Draw all bullets
	//for (unsigned int i = 0; i != tempPool->vecBulletEntities.size(); i++)
	//{
	//	sEntity* entity = tempPool->vecBulletEntities[i];
	//	if (!entity->bIsActive)
	//	{
	//		continue;
	//	}

	//	meshC = entity->GetTComponent<sMeshComponent*>(MESH_COMPONENT);
	//	if (meshC != nullptr)
	//	{
	//		sEntity* worldCamera = cPoolManager::GetInstance()->GetEntityByName("WorldCamera");
	//		sCameraComponent* cameraC = static_cast<sCameraComponent*>(worldCamera->GetComponent(eComponentTypes::CAMERA_COMPONENT));

	//		if (tempPool->vecBulletEntities[i]->entityName == "SkyboxSphere")
	//		{
	//			this->_BindSkyboxTexture(shaderProgramID);
	//			entity->transform->position = cameraC->eyePosition;
	//		}

	//		// First check if the entity mesh has alpha, and draw first the ones that don't have it
	//		if (meshC->materialDiffuse.a >= 1.0f && !meshC->bUseAlphaMap)
	//		{
	//			DrawCall(shaderProgramID, meshC, entity->transform);
	//		}
	//		/*else
	//		{
	//			alphaDrawEntities.push_back(tempPool->vecBulletEntities[i]);
	//		}*/
	//	}
	//}
}

void cDrawSystem::DrawOneObject(sEntity* entity, GLuint shaderProgramID)
{
	cPoolManager* tempPool = cPoolManager::GetInstance();
	sMeshComponent* meshC = entity->GetComponent<sMeshComponent>();
	sEntity* worldCamera = cPoolManager::GetInstance()->GetEntityByName("WorldCamera");
	sCameraComponent* cameraC = worldCamera->GetComponent<sCameraComponent>();
	nPhysics::iRigidbody* rigidbodyC = entity->GetComponent<nPhysics::iRigidbody>();
	sNeuralNetworkComponent* neuralC = entity->GetComponent<sNeuralNetworkComponent>();
	if (meshC != nullptr && entity->GetIsActive())
	{
		if (entity->GetName() == "SkyboxSphere")
		{
			this->_BindSkyboxTexture(shaderProgramID);
			entity->transform->GetWorldPosition() = cameraC->eyePosition;
		}

		if (entity->GetTag() == "Cloth")
		{
			nPhysics::sSimpleSoftBodyComponent* softbody = dynamic_cast<nPhysics::sSimpleSoftBodyComponent*>(entity->GetComponent<nPhysics::iRigidbody>());
			sEntity* debugSphere = tempPool->GetEntityByName("DebugSphere");
			sEntity* AABB = tempPool->GetEntityByName("AABB");
			sMeshComponent* debugMesh = debugSphere->GetComponent<sMeshComponent>();
			sMeshComponent* aabbMesh = AABB->GetComponent<sMeshComponent>();
			debugMesh->materialDiffuse = glm::vec4(1.0f);
			debugMesh->bIsVisible = true;
			glm::vec3 position;
			for (unsigned int j = 0; j < softbody->GetNumNodes(); j++)
			{
				float distance = fabs(glm::distance(debugSphere->transform->GetWorldPosition(), cameraC->eyePosition));
				position = softbody->GetNodePosition(j);
				debugMesh->nonUniformScale = glm::vec3(softbody->GetNodeRadius(j) * 2.0f);
				debugSphere->transform->SetWorldPosition(position);
				this->DrawCall(shaderProgramID, debugSphere, debugSphere->transform->GetWorldPosition(), distance);
			}
			debugMesh->nonUniformScale = glm::vec3(1.0f);
			debugMesh->bIsVisible = false;

			glm::vec3 minBonds;
			glm::vec3 maxBonds;
			softbody->GetAABB(minBonds, maxBonds);
			AABB->transform->SetWorldPosition(glm::vec3((maxBonds.x + minBonds.x) * 0.5f, (maxBonds.y + minBonds.y) * 0.5f, (maxBonds.z + minBonds.z) * 0.5f));

			aabbMesh->nonUniformScale = glm::vec3(fabs(maxBonds.x - minBonds.x), fabs(maxBonds.y - minBonds.y), fabs(maxBonds.z - minBonds.z));
			aabbMesh->materialDiffuse = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			aabbMesh->bIsVisible = true;
			this->DrawCall(shaderProgramID, AABB, AABB->transform->GetWorldPosition(), 0.0f);
			aabbMesh->nonUniformScale = glm::vec3(1.0f);
			aabbMesh->materialDiffuse = glm::vec4(1.0f);
			aabbMesh->bIsVisible = false;

			return;
		}

		if (neuralC != nullptr)
		{
			sEntity* debugSphere = tempPool->GetEntityByName("DebugSphere");
			sMeshComponent* debugMesh = debugSphere->GetComponent<sMeshComponent>();
			debugMesh->materialDiffuse = glm::vec4(1.0f);
			debugMesh->bIsVisible = true;
			glm::vec3 position;
			for (unsigned int i = 0; i < neuralC->detectorPoints.size(); i++)
			{
				position = neuralC->detectorPoints[i];
				debugMesh->nonUniformScale = glm::vec3(1.0f);
				debugSphere->transform->SetWorldPosition(position);
				this->DrawCall(shaderProgramID, debugSphere, debugSphere->transform->GetWorldPosition(), 0.0f);
			}

			debugMesh->nonUniformScale = glm::vec3(1.0f);
			debugMesh->bIsVisible = false;
		}

		if (rigidbodyC != nullptr) //HACK: make this more appropriately later
		{
			nPhysics::iRigidbody* rigidbody = entity->GetComponent<nPhysics::iRigidbody>();

			glm::mat4x4 matModel = glm::mat4(1.0f);
			ObjectMatrix(matModel, entity->transform, meshC);

			glm::vec3 minBonds = glm::vec3(0.0f);
			glm::vec3 maxBonds = glm::vec3(5.0f);
			rigidbody->GetAABB(minBonds, maxBonds);

			glm::mat4 moveMatrix = rigidbody->GetTransform();
			glm::vec3 position = moveMatrix[3];
			glm::quat rotation = glm::quat(moveMatrix);
			glm::vec3 scale = glm::vec3((maxBonds - minBonds));

			this->DrawColliderAt("AABB", position, rotation, scale, shaderProgramID);
		}

		if (entity->GetTag() == "FBO")
		{
			this->_bIsFBO = true;
		}
		else
		{
			this->_bIsFBO = false;
		}

		float cameraDistance = fabs(glm::distance(entity->transform->GetWorldPosition(), cameraC->eyePosition));
		DrawCall(shaderProgramID, entity, entity->transform->GetWorldPosition(), cameraDistance);
	}
}

void cDrawSystem::_BindTextures(GLuint shaderProgramID, iComponent* iMeshC)
{
	// Check for the FBO
	if (this->_bIsFBO)
	{
		// Connect the texture for this object to the FBO texture
		// Pick texture unit 16 (just because - I randomly picked that)

		int FBOTexUnit = 1;

		// 0x84C0  (or 33984)		
		// Please bind to texture unit 34,000. Why gawd, why?
		glActiveTexture(GL_TEXTURE0 + FBOTexUnit);

		// Connect the specific texture to THIS texture unit
//		glBindTexture( GL_TEXTURE_2D, g_FBO_colourTexture );
		glBindTexture(GL_TEXTURE_2D, this->_pFBO->_colourTexture0_ID[0]);
		//glBindTexture(GL_TEXTURE_2D, this->_pFBO->_colourTexture1_ID);

		// Now pick to read from the normal (output from the 1st pass):
		//glBindTexture( GL_TEXTURE_2D, this->_pFBO->_normalTexture1_ID );
		//glBindTexture( GL_TEXTURE_2D, this->_pFBO->_depthTexture_ID );
		//glBindTexture( GL_TEXTURE_2D, this->_pFBO->_vertexWorldPos2_ID );


		// Set the sampler (in the shader) to ALSO point to texture unit 16
		// This one takes the unchanged texture unit numbers 
//		glUniform1i( tex00_UniLoc, FBO_Texture_Unit_Michael_Picked );
		glUniform1i(this->_texPass1OutputTexture_UniLoc, FBOTexUnit);

		// Texture binding to the texture unit
		//sMeshComponent* meshC = static_cast<sMeshComponent*>(iMeshC);
		//GLuint alphaTextureID = this->_pTextureManager->getTextureIDFromName(meshC->alphaMapTexture.texName);
		//GLuint textureUNIT_ID = 51;			// 0 to 79
		//glActiveTexture(textureUNIT_ID + GL_TEXTURE0);	// GL_TEXTURE0 = 33984
		//glBindTexture(GL_TEXTURE_2D, alphaTextureID);
		//GLint tex_UniLoc = glGetUniformLocation(shaderProgramID, "passExtraTex");
		//glUniform1i(tex_UniLoc, textureUNIT_ID);
		// Set the blending to that it's 0th texture sampler
		// NOTE: it's only the 0th (1st) texture that we are mixing from
//		glUniform4f( texBW_0_UniLoc, 1.0f, 0.0f, 0.0f, 0.0f );		// <---- Note the 1.0f
//		glUniform4f( texBW_1_UniLoc, 0.0f, 0.0f, 0.0f, 0.0f );

		// NOTE: Early return (so we don't set any other textures
		// Again; HACK!!
		return;
	}//if ( pCurrentMesh->b_HACK_UsesOffscreenFBO )

	float blendWeights[8] = { 0 };
	sMeshComponent* meshC = static_cast<sMeshComponent*>(iMeshC);
	// For each texture, bind the texture to a texture unit and sampler
	// Texture #0 (on the mesh) -- Texture Unit 0 -- Sampler 0
	// Texture #1 (on the mesh) -- Texture Unit 1 -- Sampler 1
	for (int texBindIndex = 0; texBindIndex != meshC->vecTextures.size(); texBindIndex++)
	{
		// Bind to the "texBindIndex" texture unit
		glActiveTexture(GL_TEXTURE0 + texBindIndex);

		// Connect the specific texture to this texture unit
		std::string texName = meshC->vecTextures[texBindIndex].texName;

		//GLuint texID = _pTextureManager->getTextureIDFromName(texName);
		GLuint texID = _pTextureManager->GetThreadedTextureIDFromName(texName);

		glBindTexture(GL_TEXTURE_2D, texID);

		// Pick the texture sampler and weight (strength)
		glUniform1i(_tex_Uniloc[texBindIndex], texBindIndex);

		// Set the weights
		blendWeights[texBindIndex] = meshC->vecTextures[texBindIndex].texStrength;
	}

	// Set the weights on the shader
	glUniform4f(this->_texBW_UniLoc[0], blendWeights[0], blendWeights[1], blendWeights[2], blendWeights[3]);
	glUniform4f(this->_texBW_UniLoc[1], blendWeights[4], blendWeights[5], blendWeights[6], blendWeights[7]);

	return;
}

void cDrawSystem::_BindHeightMapTextures(GLuint shaderProgramID, iComponent* iMeshC)
{
	sMeshComponent* meshC = static_cast<sMeshComponent*>(iMeshC);
	glUniform1f(this->_bUseHeightMap_UniLoc, (float)meshC->bUseHeightMap);
	glUniform1f(this->_bFlattenHeightMap_UniLoc, (float)meshC->bFlattenHeightMap);
	glUniform1f(this->_heightMapRatio_UniLoc, meshC->heightMapRatio);
	
	if (!meshC->bUseHeightMap)
	{
		return;
	}

	// Texture binding to the texture unit
	GLuint heightTextureID = this->_pTextureManager->getTextureIDFromName(meshC->heightMapTexture.texName);
	GLuint textureUNIT_ID = 50;			// 0 to 79
	glActiveTexture(textureUNIT_ID + GL_TEXTURE0);	// GL_TEXTURE0 = 33984
	glBindTexture(GL_TEXTURE_2D, heightTextureID);
	glUniform1i(this->_heightTex_UniLoc, textureUNIT_ID);
}

void cDrawSystem::_BindAlphaMapTextures(GLuint shaderProgramID, iComponent* iMeshC)
{
	sMeshComponent* meshC = static_cast<sMeshComponent*>(iMeshC);
	glUniform1f(this->_bUseAlphaMap_UniLoc, (float)meshC->bUseAlphaMap);
	glUniform1f(this->_blackThreshold_UniLoc, meshC->blackThresholdToDiscardPixel);
	glUniform1f(this->_alphaOverride_UniLoc, meshC->alphaOverrideOnBlackPixel);
	
	if (!meshC->bUseAlphaMap)
	{
		return;
	}

	// Texture binding to the texture unit
	GLuint alphaTextureID = this->_pTextureManager->getTextureIDFromName(meshC->alphaMapTexture.texName);
	GLuint textureUNIT_ID = 51;			// 0 to 79
	glActiveTexture(textureUNIT_ID + GL_TEXTURE0);	// GL_TEXTURE0 = 33984
	glBindTexture(GL_TEXTURE_2D, alphaTextureID);
	glUniform1i(this->_alphaTex_UniLoc, textureUNIT_ID);
}

void cDrawSystem::_BindSkyboxTexture(GLuint shaderProgramID)
{
	// Set the current active texture to be bound
	GLuint brightSkyUNIT_ID = 30;			// Texture unit go from 0 to 79
	glActiveTexture(brightSkyUNIT_ID + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

	// Find the ID attributed on creation by the name
	int cubeMapTextureID = this->_pTextureManager->getTextureIDFromName("BrightSkyCubeMap");

	// Cube map is now bound to texture unit 30
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);

	// Set shader skybox texture locations
	glUniform1i(this->_skyBoxDayCubeMap_UniLoc, brightSkyUNIT_ID);

	// Set the current active texture to be bound
	brightSkyUNIT_ID = 31;			// Texture unit go from 0 to 79
	glActiveTexture(brightSkyUNIT_ID + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

	// Find the ID attributed on creation by the name
	cubeMapTextureID = this->_pTextureManager->getTextureIDFromName("SpaceBoxCubeMap");

	// Cube map is now bound to texture unit 30
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);

	// Set shader skybox texture locations
	glUniform1i(this->_skyBoxNightCubeMap_UniLoc, brightSkyUNIT_ID);

	glUniform1f(this->_dayMix_UniLoc, this->_dayMix);

	// Get and set shader skybox option
	glUniform1f(this->_bUseSkyBoxTexture_UniLoc, (float)GL_TRUE);
}

//void cDrawSystem::DrawCall(GLuint shaderProgramID, iComponent* mComponent, iComponent* lComponent)
//{
//	// Check if mesh should be drawn in the first place
//	sMeshComponent* meshC = static_cast<sMeshComponent*>(mComponent);
//	if (meshC->bIsVisible == false)
//	{
//		return;
//	}
//
//	//this->_currentMesh = meshC;
//	sLocationComponent* locationC = static_cast<sLocationComponent*>(lComponent);
//
//	glUniform1f(this->_renderPassNumber_UniLoc, this->_renderPassNum);	// Tell shader which pass it is
//
//	// Bind textures
//	this->_BindTextures(shaderProgramID, meshC);
//	this->_BindHeightMapTextures(shaderProgramID, meshC);
//	this->_BindAlphaMapTextures(shaderProgramID, meshC);
//
//	// Set the shader to be used
//	glUseProgram(shaderProgramID);
//
//	// Create matrix, set current object matrix and send to the shader
//	glm::mat4x4 matModel = glm::mat4(1.0f);
//	glm::mat4 matModelInvTrans = ObjectMatrix(matModel, locationC, meshC);
//	glUniformMatrix4fv(this->_matModelTrans_UniLoc, 1, GL_FALSE, glm::value_ptr(matModel));
//	glUniformMatrix4fv(this->_matModelInvTrans_UniLoc, 1, GL_FALSE, glm::value_ptr(matModelInvTrans));
//
//	glUniform1f(this->_bNoLighting_UniLoc, (float)this->_bNoLighting);
//	glUniform1f(this->_bNoTextures_UniLoc, (float)this->_bNoTextures);
//	glUniform1f(this->_bNoSkybox_UniLoc, (float)this->_bNoSkybox);
//
//	// Enable blend or "alpha" transparency
//	if (meshC->materialDiffuse.a >= 1.0f && !meshC->bUseAlphaMap)
//	{
//		glDisable(GL_BLEND);
//	}
//	else
//	{
//		glEnable(GL_BLEND);
//	}
//
//	//glDisable( GL_BLEND );
//	// Source == already on framebuffer
//	// Dest == what you're about to draw
//	// This is the "regular alpha blend transparency"
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	glUniform4f(this->_objectDiffuse_UniLoc,
//		meshC->materialDiffuse.r,
//		meshC->materialDiffuse.g,
//		meshC->materialDiffuse.b,
//		meshC->materialDiffuse.a);
//	glUniform4f(this->_objectSpecular_UniLoc,
//		meshC->materialSpecular.r,
//		meshC->materialSpecular.g,
//		meshC->materialSpecular.b,
//		meshC->materialSpecular.a);
//
//	glUniform1f(this->_reflectionRate_UniLoc, meshC->reflectionRate);
//	glUniform1f(this->_refractionRate_UniLoc, meshC->refractionRate);
//
//	if (meshC->bIsWireframe || this->_bAllWireframe)
//	{
//		// Set opengl to show objects as lines, or "wireframe"
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		glDisable(GL_CULL_FACE);
//	}
//	else
//	{
//		// Set opengl to fill spaces, showing the "faces"
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//		glEnable(GL_CULL_FACE);
//	}
//
//	// Check if this is a skinned mesh
//	if (meshC->meshInfo->skinnedModelInfo != nullptr)
//	{
//		// It has a skinned mesh
//		glUniform1f(this->_bIsSkinnedMesh_Uniloc, (float)GL_TRUE);
//
//
//		// Also pass up the bone information...
//		std::vector< glm::mat4x4 > vecFinalTransformation;	// Replaced by	theMesh.vecFinalTransformation
//		std::vector< glm::mat4x4 > vecOffsets;
//
//		meshC->animator->UpdateSystem();
//
//		vecFinalTransformation = meshC->meshInfo->skinnedModelInfo->vecFinalTransformation;
//		vecOffsets = meshC->meshInfo->skinnedModelInfo->vecOffsets;
//
//		//HACK: will be a component later
//		//meshC->animator->AnimationBoneTransform(this->time, meshC->animator->currentAnimation->name, vecFinalTransformation, meshC->meshInfo->skinnedModelInfo->vecBoneGlobals, vecOffsets);
//
//		//this->time += 0.01f;
//
//		unsigned int numberOfBonesUsed = static_cast<unsigned int>(vecFinalTransformation.size());
//
//		glUniform1i(this->_numBonesUsed_Uniloc, numberOfBonesUsed);
//
//		glm::mat4x4* pBoneMatrixArray = &(vecFinalTransformation[0]);
//
//		// UniformLoc_bonesArray is the getUniformLoc of "bones[0]" from
//		//	uniform mat4 bones[MAXNUMBEROFBONES] 
//		// in the shader
//		glUniformMatrix4fv(this->_bones_Uniloc, numberOfBonesUsed, GL_FALSE, (const GLfloat*)glm::value_ptr(*pBoneMatrixArray));
//
//		// Update the extents of the skinned mesh from the bones...
//		for (unsigned int boneIndex = 0; boneIndex != numberOfBonesUsed; boneIndex++)
//		{
//			glm::mat4 boneLocal = meshC->meshInfo->skinnedModelInfo->vecBoneGlobals[boneIndex];
//
//			// HACK: Need to add "uniform scale" to mesh
//	//		float scale = pCurrentMesh->nonUniformScale.x;
//			float scale = 1.0f;	// For now
//			boneLocal = glm::scale(boneLocal, glm::vec3(meshC->nonUniformScale.x, meshC->nonUniformScale.y, meshC->nonUniformScale.z));
//
//			glm::vec4 boneBallLocation = boneLocal * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
//			boneBallLocation *= scale;
//
//			//		if (boneIndex == 25)
//			//		{
//			//			//DrawDebugBall( glm::vec3(boneBallLocation), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.5f );
//			////			std::cout << "Bone 13, B_L_Finger31: " 
//			////				<< boneBallLocation.x << ", "
//			////				<< boneBallLocation.y << ", " 
//			////				<< boneBallLocation.z << std::endl;
//
//			//			cMeshSceneObject* pPlayerBody = findObjectByFriendlyName("PlayerBody");
//			//			pPlayerBody->setUniformScale(10.0f);
//			//			pPlayerBody->position = boneBallLocation;
//			//		}
//
//
//
//
//			// Update the extents of the mesh
//			//if (boneIndex == 0)
//			//{
//			//	// For the 0th bone, just assume this is the extent
//			//	meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
//			//	meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
//			//}
//			//else
//			//{	// It's NOT the 0th bone, so compare with current max and min
//			//	if (meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.x < boneBallLocation.x) { meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.x = boneBallLocation.x; }
//			//	if (meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.y < boneBallLocation.y) { meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.y = boneBallLocation.y; }
//			//	if (meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.z < boneBallLocation.z) { meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.z = boneBallLocation.z; }
//
//			//	if (meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.x > boneBallLocation.x) { meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.x = boneBallLocation.x; }
//			//	if (meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.y > boneBallLocation.y) { meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.y = boneBallLocation.y; }
//			//	if (meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.z > boneBallLocation.z) { meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.z = boneBallLocation.z; }
//			//}//if ( boneIndex == 0 )
//		}
//	}
//	else
//	{
//		glUniform1f(this->_bIsSkinnedMesh_Uniloc, (float)GL_FALSE);
//	}
//
//	// Get and draw from VAO
//	sModelDrawInfo modelInfo;
//	modelInfo.meshFileName = meshC->meshName;
//	if (this->_pVaoMeshManager->FindDrawInfoByName(modelInfo))
//	{
//		if (meshC->bUseVertexColour)
//		{
//			unsigned int vertexBufferSizeInBytes = sizeof(sModel_xyz_rgb_n_uv_bones) * modelInfo.numberOfVertices;
//
//			glUniform1f(this->_useVertexColour_UniLoc, (float)GL_TRUE);
//			glBindBuffer(GL_ARRAY_BUFFER, modelInfo.vertexBufferID);
//			glBufferData(GL_ARRAY_BUFFER, vertexBufferSizeInBytes, modelInfo.pVerticesToVBO, GL_STATIC_DRAW);
//		}
//		else
//		{
//			glUniform1f(this->_useVertexColour_UniLoc, false);
//		}
//
//		if (meshC->bIsSelected)
//		{
//			glUniform1f(this->_bIsSelected_UniLoc, (float)GL_TRUE);
//		}
//		else
//		{
//			glUniform1f(this->_bIsSelected_UniLoc, (float)GL_FALSE);
//		}
//
//		if (meshC->bIsAffectedByLights)
//		{
//			glUniform1f(this->_bIsLightAffecting_UniLoc, (float)GL_TRUE);
//		}
//		else
//		{
//			glUniform1f(this->_bIsLightAffecting_UniLoc, (float)GL_FALSE);
//		}
//
//		// Will remember what was saved of indices and vertexes
//		glBindVertexArray(modelInfo.vaoID);
//		glDrawElements(GL_TRIANGLES, modelInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
//		glBindVertexArray(0); // Clean for the next one
//
//		// Set opengl to fill spaces, showing the "faces"
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//		glEnable(GL_CULL_FACE);
//		glUniform1f(this->_bUseSkyBoxTexture_UniLoc, (float)GL_FALSE);
//	}
//}

void cDrawSystem::DrawCall(GLuint shaderProgramID, sEntity* entity, glm::vec3 position, float cameraDistance)
{
	// Check if mesh should be drawn in the first place
	sMeshComponent* meshC = entity->GetComponent<sMeshComponent>();
	if (meshC->bIsVisible == false)
	{
		return;
	}

	//glUniform1f(this->_renderPassNumber_UniLoc, this->_renderPassNum);	// Tell shader which pass it is

	// Bind textures
	this->_BindTextures(shaderProgramID, meshC);
	this->_BindHeightMapTextures(shaderProgramID, meshC);
	this->_BindAlphaMapTextures(shaderProgramID, meshC);

	// Set the shader to be used
	glUseProgram(shaderProgramID);

	// Create matrix, set current object matrix and send to the shader
	glm::mat4x4 matModel = glm::mat4(1.0f);
	glm::mat4 matModelInvTrans = ObjectMatrix(matModel, position, entity, meshC);
	glUniformMatrix4fv(this->_matModelTrans_UniLoc, 1, GL_FALSE, glm::value_ptr(matModel));
	glUniformMatrix4fv(this->_matModelInvTrans_UniLoc, 1, GL_FALSE, glm::value_ptr(matModelInvTrans));

	glUniform1f(this->_bNoLighting_UniLoc, (float)this->_bNoLighting);
	glUniform1f(this->_bNoTextures_UniLoc, (float)this->_bNoTextures);
	glUniform1f(this->_bNoSkybox_UniLoc, (float)this->_bNoSkybox);
	glUniform1f(this->_bUseBloom_UniLoc, (float)this->_bUseBloom);
	glUniform1f(this->_bUseCelShade_UniLoc, (float)this->_bUseCelShade);
	glUniform1f(this->_bUseNightVision_UniLoc, (float)this->_bUseNightVision);

	// Enable blend or "alpha" transparency
	if (meshC->materialDiffuse.a >= 1.0f && !meshC->bUseAlphaMap)
	{
		glDisable(GL_BLEND);
	}
	else
	{
		glEnable(GL_BLEND);
	}

	//glDisable( GL_BLEND );
	// Source == already on framebuffer
	// Dest == what you're about to draw
	// This is the "regular alpha blend transparency"
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUniform4f(this->_objectDiffuse_UniLoc,
		meshC->materialDiffuse.r,
		meshC->materialDiffuse.g,
		meshC->materialDiffuse.b,
		meshC->materialDiffuse.a);
	glUniform4f(this->_objectSpecular_UniLoc,
		meshC->materialSpecular.r,
		meshC->materialSpecular.g,
		meshC->materialSpecular.b,
		meshC->materialSpecular.a);



	glUniform1f(this->_reflectionRate_UniLoc, meshC->reflectionRate);
	glUniform1f(this->_refractionRate_UniLoc, meshC->refractionRate);

	// The FBO must always be drawn normally. Otherwisem it depends on the flags
	if (entity->GetTag() == "FBO" || entity->GetTag() == "Skybox" || (!meshC->bIsWireframe && !this->_bAllWireframe))
	{
		// Set opengl to fill spaces, showing the "faces"
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}
	else
	{
		// Set opengl to show objects as lines, or "wireframe"
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}

	// Check if this is a skinned mesh
	if (meshC->meshInfo->skinnedModelInfo != nullptr)
	{
		// It has a skinned mesh
		glUniform1f(this->_bIsSkinnedMesh_Uniloc, (float)GL_TRUE);


		// Also pass up the bone information...
		std::vector< glm::mat4x4 > vecFinalTransformation;	// Replaced by	theMesh.vecFinalTransformation
		std::vector< glm::mat4x4 > vecOffsets;

		meshC->animator->UpdateSystem();

		vecFinalTransformation = meshC->meshInfo->skinnedModelInfo->vecFinalTransformation;
		vecOffsets = meshC->meshInfo->skinnedModelInfo->vecOffsets;

		//HACK: will be a component later
		//meshC->animator->AnimationBoneTransform(this->time, meshC->animator->currentAnimation->name, vecFinalTransformation, meshC->meshInfo->skinnedModelInfo->vecBoneGlobals, vecOffsets);

		//this->time += 0.01f;

		unsigned int numberOfBonesUsed = static_cast<unsigned int>(vecFinalTransformation.size());

		glUniform1i(this->_numBonesUsed_Uniloc, numberOfBonesUsed);

		glm::mat4x4* pBoneMatrixArray = &(vecFinalTransformation[0]);

		// UniformLoc_bonesArray is the getUniformLoc of "bones[0]" from
		//	uniform mat4 bones[MAXNUMBEROFBONES] 
		// in the shader
		glUniformMatrix4fv(this->_bones_Uniloc, numberOfBonesUsed, GL_FALSE, (const GLfloat*)glm::value_ptr(*pBoneMatrixArray));

		// Update the extents of the skinned mesh from the bones...
		for (unsigned int boneIndex = 0; boneIndex != numberOfBonesUsed; boneIndex++)
		{
			glm::mat4 boneLocal = meshC->meshInfo->skinnedModelInfo->vecBoneGlobals[boneIndex];

			// HACK: Need to add "uniform scale" to mesh
	//		float scale = pCurrentMesh->nonUniformScale.x;
			float scale = 1.0f;	// For now
			boneLocal = glm::scale(boneLocal, glm::vec3(meshC->nonUniformScale.x, meshC->nonUniformScale.y, meshC->nonUniformScale.z));

			glm::vec4 boneBallLocation = boneLocal * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			boneBallLocation *= scale;

			//		if (boneIndex == 25)
			//		{
			//			//DrawDebugBall( glm::vec3(boneBallLocation), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.5f );
			////			std::cout << "Bone 13, B_L_Finger31: " 
			////				<< boneBallLocation.x << ", "
			////				<< boneBallLocation.y << ", " 
			////				<< boneBallLocation.z << std::endl;

			//			cMeshSceneObject* pPlayerBody = findObjectByFriendlyName("PlayerBody");
			//			pPlayerBody->setUniformScale(10.0f);
			//			pPlayerBody->position = boneBallLocation;
			//		}




			// Update the extents of the mesh
			if (boneIndex == 0)
			{
				// For the 0th bone, just assume this is the extent
				meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
				meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
			}
			else
			{	// It's NOT the 0th bone, so compare with current max and min
				if (meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.x < boneBallLocation.x) { meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.x = boneBallLocation.x; }
				if (meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.y < boneBallLocation.y) { meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.y = boneBallLocation.y; }
				if (meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.z < boneBallLocation.z) { meshC->meshInfo->skinnedModelInfo->minXYZ_from_SM_Bones.z = boneBallLocation.z; }

				if (meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.x > boneBallLocation.x) { meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.x = boneBallLocation.x; }
				if (meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.y > boneBallLocation.y) { meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.y = boneBallLocation.y; }
				if (meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.z > boneBallLocation.z) { meshC->meshInfo->skinnedModelInfo->maxXYZ_from_SM_Bones.z = boneBallLocation.z; }
			}//if ( boneIndex == 0 )
		}
	}
	else
	{
		glUniform1f(this->_bIsSkinnedMesh_Uniloc, (float)GL_FALSE);
	}

	// Get and draw from VAO
	sModelDrawInfo* modelInfo = nullptr;
	std::string meshName = "";
	if (cameraDistance > 2000.0f)
	{
		meshName = "lowres_";
	}
	meshName.append(meshC->meshName);

	bool result = this->_pVaoMeshManager->FindDrawInfoByName(meshName, &modelInfo);
	if (!result)
	{
		modelInfo->meshFileName = meshC->meshName;
		result = this->_pVaoMeshManager->FindDrawInfoByName(meshC->meshName, &modelInfo);

		if (!result)
		{
			return;
		}
	}

	if (meshC->bUseVertexColour)
	{
		unsigned int vertexBufferSizeInBytes = sizeof(sModel_xyz_rgb_n_uv_bones) * modelInfo->numberOfVertices;

		glUniform1f(this->_useVertexColour_UniLoc, (float)GL_TRUE);
		glBindBuffer(GL_ARRAY_BUFFER, modelInfo->vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, vertexBufferSizeInBytes, modelInfo->pVerticesToVBO, GL_STATIC_DRAW);
	}
	else
	{
		glUniform1f(this->_useVertexColour_UniLoc, false);
	}

	if (meshC->bIsSelected)
	{
		glUniform1f(this->_bIsSelected_UniLoc, (float)GL_TRUE);
	}
	else
	{
		glUniform1f(this->_bIsSelected_UniLoc, (float)GL_FALSE);
	}

	if (meshC->bIsAffectedByLights)
	{
		glUniform1f(this->_bIsLightAffecting_UniLoc, (float)GL_TRUE);
	}
	else
	{
		glUniform1f(this->_bIsLightAffecting_UniLoc, (float)GL_FALSE);
	}

	// Will remember what was saved of indices and vertexes
	glBindVertexArray(modelInfo->vaoID);
	glDrawElements(GL_TRIANGLES, modelInfo->numberOfIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0); // Clean for the next one

	// Set opengl to fill spaces, showing the "faces"
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glUniform1f(this->_bUseSkyBoxTexture_UniLoc, (float)GL_FALSE);
}

void cDrawSystem::DrawColliderAt(const std::string& colName, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, GLuint shaderProgramID)
{
	cPoolManager* tempPool = cPoolManager::GetInstance();
	sEntity* collider = tempPool->GetEntityByName(colName, false);
	sMeshComponent* colliderMesh = collider->GetComponent<sMeshComponent>();

	collider->SetIsActive(true);
	collider->transform->SetWorldPosition(position);
	collider->transform->SetWorldRotation(rotation);
	colliderMesh->nonUniformScale = scale;
	colliderMesh->materialDiffuse = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	colliderMesh->bIsVisible = true;
	this->DrawCall(shaderProgramID, collider, collider->transform->GetWorldPosition(), 0.0f);
	colliderMesh->nonUniformScale = glm::vec3(1.0f);
	colliderMesh->materialDiffuse = glm::vec4(1.0f);
	collider->transform->SetWorldRotation(glm::quat());
	colliderMesh->bIsVisible = false;
	collider->SetIsActive(false);
}

void cDrawSystem::DrawCall1Pass(const glm::vec3& renderPos, const glm::vec3& renderLookAt, GLFWwindow* window, GLuint shaderProgramID, double deltaTime)
{
	// *****************************************
	// 1st pass:
	//
	// Draw the scene frame to an offscreen tex
	// *****************************************

	// Set frame buffer render to the main buffer (0)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	float ratio;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	// Set to first pass
	this->_renderPassNum = 1.0f;

	glUseProgram(shaderProgramID);

	// Check if window has been resized.
	// If it has, the FBO needs to be remade, so the texture
	// has the correct memory size and pixel size
	if (width != this->_curWidth || height != this->_curHeight)
	{
		this->_curWidth = width;
		this->_curHeight = height;

		std::string error;
		this->_pFBO->ShutdownFBO();
		this->_pFBO->InitFBO(width, height, error);
	}

	// Set frame buffer render to the FBO buffer and clear it
	glBindFramebuffer(GL_FRAMEBUFFER, this->_pFBO->_ID);

	this->_pFBO->ClearBuffers(true, true);

	// Set shader render pass to the first pass
	glUniform1f(this->_renderPassNumber_UniLoc, 1.0f);

	// Set proportions for the texture
	ratio = width / (float)height;
	glViewport(0, 0, width, height);

	// Create mat view and projection
	glm::mat4x4 matProjection = glm::perspective(0.6f, ratio, 0.1f, 100000.0f); // FOV, aspect ratio, near and far clipping planes
	glm::mat4x4	matView = glm::mat4(1.0f);

	// Set draw params
	glEnable(GL_DEPTH);		// Enables the KEEPING of the depth information
	glEnable(GL_DEPTH_TEST);	// When drawing, checked the existing depth
	glEnable(GL_CULL_FACE);	// Discared "back facing" triangles

	// Get shader variables to set the MVP (mat view projection, though it happens on the inverse PVM)
	this->_eye_UniLoc = glGetUniformLocation(shaderProgramID, "eyeLocation");
	this->_matView_UniLoc = glGetUniformLocation(shaderProgramID, "matView");
	this->_matProj_UniLoc = glGetUniformLocation(shaderProgramID, "matProj");

	// Set camera position
	//sEntity* camera = cPoolManager::GetInstance()->GetEntityByName("WorldCamera");
	//sCameraComponent* cameraC = camera->GetTComponent<sCameraComponent*>(CAMERA_COMPONENT);

	// Set shader MVP
	glUniform3f(this->_eye_UniLoc, renderPos.x, renderPos.y, renderPos.z);
	matView = glm::lookAt(renderPos, renderLookAt, glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(this->_matView_UniLoc, 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(this->_matProj_UniLoc, 1, GL_FALSE, glm::value_ptr(matProjection));

	this->DrawObjects(shaderProgramID);

	if (cIOManager::GetEditorState() != cIOManager::Free)
	{
		this->DrawWireframeLightSpheres(shaderProgramID);
	}

	this->_pDebugRenderer->RenderDebugObjects(matView, matProjection, deltaTime);
	//this->DrawAABBs(this->_pAABBManager, shaderProgramID);

	//this->_pDebugRenderer->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1000.0f));
	//// Set the shader back
	//glUseProgram(shaderProgramID);
}

void cDrawSystem::DrawCall2Pass(const std::string& renderObjName, bool isScreenQuad, GLFWwindow* window, GLuint shaderProgramID)
{
	// *****************************************
	// 2nd pass:
	//
	// Now the entire scene has been drawn,
	// so draw the object the texture is
	// being applied on.
	// *****************************************

	// Set to second pass
	this->_renderPassNum = 2.0f;

	glUseProgram(shaderProgramID);

	// Set shader render pass to the second pass
	glUniform1f(this->_renderPassNumber_UniLoc, 2.0f);

	// Set frame buffer render to the main buffer (0)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Save aspect
	float ratio;
	int width, height;

	// Get the size of the actual (screen) frame buffer
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;
	glViewport(0, 0, width, height);
	

	// Set draw params
	glEnable(GL_DEPTH);		// Enables the KEEPING of the depth information
	glEnable(GL_DEPTH_TEST);	// When drawing, checked the existing depth
	glEnable(GL_CULL_FACE);	// Discared "back facing" triangles

	// Clear the main frame buffer 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind the FBO drawn texture
	sEntity* renderObj = cPoolManager::GetInstance()->GetEntityByName(renderObjName);
	sMeshComponent* renderMesh = renderObj->GetComponent<sMeshComponent>();
	renderObj->SetTag("FBO");
	renderObj->SetIsActive(true);
	renderMesh->bIsVisible = true;
	renderMesh->bIsAffectedByLights = false;

	// Create mat view and projection
	glm::mat4x4 matProjection = glm::perspective(0.6f, ratio, 0.1f, 100000.0f); // FOV, aspect ratio, near and far clipping planes
	glm::mat4x4	matView = glm::mat4(1.0f);

	// Get shader variables to set the MVP (mat view projection, though it happens on the inverse PVM)
	this->_eye_UniLoc = glGetUniformLocation(shaderProgramID, "eyeLocation");
	this->_matView_UniLoc = glGetUniformLocation(shaderProgramID, "matView");
	this->_matProj_UniLoc = glGetUniformLocation(shaderProgramID, "matProj");

	sEntity* camera = cPoolManager::GetInstance()->GetEntityByName("WorldCamera");
	sCameraComponent* cameraC = camera->GetComponent<sCameraComponent>();

	if (isScreenQuad)
	{
		renderMesh->nonUniformScale = glm::vec3(ratio, 1.0f, 1.0f);

		// Set the view transform so that the camera movement isn't impacted 
		glm::vec3 cameraFullScreenQuad = glm::vec3(0.0, 0.0, -3.0f);

		// Set shader MVP
		glUniform3f(this->_eye_UniLoc, cameraFullScreenQuad.x, cameraFullScreenQuad.y, cameraFullScreenQuad.z);
		matView = glm::lookAt(cameraFullScreenQuad, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else
	{
		// Let camera decide on it's own
		glUniform3f(this->_eye_UniLoc, cameraC->eyePosition.x, cameraC->eyePosition.y, cameraC->eyePosition.z);
		matView = glm::lookAt(cameraC->eyePosition, cameraC->GetLookAtInWorldSpace(), cameraC->GetUpVector());
	}

	glUniformMatrix4fv(this->_matView_UniLoc, 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(this->_matProj_UniLoc, 1, GL_FALSE, glm::value_ptr(matProjection));

	// Draw the render object
	this->DrawCall(shaderProgramID, renderObj, renderObj->transform->GetWorldPosition(), 0.0f);
	
	
	if (isScreenQuad)
	{
		//renderMesh->bIsVisible = false;
	}
}

void cDrawSystem::DrawWireframeLightSpheres(GLuint shaderProgramID)
{
	cPoolManager* tempPool = cPoolManager::GetInstance();
	if (tempPool->vecLightComponents.size() == 0)
	{
		return;
	}

	sEntity* wireframeEntity = tempPool->GetEntityByName("WireframeSphere");

	sMeshComponent* wireMeshC = wireframeEntity->GetComponent<sMeshComponent>();
	sLocationComponent* wireLocationC = wireframeEntity->GetComponent<sLocationComponent>();
	for (int i = 0; i != tempPool->vecLightComponents.size(); i++)
	{
		sLightComponent* lightComponent = static_cast<sLightComponent*>(tempPool->vecLightComponents[i]);
		if (lightComponent->bisWireframeOn == false)
		{
			continue;
		}

		glm::vec4 oldColour = wireMeshC->materialDiffuse;
		glm::vec3 oldScale = wireMeshC->nonUniformScale;
		wireMeshC->bIsVisible = true;
		wireMeshC->bIsAffectedByLights = false;
		wireMeshC->bIsWireframe = true;
		wireMeshC->materialDiffuse = glm::vec4(255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f, 1.0f);
		wireLocationC->SetWorldPosition(glm::vec3(lightComponent->position));

		const float ACCURACY_OF_DISTANCE = 0.001f;
		const float INFINITE_DISTANCE = 10000.0f;

		float distance90Percent = this->_pLightHelper->calcApproxDistFromAtten(0.65f, ACCURACY_OF_DISTANCE, INFINITE_DISTANCE,
			lightComponent->attenuation.x,
			lightComponent->attenuation.y,
			lightComponent->attenuation.z);

		wireMeshC->nonUniformScale = glm::vec3(distance90Percent);		// 90% brightness
		wireMeshC->materialDiffuse = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		DrawCall(shaderProgramID, wireframeEntity, wireframeEntity->transform->GetWorldPosition(), 0.0f);

		float distance50Percent =
			_pLightHelper->calcApproxDistFromAtten(0.30f, ACCURACY_OF_DISTANCE,
				INFINITE_DISTANCE,
				lightComponent->attenuation.x,
				lightComponent->attenuation.y,
				lightComponent->attenuation.z);
		wireMeshC->nonUniformScale = glm::vec3(distance50Percent);      // 50% brightness
		wireMeshC->materialDiffuse = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		DrawCall(shaderProgramID, wireframeEntity, wireframeEntity->transform->GetWorldPosition(), 0.0f);

		float distance25Percent =
			_pLightHelper->calcApproxDistFromAtten(0.10f, ACCURACY_OF_DISTANCE,
				INFINITE_DISTANCE,
				lightComponent->attenuation.x,
				lightComponent->attenuation.y,
				lightComponent->attenuation.z);
		wireMeshC->nonUniformScale = glm::vec3(distance25Percent);      // 25% brightness
		wireMeshC->materialDiffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		DrawCall(shaderProgramID, wireframeEntity, wireframeEntity->transform->GetWorldPosition(), 0.0f);

		// Reset
		wireMeshC->nonUniformScale = oldScale;
		wireMeshC->materialDiffuse = oldColour;
		wireMeshC->bIsVisible = false;
	}
}

void cDrawSystem::DrawAABBs(cAABBManager* pAABBManager, GLuint shaderProgramID)
{
	for (std::map<unsigned long long, cAABB*>::iterator mapIter = pAABBManager->_pMapAABB.begin(); mapIter != pAABBManager->_pMapAABB.end(); mapIter++)
	{
		mapIter->second->bShouldDraw = true;

		cPoolManager* tempPool = cPoolManager::GetInstance();
		sEntity* AABBEntity = tempPool->GetEntityByName("AABB", false);

		sMeshComponent* AABBMeshC = AABBEntity->GetComponent<sMeshComponent>();
		sLocationComponent* AABBLocationC = AABBEntity->GetComponent<sLocationComponent>();

		AABBEntity->SetIsActive(true);
		AABBMeshC->meshName = mapIter->second->GetMeshName();
		AABBLocationC->SetWorldPosition(mapIter->second->GetCenterXYZ());
		AABBMeshC->nonUniformScale = glm::vec3(mapIter->second->GetSideLength());
		AABBMeshC->bIsWireframe = true;
		AABBMeshC->bIsVisible = true;

		DrawCall(shaderProgramID, AABBEntity, AABBEntity->transform->GetWorldPosition(), 0.0f);

		AABBMeshC->bIsVisible = false;
		AABBEntity->SetIsActive(false);

		/*if (mapIter->second->bShouldDraw || this->_bDrawBoundingBoxes)
		{
			mapIter->second->bShouldDraw = true;

			cPoolManager* tempPool = cPoolManager::GetInstance();
			sEntity* AABBEntity = tempPool->GetEntityByName("AABB", false);

			sMeshComponent* AABBMeshC = static_cast<sMeshComponent*>(AABBEntity->GetComponent(eComponentTypes::MESH_COMPONENT));
			sLocationComponent* AABBLocationC = static_cast<sLocationComponent*>(AABBEntity->GetComponent(eComponentTypes::LOCATION_COMPONENT));

			AABBMeshC->meshName = mapIter->second->GetMeshName();
			AABBLocationC->position = mapIter->second->GetCenterXYZ();
			AABBMeshC->nonUniformScale = glm::vec3(mapIter->second->GetSideLength());
			AABBMeshC->bIsWireframe = true;
			AABBMeshC->bIsVisible = true;
			
			DrawCall(shaderProgramID, AABBMeshC, AABBLocationC);

			AABBMeshC->bIsVisible = false;
		}*/
	}
}

void cDrawSystem::DrawCollider(sEntity* entity, GLuint shaderProgramID)
{
	sPointsColliderComponent* pointsColliderC = entity->GetComponent<sPointsColliderComponent>();
	sLocationComponent* locationC = entity->GetComponent<sLocationComponent>();
	sMeshComponent* meshC = entity->GetComponent<sMeshComponent>();
	if (pointsColliderC != nullptr)
	{
		std::vector<glm::vec3> colliderPoints = pointsColliderC->GetModelColliderPoints();
		for (unsigned int i = 0; i != colliderPoints.size(); i++)
		{
			cPoolManager* tempPool = cPoolManager::GetInstance();
			sEntity* debugEntity = tempPool->GetEntityByName("DebugSphere");

			sMeshComponent* debugMeshComponent = debugEntity->GetComponent<sMeshComponent>();
			sLocationComponent* debugLocationComponent = debugEntity->GetComponent<sLocationComponent>();

			glm::vec4 worldTransform = glm::vec4(0.0f);
			glm::mat4 mat = glm::mat4x4(1.0f);
			worldTransform = ObjectMatrix(mat, locationC, meshC) * glm::vec4(colliderPoints[i], 1.0f);
			pointsColliderC->GetFinalColliderPoints()[i] = worldTransform;
			debugLocationComponent->SetWorldPosition(worldTransform);
			meshC->nonUniformScale = glm::vec3(0.3f);
			debugMeshComponent->materialDiffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			debugMeshComponent->bIsVisible = true;

			DrawCall(shaderProgramID, debugEntity, debugEntity->transform->GetWorldPosition(), 0.0f);

			debugLocationComponent->SetWorldPosition(glm::vec3(0.0f));
			meshC->nonUniformScale = glm::vec3(1.0f);
			debugMeshComponent->materialDiffuse = glm::vec4(1.0f);
			debugMeshComponent->bIsVisible = false;
		}
	}
}

void cDrawSystem::ProcessKeys(GLFWwindow* window, cIOManager* pIOManager)
{
	if (cIOManager::GetEditorState() == cIOManager::Object)
	{
		cPoolManager* tempPool = cPoolManager::GetInstance();
		if (glfwGetKey(window, GLFW_KEY_TAB) && !this->_bIsChanging)
		{
			sMeshComponent* meshCold = tempPool->vecStaticEntities[this->_currentSelectedIndex]->GetComponent<sMeshComponent>();
			sMeshComponent* meshC = nullptr;

			this->_bIsChanging = true;

			sEntity* entity = tempPool->vecStaticEntities[this->_currentSelectedIndex];
			while (meshC == nullptr || !meshC->bIsVisible || !entity->GetIsActive())
			{
				this->_currentSelectedIndex++;
				if (this->_currentSelectedIndex >= tempPool->vecStaticEntities.size())
				{
					this->_currentSelectedIndex = 5;
				}
				

				meshC = tempPool->vecStaticEntities[this->_currentSelectedIndex]->GetComponent<sMeshComponent>();
			}
			
			if (meshCold != nullptr)
			{
				meshCold->bIsSelected = false;
			}
			
			meshC->bIsSelected = true;
		}
		else if (!glfwGetKey(window, GLFW_KEY_TAB) && this->_bIsChanging)
		{
			this->_bIsChanging = false;
		}

		if (pIOManager->IsMouseLeftButtonPressed(window))
		{
			return;
		}

		sEntity* entityControlled = tempPool->vecStaticEntities[this->_currentSelectedIndex];
		sMeshComponent* meshC = entityControlled->GetComponent<sMeshComponent>();
		
		if (glfwGetKey(window, GLFW_KEY_W))
		{
			entityControlled->transform->SetWorldPosition(entityControlled->transform->GetWorldPosition() + entityControlled->transform->GetForwardVector() * 5.0f);
		}
		else if (glfwGetKey(window, GLFW_KEY_A))
		{
			entityControlled->transform->SetWorldPosition(entityControlled->transform->GetWorldPosition() + entityControlled->transform->GetRightVector() * 5.0f);
		}
		else if (glfwGetKey(window, GLFW_KEY_S))
		{
			entityControlled->transform->SetWorldPosition(entityControlled->transform->GetWorldPosition() + entityControlled->transform->GetForwardVector() * -5.0f);
		}
		else if (glfwGetKey(window, GLFW_KEY_D))
		{
			entityControlled->transform->SetWorldPosition(entityControlled->transform->GetWorldPosition() + entityControlled->transform->GetRightVector() * -5.0f);
		}
		else if (glfwGetKey(window, GLFW_KEY_Q))
		{
			entityControlled->transform->SetWorldPosition(entityControlled->transform->GetWorldPosition() + entityControlled->transform->GetUpVector() * 5.0f);
		}
		else if (glfwGetKey(window, GLFW_KEY_E))
		{
			entityControlled->transform->SetWorldPosition(entityControlled->transform->GetWorldPosition() + entityControlled->transform->GetUpVector() * -5.0f);
		}

		if (glfwGetKey(window, GLFW_KEY_I))
		{
			glm::quat newRot = nQuaternionHelper::AdjOrientationEulerAngles(entityControlled->transform->GetWorldRotation(), glm::vec3(0.0f, 0.0f, 5.0f), true);
			entityControlled->transform->SetWorldRotation(newRot);
		}
		else if (glfwGetKey(window, GLFW_KEY_J))
		{
			glm::quat newRot = nQuaternionHelper::AdjOrientationEulerAngles(entityControlled->transform->GetWorldRotation(), glm::vec3(0.0f, -5.0f, 0.0f), true);
			entityControlled->transform->SetWorldRotation(newRot);
		}
		else if (glfwGetKey(window, GLFW_KEY_K))
		{
			glm::quat newRot = nQuaternionHelper::AdjOrientationEulerAngles(entityControlled->transform->GetWorldRotation(), glm::vec3(0.0f, 0.0f, -5.0f), true);
			entityControlled->transform->SetWorldRotation(newRot);
		}
		else if (glfwGetKey(window, GLFW_KEY_L))
		{
			glm::quat newRot = nQuaternionHelper::AdjOrientationEulerAngles(entityControlled->transform->GetWorldRotation(), glm::vec3(0.0f, 5.0f, 0.0f), true);
			entityControlled->transform->SetWorldRotation(newRot);
		}
		else if (glfwGetKey(window, GLFW_KEY_U))
		{
			glm::quat newRot = nQuaternionHelper::AdjOrientationEulerAngles(entityControlled->transform->GetWorldRotation(), glm::vec3(5.0f, 0.0f, 0.0f), true);
			entityControlled->transform->SetWorldRotation(newRot);
		}
		else if (glfwGetKey(window, GLFW_KEY_O))
		{
			glm::quat newRot = nQuaternionHelper::AdjOrientationEulerAngles(entityControlled->transform->GetWorldRotation(), glm::vec3(-5.0f, 0.0f, 0.0f), true);
			entityControlled->transform->SetWorldRotation(newRot);
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET))
		{
			float curScale = meshC->nonUniformScale.x;
			curScale -= 1.0f;
			meshC->nonUniformScale = glm::vec3(curScale);
		}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET))
		{
			float curScale = meshC->nonUniformScale.x;
			curScale += 1.0f;
			meshC->nonUniformScale = glm::vec3(curScale);
		}

		nPhysics::iRigidbody* rb = entityControlled->GetComponent<nPhysics::iRigidbody>();
		if (rb != nullptr)
		{
			rb->SetPosition(entityControlled->transform->GetWorldPosition());
			rb->SetRotation(entityControlled->transform->GetWorldRotation());
		}
	}
	else if (cIOManager::GetEditorState() == cIOManager::Shader)
	{
		if (glfwGetKey(window, GLFW_KEY_1))
		{
			this->_bNoLighting = true;
			this->_bNoTextures = false;
			this->_bNoSkybox = false;
			this->_bAllWireframe = false;
			this->_bDrawBoundingBoxes = false;
		}
		else if(glfwGetKey(window, GLFW_KEY_2))
		{
			this->_bNoLighting = false;
			this->_bNoTextures = true;
			this->_bNoSkybox = false;
			this->_bAllWireframe = false;
			this->_bDrawBoundingBoxes = false;
		}
		else if (glfwGetKey(window, GLFW_KEY_3))
		{
			this->_bNoLighting = false;
			this->_bNoTextures = false;
			this->_bNoSkybox = true;
			this->_bAllWireframe = false;
			this->_bDrawBoundingBoxes = false;
		}
		else if (glfwGetKey(window, GLFW_KEY_4))
		{
			this->_bNoLighting = true;
			this->_bNoTextures = true;
			this->_bNoSkybox = true;
			this->_bAllWireframe = false;
			this->_bDrawBoundingBoxes = false;
			this->_bUseBloom = false;
			this->_bUseCelShade = false;
		}
		else if (glfwGetKey(window, GLFW_KEY_5))
		{
			this->_bNoLighting = false;
			this->_bNoTextures = false;
			this->_bNoSkybox = false;
			this->_bAllWireframe = true;
			this->_bDrawBoundingBoxes = false;
		}
		else if (glfwGetKey(window, GLFW_KEY_6))
		{
			this->_bNoLighting = false;
			this->_bNoTextures = false;
			this->_bNoSkybox = false;
			this->_bAllWireframe = false;
			this->_bDrawBoundingBoxes = false;
			this->_bUseBloom = false;
			this->_bUseCelShade = false;
			this->_bUseNightVision = false;
		}
		else if (glfwGetKey(window, GLFW_KEY_7))
		{
			this->_bNoLighting = false;
			this->_bNoTextures = false;
			this->_bNoSkybox = false;
			this->_bAllWireframe = false;
			this->_bDrawBoundingBoxes = true;
			this->_bUseBloom = true;
			this->_bUseCelShade = false;
			this->_bUseNightVision = false;
		}
		else if (glfwGetKey(window, GLFW_KEY_8))
		{
			this->_bNoLighting = false;
			this->_bNoTextures = false;
			this->_bNoSkybox = false;
			this->_bAllWireframe = false;
			this->_bDrawBoundingBoxes = true;
			this->_bUseBloom = false;
			this->_bUseCelShade = true;
			this->_bUseNightVision = false;
		}
		else if (glfwGetKey(window, GLFW_KEY_9))
		{
			this->_bNoLighting = false;
			this->_bNoTextures = false;
			this->_bNoSkybox = false;
			this->_bAllWireframe = false;
			this->_bDrawBoundingBoxes = false;
			this->_bUseBloom = true;
			this->_bUseCelShade = true;
			this->_bUseNightVision = false;
		}
		else if (glfwGetKey(window, GLFW_KEY_0))
		{
			this->_bNoLighting = false;
			this->_bNoTextures = false;
			this->_bNoSkybox = false;
			this->_bAllWireframe = false;
			this->_bDrawBoundingBoxes = false;
			this->_bUseBloom = false;
			this->_bUseCelShade = false;
			this->_bUseNightVision = true;
		}
	}
}

void cDrawSystem::UpdateSystem(double deltaTime)
{
}

bool cDrawSystem::ScheduleEntity(sEntity* entityToSchedule, iComponent* mesh)
{
	sDrawEntity* newEntity = new sDrawEntity(entityToSchedule, static_cast<sMeshComponent*>(mesh));
	//this->_vecDrawEntities.push_back(newEntity);

	return false;
}

bool cDrawSystem::UnscheduleEntity(sEntity * entityToSchedule, iComponent * mesh)
{
	return false;
}
