// cDebugRenderer.cpp

#include "cDebugRenderer.h"
#include <Shader\cShaderManager.h>


cDebugRenderer::cDebugRenderer()
{
	this->_matProjection = glm::mat4(1.0f); // identity

	return;
}

cDebugRenderer::~cDebugRenderer()
{
	return;
}

cDebugRenderer::sDebugLine::sDebugLine(glm::vec3 start, glm::vec3 end)
{
	// Set points on the array
	this->points[0] = start;
	this->points[1] = end;
}

cDebugRenderer::sDebugLine::sDebugLine(glm::vec3 start, glm::vec3 end, glm::vec3 colour, float lifeTime)
{
	// Set points and colours on the array
	this->points[0] = start;
	this->points[1] = end;

	this->colour = colour;
	this->lifeTime = lifeTime;
}

void cDebugRenderer::SetMatView(const glm::mat4 & matView)
{
	this->_matView = matView;
}

void cDebugRenderer::SetMatProjection(const glm::mat4& matProjection)
{
	this->_matProjection = matProjection;
}

void cDebugRenderer::Initialize(cShaderManager* pShaderManager)
{
	// Load debug shader
	cShaderManager::cShader debugVertexLineShader = pShaderManager->CreateShaderFromFile("vertexDebugLine.glsl", cShaderManager::cShader::VERTEX_SHADER);
	cShaderManager::cShader debugFragmentLineShader = pShaderManager->CreateShaderFromFile("fragmentDebugLine.glsl", cShaderManager::cShader::FRAGMENT_SHADER);
	cShaderManager::cShader debugGeometryLineShader = pShaderManager->CreateShaderFromFile("geometryDebugLine.glsl", cShaderManager::cShader::GEOMETRY_SHADER);
	if (!pShaderManager->CreateShaderProgramFromFiles("debugLineShader", debugVertexLineShader, debugFragmentLineShader, debugGeometryLineShader))
	{
		// Wait for input, so we can see the code
		getchar();
	}

	this->_lineDebugInfo.shaderProgramID = pShaderManager->GetShaderProgramIDByName("debugLineShader");

	this->_SetUniLocs();
}

void cDebugRenderer::_SetUniLocs()
{
	GLuint shaderProgramID = this->_lineDebugInfo.shaderProgramID;

	glUseProgram(shaderProgramID);

	this->_matModel_Uniloc = glGetUniformLocation(shaderProgramID, "mModel");
	this->_matView_Uniloc = glGetUniformLocation(shaderProgramID, "mView");
	this->_matProjection_Uniloc = glGetUniformLocation(shaderProgramID, "mProjection");

	glUseProgram(0);

	// Start buffer sizes
	this->_ResizeLinesBuffer(this->_DEFAULTNUMBEROFLINES);

}

bool cDebugRenderer::_ResizeLinesBuffer(unsigned int newNumberOfLines)
{
	// Erase any exisiting buffers 
	if (this->_lineDebugInfo.bIsValid)
	{
		delete[] this->_lineDebugInfo.pVertexArray;

		// Delete VBO and VAO
		glDeleteBuffers(1, &(this->_lineDebugInfo.vertexBufferID));
		glDeleteVertexArrays(1, &(this->_lineDebugInfo.VAO_ID));
	}

	// Add 10% more size to the buffer
	newNumberOfLines = (unsigned int)(newNumberOfLines * 1.1);

	// Adjust new values insode the struct
	this->_lineDebugInfo.bufferSizeObjects = newNumberOfLines;
	this->_lineDebugInfo.bufferSizeVertices = newNumberOfLines * 2;
	this->_lineDebugInfo.bufferSizeBytes = 0;
	this->_lineDebugInfo.numberOfObjectsToDraw = 0;
	this->_lineDebugInfo.numberOfVerticesToDraw = 0;

	// Re-initialize buffer
	return this->_InitBuffer(this->_lineDebugInfo);
}

bool cDebugRenderer::_InitBuffer(sDebugInfo& debugInfo)
{
	glUseProgram(debugInfo.shaderProgramID);

	// Create the Vertex Buffer Object (VBO)
	glGenBuffers(1, &(debugInfo.vertexBufferID));
	glBindBuffer(GL_ARRAY_BUFFER, debugInfo.vertexBufferID);

	// Create the Vertex Array Object (VAO)
	glGenVertexArrays(1, &(debugInfo.VAO_ID));
	glBindVertexArray(debugInfo.VAO_ID);

	debugInfo.pVertexArray = new sVertex_xyzw_rgba[debugInfo.bufferSizeVertices];

	// Clear buffer
	debugInfo.bufferSizeBytes = sizeof(sVertex_xyzw_rgba) * debugInfo.bufferSizeVertices;
	memset(debugInfo.pVertexArray, 0, debugInfo.bufferSizeBytes);

	// Copy the local vertex array into the GPUs memory
	glBufferData(GL_ARRAY_BUFFER, debugInfo.bufferSizeBytes, debugInfo.pVertexArray, GL_DYNAMIC_DRAW);

	GLuint vPos_Uniloc = glGetAttribLocation(debugInfo.shaderProgramID, "vPosition");
	GLuint vCol_Uniloc = glGetAttribLocation(debugInfo.shaderProgramID, "vColour");

	// Size of the vertex we are using in the array.
	// This is called the "stride" of the vertices in the vertex buffer
	const unsigned int VERTEX_SIZE_OR_STRIDE_IN_BYTES = sizeof(sVertex_xyzw_rgba);

	glEnableVertexAttribArray(vPos_Uniloc);
	const unsigned int OFFSET_TO_X_IN_CVERTEX = offsetof(sVertex_xyzw_rgba, x);
	glVertexAttribPointer(vPos_Uniloc, 4, GL_FLOAT, GL_FALSE, VERTEX_SIZE_OR_STRIDE_IN_BYTES, reinterpret_cast<void*>(static_cast<uintptr_t>(OFFSET_TO_X_IN_CVERTEX)));	// 64-bit


	glEnableVertexAttribArray(vCol_Uniloc);
	const unsigned int OFFSET_TO_R_IN_CVERTEX = offsetof(sVertex_xyzw_rgba, r);
	glVertexAttribPointer(vCol_Uniloc, 4, GL_FLOAT, GL_FALSE, VERTEX_SIZE_OR_STRIDE_IN_BYTES, reinterpret_cast<void*>(static_cast<uintptr_t>(OFFSET_TO_R_IN_CVERTEX)));	// 64-bit

	debugInfo.bIsValid = true;

	glBindVertexArray(0);

	// Unbind and disable everything
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(vPos_Uniloc);
	glDisableVertexAttribArray(vCol_Uniloc);

	glUseProgram(0);

	return true;
}

void cDebugRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end)
{
	sDebugLine line(start, end);
	this->_vecLines.push_back(line);
}

void cDebugRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour, float lifeTime)
{
	sDebugLine line(start, end, colour, lifeTime);
	this->_vecLines.push_back(line);
}


void cDebugRenderer::RenderDebugObjects(const glm::mat4& matCameraView, const glm::mat4& matProjection, double deltaTime)
{
	// Copy and draw lines
	this->_CopyLinesIntoInfoBuffer(deltaTime);
	this->_RenderDebugLines(matCameraView, matProjection, deltaTime);
}

void cDebugRenderer::_CopyLinesIntoInfoBuffer(double deltaTime)
{
	this->_lineDebugInfo.numberOfObjectsToDraw = (unsigned int)this->_vecLines.size();

	// Is the draw buffer big enough? 
	if (this->_lineDebugInfo.bufferSizeObjects < this->_lineDebugInfo.numberOfObjectsToDraw)
	{
		// Resize the buffer
		this->_ResizeLinesBuffer(this->_lineDebugInfo.numberOfObjectsToDraw);
	}

	this->_lineDebugInfo.numberOfVerticesToDraw = this->_lineDebugInfo.numberOfObjectsToDraw * 2;

	unsigned int vertexIndex = 0;	// index of the vertex buffer to copy into 
	unsigned int lineIndex = 0;		// index of the triangle buffer

	// Each line contains two vertexes, so for each line, two vertexes are counted
	for (lineIndex; lineIndex < this->_lineDebugInfo.numberOfObjectsToDraw; lineIndex++, vertexIndex += 2)
	{
		sDebugLine line = this->_vecLines[lineIndex];

		this->_lineDebugInfo.pVertexArray[vertexIndex + 0].x = line.points[0].x;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 0].y = line.points[0].y;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 0].z = line.points[0].z;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 0].w = 1.0f;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 0].r = line.colour.r;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 0].g = line.colour.g;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 0].b = line.colour.b;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 0].a = 1.0f;

		this->_lineDebugInfo.pVertexArray[vertexIndex + 1].x = line.points[1].x;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 1].y = line.points[1].y;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 1].z = line.points[1].z;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 1].w = 1.0f;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 1].r = line.colour.r;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 1].g = line.colour.g;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 1].b = line.colour.b;
		this->_lineDebugInfo.pVertexArray[vertexIndex + 1].a = 1.0f;

		line.lifeTime -= static_cast<float>(deltaTime);
	}

	this->_vecLines.clear();
	unsigned int numberOfBytesToCopy = this->_lineDebugInfo.numberOfVerticesToDraw * sizeof(sVertex_xyzw_rgba);

	GLenum err = glGetError();

	glBindBuffer(GL_ARRAY_BUFFER, this->_lineDebugInfo.vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, numberOfBytesToCopy, this->_lineDebugInfo.pVertexArray, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	err = glGetError();
	std::string error;
	std::string errDetails;
}

void cDebugRenderer::_RenderDebugLines(const glm::mat4& matCameraView, const glm::mat4& matProjection, double deltaTime)
{
	// Start rendering 
	glUseProgram(this->_lineDebugInfo.shaderProgramID);

	// Model matrix is just set to identity. 
	// So the values in the buffers are in WORLD coordinates (untransformed)
	glUniformMatrix4fv(this->_matModel_Uniloc, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(this->_matView_Uniloc, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(matCameraView));
	glUniformMatrix4fv(this->_matProjection_Uniloc, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(matProjection));

	//TODO: Right now, the objects are drawn WITHOUT the depth buffer
	//      To be added is the ability to draw objects with and without depth
	//      (like some objects are draw "in the scene" and others are drawn 
	//       "on top of" the scene)
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	//glEnable(GL_PROGRAM_POINT_SIZE);
	//glPointSize(50.0f);

	// Draw Lines
	glBindVertexArray(this->_lineDebugInfo.VAO_ID);
	glDrawArrays(GL_LINES, 0, this->_lineDebugInfo.numberOfVerticesToDraw);

	glBindVertexArray(0);
	glUseProgram(0);

	// Put everything back as it was 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Default
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}
