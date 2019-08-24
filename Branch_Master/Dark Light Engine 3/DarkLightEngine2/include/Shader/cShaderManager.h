#ifndef _cShaderManager_HG_
#define _cShaderManager_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <string>
#include <vector>
#include <map>


class cShaderManager
{
public:
	class cShader
	{
	public:
		cShader();
		~cShader();

		enum eShaderTypes
		{
			VERTEX_SHADER, // Basic shader used for mesh vertex computations
			FRAGMENT_SHADER, // Basic shader used for mesh related computations
			GEOMETRY_SHADER,
			TESSELATION_SHADER,
			COMPUTATION_SHADER,
			UNKNOWN // Shouldn't stay here, if so, we have an error
		};
		eShaderTypes shaderType;
		std::string shaderFileName;
		unsigned int shaderID;
		std::vector<std::string> vecShaderCodeLines;

		std::string GetShaderType(); // Return the current shader being used in string format
	};

	class cShaderProgram
	{
	public:
		cShaderProgram();
		~cShaderProgram();

		GLuint programID; // The program ID is used so we can find it on the GPU when needed
		std::string friendlyName; // A string name to find the shader on the GPU when needed

		std::map<std::string /*name of the uniform location*/, GLuint /*ID of the uniform location on the GPU*/> mapUniLoc;

		bool LoadUniLoc(std::string uniLocName); // Add a uniform location to the map. returns false if the location is invalid on the shader.
		GLuint GetUniLocFromName(std::string& uniLocName); // Find the uniform location from the map, return -1 if not found
	};

	cShaderManager();
	~cShaderManager();

	bool SetBasePath(std::string pathName); // Path for the shader files to be found. Returns false if path doesn't exists
	cShader CreateShaderFromFile(std::string shaderFileName, cShader::eShaderTypes shaderFileType);
	bool CreateShaderProgramFromFiles(std::string programName, cShader& vertexShader, cShader& fragmentShader); // Create the shader program from the designated vertex and fragment shaders. Returns false in case of an error.
	bool CreateShaderProgramFromFiles(std::string programName, cShader& vertexShader, cShader& fragmentShader, cShader& geometryShader); // Create the shader program from the designated vertex and fragment shaders. Returns false in case of an error.
	bool UseShaderProgramByID(GLuint shaderProgramID); // Sets a shader program to be used by it's ID. Returns false in case of an error or if not found.
	bool UseShaderProgramByName(std::string shaderProgramName); // Sets a shader program to be used by it's friendly name. Returns false in case of an error or if not found.
	GLuint GetShaderProgramIDByName(std::string shaderProgramName); // Returns the shader program ID using it's friendly name

	cShaderProgram* GetShaderProgramByID(GLuint shaderProgramID); // Returns a shader program using the ID
	cShaderProgram* GetShaderProgramByName(std::string shaderProgramName); // Returns a shader program using it's friendly name

	std::string GetLastError(); // Tells the last error that happened with the shaders

	
private:
	std::string _basePath;
	std::string _lastError;

	std::map<GLuint /*shader program ID*/, cShaderProgram /*shader program*/> _mapShaderProgramByID; // Map of shader programs by ID
	std::map<std::string /*shader program name*/, GLuint /*shader program ID*/> _mapShaderProgramIDByName; // Map of shader program ID by name

	bool _LoadShaderCodeFromFile(cShader& shader); // Load a shader to be linked together for the shader program
	bool _CompileShaderCodeFromFile(cShader& shader, std::string& errorText); // Compiles the selected shader

	bool _CheckCompileError(GLuint shaderID, std::string& errorText); // Check if the shader was compiled OK
	bool _CheckLinkError(GLuint shaderProgramID, std::string& errorText); // Check if the shader program was linked OK
};


#endif // !_cShaderManager_HG_
