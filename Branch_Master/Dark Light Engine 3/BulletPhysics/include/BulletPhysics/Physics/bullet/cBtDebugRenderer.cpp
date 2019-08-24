// cBtDebugRenderer.cpp

#include "cBtDebugRenderer.h"
#include "LinearMath\btIDebugDraw.h"
#include <nConvert.h>


namespace nPhysics
{
	cBtDebugRenderer::cBtDebugRenderer(iDebugRenderer* pDebugRenderer)
	{
		this->_pDebugRenderer = pDebugRenderer;

		return;
	}

	cBtDebugRenderer::~cBtDebugRenderer()
	{
		return;
	}

	void cBtDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
	{
		//this->_debugRenderer->addLine(nConvert::ToSimple(from), nConvert::ToSimple(to), nConvert::ToSimple(fromColor), nConvert::ToSimple(toColor));
	}

	void cBtDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& colour)
	{
		this->_pDebugRenderer->DrawLine(nConvert::ToSimple(from), nConvert::ToSimple(to), nConvert::ToSimple(colour), 10.0f);
	}

	void cBtDebugRenderer::drawSphere(const btVector3& p, btScalar radius, const btVector3& color)
	{
		//this->_pDebugRenderer->addDebugSphere(nConvert::ToSimple(p), nConvert::ToSimple(color), nConvert::ToSimple(radius), 10.0f);
	}

	void cBtDebugRenderer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha)
	{
		//this->_pDebugRenderer->addTriangle(nConvert::ToSimple(a), nConvert::ToSimple(b), nConvert::ToSimple(c), nConvert::ToSimple(color), 10.0f);
	}

	void cBtDebugRenderer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		//this->_pDebugRenderer->addPoint(nConvert::ToSimple(PointOnB), nConvert::ToSimple(color), (float)lifeTime, nConvert::ToSimple(distance));
	}

	void cBtDebugRenderer::reportErrorWarning(const char* warningString)
	{
		
	}

	void cBtDebugRenderer::draw3dText(const btVector3 & location, const char * textString)
	{
	}

	void cBtDebugRenderer::setDebugMode(int debugMode)
	{
		this->_debugMode = static_cast<DebugDrawModes>(debugMode);
	}
}

