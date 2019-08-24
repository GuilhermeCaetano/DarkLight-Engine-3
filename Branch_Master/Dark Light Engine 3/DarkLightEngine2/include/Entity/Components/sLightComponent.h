#ifndef _sLightComponent_HG_
#define _sLightComponent_HG_

#include <Entity\Components\iComponent.h>
#include <OpenGL\OpenGLHeaders.h>


struct sLightComponent : public iComponent
{
public:
	sLightComponent(unsigned long long entityID);
	~sLightComponent();

	virtual void Start();

	// This must be overriden on the derived components, otherwise it will return an error!
	static std::string GetStaticType() { return "LIGHT_COMPONENT"; };

	enum sLightType
	{
		POINT_LIGHT,  // 0
		DIRECTIONAL_LIGHT, // 1
		SPOT_LIGHT  // 2
	};

	void SetLightType(sLightType lightType); // Setting the type of light. Point, Directional or Spot
	void SetPosition(glm::vec4 lightPos); // Setting the light position
	void SetDiffuseColour(glm::vec4 lightColour); // Setting the light colour
	void SetConstantAttenuation(float constAtten); // Setting constant attenuation
	void SetLinearAttenuation(float linAtten); // Setting linear attenuation
	void SetQuadraticAttenuation(float quadAtten); // Setting quadratic attenuation
	void SetLightTurnedOn(bool isLightOn); // Setting whether the light is on or not
	void SetSpotConeAngles(float innerAngleDegrees, float outerAngleDegrees); // Setting the angle for the inner and outer cone angles of the spot light
	void SetRelativeDirection(glm::vec3 relDirection); // Setting light direction
	void SetRelativeDirectionByEulerAngles(glm::vec3 directionAngle);
	void SetRelativeDirectionByLookAt(glm::vec3 pointInWorldXYZ);

	int position_UniLoc;
	int diffuse_UniLoc;
	int specular_UniLoc;
	int atten_UniLoc;
	int direction_UniLoc;
	int param1_UniLoc;
	int param2_UniLoc;

	sLightType lightType;

	glm::vec4 position; // XYZ position of the light "bulb" on scene
	glm::vec4 diffuse; // The natural colour of the light. Normally white or yellow
	glm::vec4 attenuation;	// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	glm::vec4 direction;	// Spot, directional lights
	glm::vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	glm::vec4 param2;	// x = 0 for off, 1 for on

	bool bisWireframeOn;

protected:
	virtual void _SerializeToJson(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& prettyWriter);
	virtual void _DeserializeFromJson(rapidjson::Value::ConstValueIterator iter);

};



#endif // !_sLightComponent_HG_

