#ifndef _cFollowCurveCommand_HG_
#define _cFollowCurveCommand_HG_

#include "iCommand.h"
#include "cQuaternions.h"
#include "cMeshObject.h"
#include <vector>

class cFollowCurveCommand : public iCommand
{
public:
	cFollowCurveCommand(unsigned long long uniqueID); // c'tor
	~cFollowCurveCommand(); // d'tor

	// Takes a json string that contains name:value pairs
	virtual void Setup(rapidjson::StringBuffer& buffer);

	// Called every frame to check functionality
	virtual void Update(double deltaTime);
	virtual bool IsFinished();

	void CalculateBezierPoints();
	glm::vec3 CatMullRom(float currentBezierInterval);
	void DrawBezier();
	void UpdateMovement(double deltaTime);
	void UpdateOrientation(double deltaTime, glm::vec3 bezierPoint);
	void OrientateImmediately();

	virtual glm::vec3 GetCurrentPosition();

	glm::vec3 GetBezierPoint(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 point4, float t);

private:
	int _ID;
	std::string _type;

	cMeshObject* _followObject;

	int _currentBezierIndex;
	std::vector<glm::vec3> _controlPoints; // Will use this one to store the control points
	std::vector<glm::vec3> _pointsAlongBezier; // Will use this to store the points calculated along a bezier style curve

	glm::vec3 _currentPosition; // Position along the bezier
	glm::vec3 _currentDirection;
	float _currentLerpVelocity;
	glm::quat _currentOrientation; // Current orientation

	float _speed;
	float _moveTime;
	float _currentMoveTime;

	bool _setup;
	bool _bBezierPathFinished;
	bool _bFinishedFollowingBezier;

	bool _bIsFinished;
	bool _bIsInfinite;
	int _lifeTime;

	glm::vec3 debugBezierPoint;

	virtual void Reset();
};


#endif // !_cFollowCurveCommand_HG_

