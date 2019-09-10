// cFBO.cpp

#include <FBO\cFBO.h>

cFBO::cFBO()
{
	this->_ID = 0;
	this->_colourTexture0_ID[2] = {0};
	this->_normalTexture1_ID = 1;
	this->_vertexWorldPos2_ID = 2;
	this->_depthTexture_ID = 0;
	this->_width = -1;
	this->_height = -1;

	return;
}

cFBO::~cFBO()
{
	return;
}

bool cFBO::InitFBO(int width, int height, std::string &error)
{
	this->_width = width;
	this->_height = height;

	// Generate the frame buffers to hold the information
	glGenFramebuffers(1, &(this->_ID));
	glBindFramebuffer(GL_FRAMEBUFFER, this->_ID);

	// Generate Colour buffer
	glGenTextures(2, this->_colourTexture0_ID);
	glBindTexture(GL_TEXTURE_2D, this->_colourTexture0_ID[0]);

	// Allocate storage for colour buffer
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, this->_width, this->_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, this->_colourTexture0_ID[1]);

	// Allocate storage for colour buffer
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, this->_width, this->_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, this->_colourTexture0_ID[0]);


	// Generate Normal buffer
	glGenTextures(1, &(this->_normalTexture1_ID));
	glBindTexture(GL_TEXTURE_2D, this->_normalTexture1_ID);

	// Allocate storage for normal buffer
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, this->_width, this->_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate Vertex buffer
	glGenTextures(1, &(this->_vertexWorldPos2_ID));	
	glBindTexture(GL_TEXTURE_2D, this->_vertexWorldPos2_ID);

	// Allocate storage for vertex buffer
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, this->_width, this->_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate Depth buffer
	glGenTextures(1, &(this->_depthTexture_ID));
	glBindTexture(GL_TEXTURE_2D, this->_depthTexture_ID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, this->_width, this->_height);

	// Note that, unless you specifically ask for it, the stencil buffer
	// is NOT present... i.e. GL_DEPTH_COMPONENT32F DOESN'T have stencil

	// These are:
	// - GL_DEPTH32F_STENCIL8, which is 32 bit float depth + 8 bit stencil
	// - GL_DEPTH24_STENCIL8,  which is 24 bit float depth + 8 bit stencil (more common?)
	
	//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT );
	//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_COMPONENT );
	//	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, this->width, this->height, 0, GL_EXT_packe

	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_TEXTURE_2D, this->depthTexture_ID, 0);
	
	// Attach the buffers 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->_colourTexture0_ID[0], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, this->_colourTexture0_ID[1], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, this->_normalTexture1_ID, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, this->_vertexWorldPos2_ID, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, this->_depthTexture_ID, 0);

	static const GLenum draw_bufers[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};
	glDrawBuffers(4, draw_bufers);

	// Check frame buffer status
	bool bFrameBufferIsGoodToGo = true;

	switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
	case GL_FRAMEBUFFER_COMPLETE:
		bFrameBufferIsGoodToGo = true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		bFrameBufferIsGoodToGo = false;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
		bFrameBufferIsGoodToGo = false;
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		error = "GL_FRAMEBUFFER_UNSUPPORTED";
		bFrameBufferIsGoodToGo = false;
		break;
	default:
		bFrameBufferIsGoodToGo = false;
		break;
	}

	// Point back to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return bFrameBufferIsGoodToGo;
}

bool cFBO::ShutdownFBO()
{
	// Delete buffers
	glDeleteTextures(1, &(this->_colourTexture0_ID[0]));
	glDeleteTextures(1, &(this->_colourTexture0_ID[1]));
	glDeleteTextures(1, &(this->_normalTexture1_ID));
	glDeleteTextures(1, &(this->_vertexWorldPos2_ID));
	glDeleteTextures(1, &(this->_depthTexture_ID));

	glDeleteFramebuffers(1, &(this->_ID));

	return true;
}

void cFBO::ClearBuffers(bool bClearColour, bool bClearDepth)
{
	glViewport(0, 0, this->_width, this->_height);
	GLfloat	zero = 0.0f;
	GLfloat one = 1.0f;

	if (bClearColour)
	{
		glClearBufferfv(GL_COLOR, 0, &zero);
		glClearBufferfv(GL_COLOR, 1, &zero);
		glClearBufferfv(GL_COLOR, 2, &zero);
	}
	if (bClearDepth)
	{
		glClearBufferfv(GL_DEPTH, 0, &one);		// Depth is normalized 0.0 to 1.0f
	}
	// If buffer is GL_STENCIL, drawbuffer must be zero, and value points to a 
	//  single value to clear the stencil buffer to. Masking is performed in the 
	//  same fashion as for glClearStencil. Only the *iv forms of these commands 
	//  should be used to clear stencil buffers; be used to clear stencil buffers; 
	//  other forms do not accept a buffer of GL_STENCIL.

	// 
	glStencilMask(0xFF);

	{	// Clear stencil
		//GLint intZero = 0;
		//glClearBufferiv(GL_STENCIL, 0, &intZero );
		glClearBufferfi(GL_DEPTH_STENCIL,
			0,		// Must be zero
			1.0f,	// Clear value for depth
			0);	// Clear value for stencil
	}
}

void cFBO::ClearColourBuffer(int bufferindex)
{
	glViewport(0, 0, this->_width, this->_height);
	GLfloat	zero = 0.0f;

	glClearBufferfv(GL_COLOR, bufferindex, &zero);
}

//void cFBO::ClearDepthBuffer()
//{
//	glViewport(0, 0, this->_width, this->_height);
//	GLfloat	zero = 0.0f;
//
//	glClearBufferfv(GL_DEPTH, bufferindex, &zero);
//}
//
//void cFBO::ClearStencilBuffer(int clearColour, int mask = 0xFF)
//{
//	glViewport(0, 0, this->_width, this->_height);
//	GLfloat	zero = 0.0f;
//
//	glClearBufferfv(GL_STENCIL, bufferindex, &zero);
//}

int cFBO::GetMaxColourAttachments()
{
	int maxColourAttach = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColourAttach);

	return maxColourAttach;
}

int cFBO::GetMaxDrawBuffers()
{
	int maxDrawBuffers = 0;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);

	return maxDrawBuffers;
}
