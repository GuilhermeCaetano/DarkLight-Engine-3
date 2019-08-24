#ifndef _cVaoMeshManager_HG_
#define _cVaoMeshManager_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Mesh\sModelDrawInfo.h>
#include <string>
#include <map>

class cVaoMeshManager
{
public:
	cVaoMeshManager();
	~cVaoMeshManager();

	bool SetBasePath(const std::string& pathName); // Path for the model files to be found. Returns false if path doesn't exists
	bool LoadThreadModelIntoCPU(sModelDrawInfo& meshInfo);
	bool LoadThreadModelIntoGPU(sModelDrawInfo& meshInfo, GLuint shaderProgramID);
	bool LoadModelIntoVAO(sModelDrawInfo& sModelDrawInfo, GLuint shaderProgramID); // Loads a model into the VAO to be used and drawn later
	bool FindDrawInfoByName(const std::string& meshName, sModelDrawInfo** modelInfo); // Get a model information using it's name
	void UpdateModelVertexInformation(sModelDrawInfo& sModelDrawinfo); // Update the model vertex

	bool UnloadModelFromVAO(sModelDrawInfo* modelInfo);

	std::string GetLastError(bool bClear); // Get the last error that happened
	void Shutdown(); // exit

private:
	std::string _basePath;
	std::string _lastError;
	std::map<std::string /*Model name*/, sModelDrawInfo* /*model info*/> _mapModelInfoByName;

	bool _LoadModelFromFile(sModelDrawInfo& sModelDrawInfo); // Load model vertices and triangle information from a file
	bool _LoadDrawInfoIntoVAO(sModelDrawInfo& sModelDrawInfo, GLuint shaderProgramID); // Load the information of the model into the VAO
	void _AppendTextToLastError(std::string text, bool addNewLineBefore);
};


#endif // !_cVaoMeshManager_HG_

