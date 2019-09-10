#ifndef _cDebugRenderer_HG_
#define _cDebugRenderer_HG_

#include <Interfaces\iDebugRenderer.h>
#include <string>
#include <vector>


class cDrawSystem;
class cShaderManager;
class cDebugRenderer : public iDebugRenderer
{
public:
	cDebugRenderer();
	~cDebugRenderer();

	// The VAO specific way that OpenGL uses
	struct sVertex_xyzw_rgba
	{
		sVertex_xyzw_rgba()
		{
			x = y = z = w = 0.0f;
			r = g = b = a = 0.0f;
		}

		// positions and colours
		float x, y, z, w;
		float r, g, b, a;
	};

	struct sDebugInfo
	{
		sDebugInfo()
		{
			VAO_ID = 0;
			shaderProgramID = 0;
			vertexBufferID = 0;
			numberOfVerticesToDraw = 0;
			numberOfObjectsToDraw = 0;
			bufferSizeBytes = 0;
			bufferSizeObjects = 0;
			bufferSizeVertices = 0;
			pVertexArray = 0;
			bIsValid = false;
		}

		// Keep IDs to search the buffers
		unsigned int VAO_ID;
		unsigned int shaderProgramID;				
		unsigned int vertexBufferID;	

		// Number of objects to draw
		unsigned int numberOfVerticesToDraw;
		unsigned int numberOfObjectsToDraw;

		// Current size of the buffers
		unsigned int bufferSizeBytes;
		unsigned int bufferSizeObjects;
		unsigned int bufferSizeVertices;

		sVertex_xyzw_rgba* pVertexArray;
		bool bIsValid;
	};

	struct sDebugPoint
	{
		sDebugPoint();
		sDebugPoint(glm::vec3 xyz);		// colour=1,1,1		lifeTime=0.0f	pointSize=1.0f
		sDebugPoint(glm::vec3 xyz, glm::vec3 colour, float lifeTime = 0.0f, float pointSize = 1.0f);
		glm::vec3 xyz;
		glm::vec3 colour;
		float pointSize;
		float lifeTime;
		bool bIgnorDepthBufferOnDraw;
	};

	struct sDebugLine
	{
		sDebugLine(glm::vec3 start, glm::vec3 end);
		sDebugLine(glm::vec3 start, glm::vec3 end, glm::vec3 colour, float lifeTime = 0.0f);
		//sDebugLine(glm::vec3 points[2], glm::vec3 colour, float lifeTime = 0.0f);
		glm::vec3 points[2];
		glm::vec3 colour;
		float lifeTime;
		bool bIgnorDepthBufferOnDraw;
	};

	struct sDebugTriangle
	{
		sDebugTriangle();
		sDebugTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);	// colour = 1,1,1	lifeTime=0.0f
		sDebugTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 colour, float lifeTime = 0.0f);
		sDebugTriangle(glm::vec3 v[3], glm::vec3 colour, float lifeTime = 0.0f);
		glm::vec3 v[3];
		glm::vec3 colour;
		float lifeTime;
		bool bIgnorDepthBufferOnDraw;
	};

	void Initialize(cShaderManager* pShaderManager);

	void SetMatView(const glm::mat4& matView);
	void SetMatProjection(const glm::mat4& matProjection);

	virtual void DrawLine(const glm::vec3& start, const glm::vec3& end);
	virtual void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour, float lifeTime);

	void RenderDebugObjects(const glm::mat4& matCameraView, const glm::mat4& matProjection, double deltaTime);

private:
	// Sets the unilocs once to save processing
	void _SetUniLocs();

	// Resize the buffers to a new total size
	bool _ResizeLinesBuffer(unsigned int newNumberOfLines);

	// Initialize a created buffer
	bool _InitBuffer(sDebugInfo& debugInfo);
	
	// Draw objects
	void _CopyLinesIntoInfoBuffer(double deltaTime);
	void _RenderDebugLines(const glm::mat4& matCameraView, const glm::mat4& matProjection, double deltaTime);

	void _Cleanup();

	sDebugInfo _lineDebugInfo;

	// Max number of objects on the buffer, before it needs to be resized
	static const unsigned int _DEFAULTNUMBEROFLINES = 1000;

	std::vector<sDebugLine> _vecLines;

	glm::mat4 _matView;
	glm::mat4 _matProjection;

	// Uniform locations
	GLuint _matModel_Uniloc;
	GLuint _matView_Uniloc;
	GLuint _matProjection_Uniloc;

};


#endif // !_cDebugRenderer_HG_

