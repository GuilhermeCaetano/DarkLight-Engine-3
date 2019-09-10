// cShaderManager.cpp

#include <Shader\cShaderManager.h>
#include <stdio.h>
#include <sys\stat.h>
#include <fstream>
#include <sstream>


cShaderManager::cShaderProgram::cShaderProgram()
{
	this->programID = -1;
	this->friendlyName = "";

	return;
}

cShaderManager::cShaderProgram::~cShaderProgram()
{
	return;
}

bool cShaderManager::cShaderProgram::LoadUniLoc(std::string uniLocName)
{
	GLint new_UniLoc = glGetUniformLocation(this->programID, uniLocName.c_str());

	if (new_UniLoc == -1)
	{
		printf("Error loading %s uniform location!", uniLocName.c_str());
		return false;
	}
	else
	{
		this->mapUniLoc[uniLocName] = (GLuint)new_UniLoc;
		return true;
	}
}

GLuint cShaderManager::cShaderProgram::GetUniLocFromName(std::string& unilocName)
{
	std::map<std::string /*name of the uniform location*/, GLuint /*ID of the uniform location on the GPU*/>::iterator mapIter;
	mapIter = this->mapUniLoc.find(unilocName);

	if (mapIter == this->mapUniLoc.end())
	{
		// Reached the end, so we didn't find a match
		return -1;
	}

	return mapIter->second;
}

cShaderManager::cShaderManager()
{
	this->_basePath = "";
	this->_lastError = "";

	return;
}

cShaderManager::~cShaderManager()
{
	return;
}

bool cShaderManager::SetBasePath(std::string basePath)
{
	struct stat info;

	if (stat(basePath.c_str(), &info) != 0)
	{
		printf("Path %s couldn't be accessed!\n", basePath.c_str());
		return false;
	}
	else if(info.st_mode& S_IFDIR)
	{
		//printf("Shader path set.\n");
		this->_basePath = basePath;
		return true;
	}

	return false;
}



cShaderManager::cShader cShaderManager::CreateShaderFromFile(std::string shaderFileName, cShader::eShaderTypes shaderFileType)
{
	cShader newShader;
	newShader.shaderFileName = shaderFileName;
	newShader.shaderType = shaderFileType;

	return newShader;
}

bool cShaderManager::_LoadShaderCodeFromFile(cShaderManager::cShader& shader)
{
	std::string fullPathName = this->_basePath + shader.shaderFileName;
	const unsigned int fileLenght = 16 * 1024;

	std::ifstream shaderFile(fullPathName.c_str());
	if (!shaderFile.is_open())
	{
		printf("Error loading %s shader from the folder!", shader.shaderFileName.c_str());
		return false;
	}

	shader.vecShaderCodeLines.clear();
	char tempShaderLines[fileLenght] = { 0 }; // 16x1024

	while (shaderFile.getline(tempShaderLines, fileLenght))
	{
		// Add all the lines to the final line vector
		shader.vecShaderCodeLines.push_back(tempShaderLines);
	}

	shaderFile.close();
	return true;
}

bool cShaderManager::_CompileShaderCodeFromFile(cShader& shader, std::string& errorText)
{
	errorText = "";

	const unsigned int maxLineLength = 8 * 1024;
	unsigned int numberOfLines = static_cast<unsigned int>(shader.vecShaderCodeLines.size());

	// Get all the characters from the vector of strings
	char** fileCodeAsChar = new char*[numberOfLines];
	memset(fileCodeAsChar, 0, numberOfLines);

	for (unsigned int stringIndex = 0; stringIndex != numberOfLines; stringIndex++) // This is for each string line on the vector of lines
	{
		unsigned int numOfCharacters = (unsigned int)(shader.vecShaderCodeLines[stringIndex].length()); // This is the size for each string line
		fileCodeAsChar[stringIndex] = new char[numOfCharacters + 2]; // For the \n and \0 at the end
		memset(fileCodeAsChar[stringIndex], 0, numOfCharacters + 2);

		for (unsigned int charIndex = 0; charIndex != shader.vecShaderCodeLines[stringIndex].length(); charIndex++) // This is for each char on the current string line
		{
			// Copy all the chars from the string code vector
			fileCodeAsChar[stringIndex][charIndex] = shader.vecShaderCodeLines[stringIndex][charIndex];
		}

		// End every string char with '\n' and '\0' to avoid garbage
		fileCodeAsChar[stringIndex][numOfCharacters] = '\n';
		fileCodeAsChar[stringIndex][numOfCharacters + 1] = '\0';
	}

	// Compile shader
	glShaderSource(shader.shaderID, numberOfLines, fileCodeAsChar, NULL);
	glCompileShader(shader.shaderID);

	// Clean up, since we used new
	for (unsigned int stringIndex = 0; stringIndex != numberOfLines; stringIndex++)
	{
		delete[] fileCodeAsChar[stringIndex]; // Delete each pointer line
	}
	delete[] fileCodeAsChar; // Also created using new

	std::string error = "";
	if (this->_CheckCompileError(shader.shaderID, error))
	{
		std::stringstream ssError;
		ssError << shader.GetShaderType();
		ssError << " compile error: ";
		ssError << error;
		errorText = ssError.str();
		return false;
	}

	return true;
}

bool cShaderManager::_CheckCompileError(GLuint shaderID, std::string& errorText)
{
	errorText = "";

	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled); // Check compilation status

	if (isCompiled == GL_FALSE) // Uh-oh...
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength); // Find the max error length

		// Get the log from the compilation status
		GLchar* logText = new GLchar[maxLength];
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, logText);

		// Append errors to strings
		errorText.append(logText);
		this->_lastError.append("\n");
		this->_lastError.append(errorText);

		// Clean up since we used new
		delete[] logText;

		return true;
	}

	return false;
}

bool cShaderManager::_CheckLinkError(GLuint shaderProgramID, std::string& errorText)
{
	errorText = "";

	GLint isLinked = 0;
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &isLinked); // Check link status

	if (isLinked == GL_FALSE) // Uh-oh...
	{
		GLint maxLength = 0;
		glGetProgramiv(shaderProgramID, GL_INFO_LOG_LENGTH, &maxLength); // Find the max error length

		// Get the log from the compilation status
		GLchar* logText = new GLchar[maxLength];
		glGetProgramInfoLog(shaderProgramID, maxLength, &maxLength, logText);

		// Append errors to strings
		errorText.append(logText);
		this->_lastError.append("\n");
		this->_lastError.append(errorText);

		// Clean up since we used new
		delete[] logText;

		return true;
	}

	return false;
}

std::string cShaderManager::GetLastError()
{
	std::string returnString = this->_lastError;
	this->_lastError = "";

	return returnString;
}

bool cShaderManager::CreateShaderProgramFromFiles(std::string shaderProgramName, cShaderManager::cShader& vShader, cShaderManager::cShader& fShader)
{
	std::string errorText = "";

	// Load and compile vertex shader
	vShader.shaderID = glCreateShader(GL_VERTEX_SHADER);
	vShader.shaderType = cShader::VERTEX_SHADER;
	if (!this->_LoadShaderCodeFromFile(vShader))
	{
		printf("Error loading the Vertex shader code!\n");
		return false;
	}

	if (!this->_CompileShaderCodeFromFile(vShader, errorText))
	{
		this->_lastError = errorText;
		printf("Error: %s", this->GetLastError().c_str());
		return false;
	}

	// Load and compile fragment shader
	fShader.shaderID = glCreateShader(GL_FRAGMENT_SHADER);
	fShader.shaderType = cShader::FRAGMENT_SHADER;
	if (!this->_LoadShaderCodeFromFile(fShader))
	{
		printf("Error loading the Fragment shader code!\n");
		return false;
	}

	if (!this->_CompileShaderCodeFromFile(fShader, errorText))
	{
		this->_lastError = errorText;
		printf("Error: %s", this->GetLastError().c_str());
		return false;
	}

	// Create and compile shader program
	cShaderProgram currentProgram;
	currentProgram.programID = glCreateProgram();

	glAttachShader(currentProgram.programID, vShader.shaderID);
	glAttachShader(currentProgram.programID, fShader.shaderID);
	glLinkProgram(currentProgram.programID);

	errorText = "";
	if (this->_CheckLinkError(currentProgram.programID, errorText))
	{
		std::stringstream ssError;
		ssError << "Shader program link error: ";
		ssError << errorText;
		this->_lastError = ssError.str();
		printf("Error linking shaders %s!\n", this->_lastError.c_str());
		return false;
	}

	// Shaders were compiled and the program was sucessfully created and linked
	//printf("Shaders compiled and program created.\n");
	currentProgram.friendlyName = shaderProgramName;
	this->_mapShaderProgramByID[currentProgram.programID] = currentProgram;
	this->_mapShaderProgramIDByName[currentProgram.friendlyName] = currentProgram.programID;

	return true;
}

bool cShaderManager::CreateShaderProgramFromFiles(std::string shaderProgramName, cShader& vShader, cShader& fShader, cShader& gShader)
{
	std::string errorText = "";

	// Load and compile vertex shader
	vShader.shaderID = glCreateShader(GL_VERTEX_SHADER);
	vShader.shaderType = cShader::VERTEX_SHADER;
	if (!this->_LoadShaderCodeFromFile(vShader))
	{
		printf("Error loading the Vertex shader code!\n");
		return false;
	}

	if (!this->_CompileShaderCodeFromFile(vShader, errorText))
	{
		this->_lastError = errorText;
		printf("Error: %s", this->GetLastError().c_str());
		return false;
	}

	// Load and compile fragment shader
	fShader.shaderID = glCreateShader(GL_FRAGMENT_SHADER);
	fShader.shaderType = cShader::FRAGMENT_SHADER;
	if (!this->_LoadShaderCodeFromFile(fShader))
	{
		printf("Error loading the Fragment shader code!\n");
		return false;
	}

	if (!this->_CompileShaderCodeFromFile(fShader, errorText))
	{
		this->_lastError = errorText;
		printf("Error: %s", this->GetLastError().c_str());
		return false;
	}

	// Load and compile geometry shader
	gShader.shaderID = glCreateShader(GL_GEOMETRY_SHADER);
	gShader.shaderType = cShader::GEOMETRY_SHADER;
	if (!this->_LoadShaderCodeFromFile(gShader))
	{
		printf("Error loading the Fragment shader code!\n");
		return false;
	}

	if (!this->_CompileShaderCodeFromFile(gShader, errorText))
	{
		this->_lastError = errorText;
		printf("Error: %s", this->GetLastError().c_str());
		return false;
	}

	// Create and compile shader program
	cShaderProgram currentProgram;
	currentProgram.programID = glCreateProgram();

	glAttachShader(currentProgram.programID, vShader.shaderID);
	glAttachShader(currentProgram.programID, fShader.shaderID);
	glAttachShader(currentProgram.programID, gShader.shaderID);
	glLinkProgram(currentProgram.programID);

	errorText = "";
	if (this->_CheckLinkError(currentProgram.programID, errorText))
	{
		std::stringstream ssError;
		ssError << "Shader program link error: ";
		ssError << errorText;
		this->_lastError = ssError.str();
		printf("Error linking shaders %s!\n", this->_lastError.c_str());
		return false;
	}

	// Shaders were compiled and the program was sucessfully created and linked
	printf("Shaders compiled and program created.\n");
	currentProgram.friendlyName = shaderProgramName;
	this->_mapShaderProgramByID[currentProgram.programID] = currentProgram;
	this->_mapShaderProgramIDByName[currentProgram.friendlyName] = currentProgram.programID;

	return true;
}

GLuint cShaderManager::GetShaderProgramIDByName(std::string shaderProgramName)
{
	std::map<std::string /*shader program name*/, GLuint /*shader program ID*/>::iterator mapIter;
	mapIter = this->_mapShaderProgramIDByName.find(shaderProgramName);

	if (mapIter == this->_mapShaderProgramIDByName.end())
	{
		// Didn't find it
		printf("Couldn't find the shader program -%s-.\n", shaderProgramName.c_str());
		return 0;
	}

	return mapIter->second;
}

cShaderManager::cShaderProgram* cShaderManager::GetShaderProgramByName(std::string shaderProgramName)
{
	std::map<std::string /*shader program name*/, GLuint /*shader program ID*/>::iterator mapIDIter;
	std::map<GLuint /*shader program ID*/, cShaderProgram /*shader program*/>::iterator mapProgIter;

	cShaderProgram* returnProgram = nullptr;
	mapIDIter = this->_mapShaderProgramIDByName.find(shaderProgramName);

	if (mapIDIter != this->_mapShaderProgramIDByName.end())
	{
		mapProgIter = this->_mapShaderProgramByID.find(mapIDIter->second);
		if (mapProgIter == this->_mapShaderProgramByID.end())
		{
			printf("No shader program found with this ID!\n");
			return nullptr;
		}
	}
	else
	{
		printf("No shader program ID found with this name!\n");
	}
}
