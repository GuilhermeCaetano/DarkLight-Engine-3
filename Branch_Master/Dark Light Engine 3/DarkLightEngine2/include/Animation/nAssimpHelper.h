#ifndef _nAssimpHelper_HG_
#define _nAssimpHelper_HG_

#include <Animation\Assimp\matrix4x4.h>
#include <OpenGL\OpenGLHeaders.h>

namespace nAssimpHelper
{
	glm::mat4 AIMatrixToGLMMatrix(const aiMatrix4x4 &mat);
}

#endif // !_nAssimpHelper_HG_
