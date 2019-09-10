// cShader.cpp

#include <Shader\cShaderManager.h>

// the shader class itself is kinda big, so we'll separate it from the manager class

cShaderManager::cShader::cShader()
{
	this->shaderType = cShader::eShaderTypes::UNKNOWN;
	this->shaderID = -1;

	return;
}

cShaderManager::cShader::~cShader()
{
	return;
}

std::string cShaderManager::cShader::GetShaderType()
{
	switch (this->shaderType)
	{
		case eShaderTypes::VERTEX_SHADER:
			return "VERTEX_SHADER";
		break;
		case eShaderTypes::FRAGMENT_SHADER:
			return "FRAGMENT_SHADER";
		break;
		case eShaderTypes::GEOMETRY_SHADER:
			return "Geometry_Shader";
		break;
		case eShaderTypes::TESSELATION_SHADER:
			return "Tesselation_Shader";
		break;
		case eShaderTypes::COMPUTATION_SHADER:
			return "Computation_Shader";
		break;
		case eShaderTypes::UNKNOWN:
			return "Unknown"; // Shouldn't happen
		break;
	}

	return "Unknown"; // Also shouldn't happen
}