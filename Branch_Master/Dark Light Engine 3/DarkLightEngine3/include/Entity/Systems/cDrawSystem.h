#ifndef _cDrawSystem_HG_
#define _cDrawSystem_HG_

#include "iSystem.h"
#include <OpenGL\OpenGLHeaders.h>
#include <Entity\sEntity.h>
#include <Entity\Components\iComponent.h>
#include <Mesh\cVaoMeshManager.h>
#include <Helpers\cLightHelper.h>
#include <Texture\cTextureManager.h>
#include <FBO\cFBO.h>


struct sMeshComponent;
class cAABBManager;
class cDebugRenderer;
class cIOManager;
class cDrawSystem : public iSystem
{
public:
	cDrawSystem();
	~cDrawSystem();

	void SetupSystem(GLuint shaderProgramID, cVaoMeshManager* pVaoMeshManager, cTextureManager* pTextureManager/*, cAABBManager* pAABBManager*/);
	void SetDebugRenderer(cDebugRenderer* pDebugRenderer);

	glm::mat4 GetCameraProjection();
	void SetCameraProjection(float fov, float ratio, float nearClip, float farClip);

	void SetupFBO();
	void DeleteFBO();

	void SetFBO(cFBO* pFBO);

	void DrawObjects(GLuint shaderProgramID);
	void DrawOneObject(sEntity* entity, GLuint shaderProgramID);
	//void DrawCall(GLuint shaderProgramID, sEntity* entity, glm::vec3 position, float cameraDistance);
	void DrawCall(GLuint shaderProgramID, const std::pair<sEntity*, sMeshComponent*> meshPair, float cameraDistance);

	void DrawColliderAt(const std::string& colName, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, GLuint shaderProgramID);
	void DrawCall1PassOld(const glm::vec3& renderPos, const glm::vec3& renderLookAt, GLFWwindow* window, GLuint shaderProgramID, double deltaTime);
	void DrawCall1Pass(const glm::vec3& renderPos, const glm::vec3& renderLookAt, int width, int height, GLuint shaderProgramID, double deltaTime);
	void DrawCall2PassOld(const std::string& renderObjName, bool isScreenQuad, GLFWwindow* window, GLuint shaderProgramID);
	void DrawCall2Pass(sEntity* FBOEntity, int width, int height, GLuint shaderProgramID);

	void DrawWireframeLightSpheres(GLuint shaderProgramID);
	void DrawAABBs(cAABBManager* pAABBManager, GLuint shaderProgramID);
	void DrawCollider(sEntity* entity, GLuint shaderProgramID);

	void ProcessKeys(GLFWwindow* window, cIOManager* pIOManager);

	// Updates the system
	virtual void UpdateSystem(double deltaTime);

	// Schedules and unschedule specific components
	// in order to save searching memory
	virtual bool ScheduleEntity(sEntity* entity, iComponent* component);
	virtual bool UnscheduleEntity(sEntity* entity, iComponent* component);

protected:
	virtual void _Cleanup();

private:
	struct sDrawEntity
	{
		sDrawEntity(sEntity* entity, sMeshComponent* meshC)
		{
			this->_entity = entity;
			this->_meshC = meshC;
		}

		sEntity* _entity;
		sMeshComponent* _meshC;
	};

	void _SetUniformLocations(GLuint shaderProgramID);
	void _BindTextures(GLuint shaderProgramID, iComponent* iMeshC);
	void _BindSkyboxTexture(GLuint shaderProgram);
	void _BindHeightMapTextures(GLuint shaderProgram, iComponent* iMeshC);
	void _BindAlphaMapTextures(GLuint shaderProgram, iComponent* iMeshC);

	cVaoMeshManager* _pVaoMeshManager;
	cTextureManager* _pTextureManager;
	cAABBManager* _pAABBManager;
	cLightHelper* _pLightHelper;
	cDebugRenderer* _pDebugRenderer;

	cFBO* _pFBO;
	cFBO* _pFBO2;
	int _curWidth;
	int _curHeight;

	glm::mat4x4 _cameraMatProjection;

	////////// UNIFORM LOCATIONS /////////////
	// FBO
	float _renderPassNum;
	GLint _renderPassNumber_UniLoc;
	GLint _texPass1OutputTexture_UniLoc;

	// MVP
	GLint _eye_UniLoc;
	GLint _matView_UniLoc;
	GLint _matProj_UniLoc;

	// Mesh
	GLint _matModelTrans_UniLoc;
	GLint _matModelInvTrans_UniLoc;
	GLint _useVertexColour_UniLoc;
	GLint _objectDiffuse_UniLoc;
	GLint _objectSpecular_UniLoc;
	GLint _reflectionRate_UniLoc;
	GLint _refractionRate_UniLoc;
	GLint _bIsLightAffecting_UniLoc;
	GLint _bIsSelected_UniLoc;

	// Skinned Mesh
	GLint _bIsSkinnedMesh_Uniloc;
	GLint _numBonesUsed_Uniloc;
	GLint _bones_Uniloc;

	// Texture
	GLint _tex_Uniloc[8];
	GLint _texBW_UniLoc[2];

	// Height Map
	GLint _heightTex_UniLoc;
	GLint _heightMapRatio_UniLoc;
	GLint _bUseHeightMap_UniLoc;
	GLint _bFlattenHeightMap_UniLoc;

	// Alpha Map
	GLint _alphaTex_UniLoc;
	GLint _blackThreshold_UniLoc;
	GLint _alphaOverride_UniLoc;
	GLint _bUseAlphaMap_UniLoc;

	// Skybox
	GLint _skyBoxDayCubeMap_UniLoc;
	GLint _skyBoxNightCubeMap_UniLoc;
	GLint _dayMix_UniLoc;
	GLint _bUseSkyBoxTexture_UniLoc;

	// Shader Aspects
	GLint _bNoLighting_UniLoc;
	GLint _bNoTextures_UniLoc;
	GLint _bNoSkybox_UniLoc;
	GLint _bUseBloom_UniLoc;
	GLint _bUseCelShade_UniLoc;
	GLint _bUseNightVision_UniLoc;
	/////////////////////////////////////////

	int _currentSelectedIndex;
	bool _bIsChanging;

	bool _bTextureUniformLocationsLoaded;
	bool _bNoLighting;
	bool _bNoTextures;
	bool _bNoSkybox;
	bool _bAllWireframe;
	bool _bDrawBoundingBoxes;

	bool _bUseBloom;
	bool _bUseCelShade;
	bool _bUseNightVision;

	glm::vec4 waterOffset = glm::vec4(0.0f);

	float _dayMix;

	float time;

	bool _bIsFBO;
};


#endif // !_cDrawSystem_HG_

