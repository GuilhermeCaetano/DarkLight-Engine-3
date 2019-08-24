#ifndef _cFBO_HG_
#define _cFBO_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <string>


class cFBO
{
public:
	cFBO();
	~cFBO();

	bool InitFBO(int width, int height, std::string &error);
	bool ShutdownFBO();
	bool ResetFBO(int width, int height, std::string &error);

	void ClearBuffers(bool bClearColour = true, bool bClearDepth = true);

	void ClearColourBuffer(int bufferindex);
	void ClearDepthBuffer();
	void ClearStencilBuffer(int clearColour, int mask = 0xFF);

	int GetMaxColourAttachments();
	int GetMaxDrawBuffers();

	GLuint _ID;						// = 0;
	GLuint _colourTexture0_ID[2];		// = 0;
	GLuint _normalTexture1_ID;
	GLuint _vertexWorldPos2_ID;
	GLuint _depthTexture_ID;		// = 0;

private:
	//	GLuint TBDTexture_2_ID;
	//	GLuint TBDTexture_3_ID;

	
	GLint _width;		// = 512 the WIDTH of the framebuffer, in pixels;
	GLint _height;

};



#endif // !_cFBO_HG_

