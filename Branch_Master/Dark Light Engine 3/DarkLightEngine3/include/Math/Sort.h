#ifndef _Sort_HG_
#define _Sort_HG_

#include <OpenGL\OpenGLHeaders.h>
#include <Entity\sEntity.h>
#include <Entity\Components\sMeshComponent.h>
#include <vector>

// Sorts objects in order of proximity. Uses bubble algorithm
void BubbleSort(std::vector<std::pair<sEntity*, sMeshComponent*>>& pVecObjectsToDraw, glm::vec3 comparePosition);


#endif // !_Sort_HG_

