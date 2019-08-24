#version 420
// fragment01.glsl

// In from a previous stage (vertex shader)
in vec4 color;			// in colour from the vertex shader
in vec4 vertPosWorld;	// in vertex position from the vertex shader
in vec4 vertNormal;		// in vertex normal from the vertex shader
in vec4 vertUV_x2; 		// in texture coordinates from the vertex shader
in vec4 vertTanXYZ;		// in tangent to the surface from the vertex shader
in vec4 vertBiNormXYZ;	// in bi-normal (or bi-tangent) to the surface from the vertex shader

// Alpha map from vertex shader
uniform float alphaTextureOverride;
in float alphaMapDiscardOverride;
uniform float blackThreshold;

//uniform vec3 objectColour;
uniform vec4 objectDiffuse;
uniform vec4 objectSpecular;

uniform vec3 eyeLocation;

// For the 2 pass rendering
uniform float renderPassNumber;	// 1 = 1st pass, 2nd for offscreen to quad
uniform sampler2D texPass1OutputTexture;
uniform sampler2D passExtraTex;

// Cube map texture for the skybox. Note that sampler3D is another thing
uniform samplerCube textureSkyBoxDay;
uniform samplerCube textureSkyBoxNight;
uniform float dayMix;
uniform bool useSkyBoxTexture;

// Textures
uniform sampler2D texture00;   // There are more types of samplers. They can't be an array...
uniform sampler2D texture01;
uniform sampler2D texture02;
uniform sampler2D texture03;
uniform sampler2D texture04;
uniform sampler2D texture05;
uniform sampler2D texture06;
uniform sampler2D texture07;

uniform vec4 texBlendWeights[2]; // that's 8, where x = 0, y = 1, z = 2, w = 3

// Flags
uniform bool useVertexColour; // This will tell if we should use the colours by each vertex
uniform bool isSelected; // If the object is "selected" for us to move it around
uniform float reflectionRate;
uniform float refractionRate;

// Control shader type... change later for separate shaders when there is time
uniform bool bNoLighting;
uniform bool bNoTextures;
uniform bool bNoSkybox;
uniform bool bUseBloom;
uniform bool bUseCelShade;
uniform bool bUseNightVision;

//uniform float waterTextureID; // 0, not water, 1 upper, 2 lower
//uniform vec4 waterOffset;

// Is the light affecting this objectColour
uniform bool isLightAffecting;

struct sLight
{
	vec4 position;	// Light "bulb" position		
	vec4 diffuse;	// Natural colour. Usually white or yellow
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	vec4 param2;	// x = 0 for off, 1 for on
};

// What kind of light is the actual light
const int sLightType_POINT = 0;  // Point in the world, a light that shines spherically
const int sLightType_DIRECTIONAL = 1;  // A light that shines equally, like the sun
const int sLightType_SPOT = 2;  // A light that shines like a spotlight

const int numberOfLights = 50;
uniform sLight theLights[numberOfLights];

// Flags for reflection and refraction
bool bUseReflect = (reflectionRate > 0.0f);
bool bUseRefract = (refractionRate > 0.0f);

layout (location = 0) vec4 renderColour;			// GL_COLOR_ATTACHMENT0
layout (location = 1) vec4 brightColour;         // GL_COLOR_ATTACHMENT1

out vec4 finalOutputColour;			// GL_COLOR_ATTACHMENT0
out vec4 finalOutputNormal;			// GL_COLOR_ATTACHMENT2
out vec4 finalOutputVertWorldPos;	// GL_COLOR_ATTACHMENT3

void main()
{
	// Set everything to black initially
	finalOutputColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	finalOutputNormal = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	finalOutputVertWorldPos = vertPosWorld;
	bool useBlur = false;
	
	// Are we in the 2nd pass? 
	if ( int(renderPassNumber) == 2 )
	{ 
		// 2nd pass
		
		// First get the normal render colour
		renderColour.rgb = texture(texPass1OutputTexture, vertUV_x2.st).rgb;
		renderColour.a = 1.0f;
		
		// Add the extra texture
		//vec3 extraTex = texture(passExtraTex, vertUV_x2.st).rgb;
		//float pixelBW = (extraTex.r + extraTex.g + extraTex.b);
		//if(pixelBW <= 0.3f) // We only care about non-white pixels
		//{
			//finalOutputColour.rgb = vec3(0.0f, 0.0f, 0.0f);
		//}
		
		float weights[5] = float[] (0.227027f, 0.1945946f, 0.1216216f, 0.054054, 0.016216f);
		
		// Generate the texture coordinates from the fragment location on screen
		vec2 textCoords = vec2( gl_FragCoord.x / 1920, gl_FragCoord.y / 1080 );
		vec3 vertexMaterialColour = texture( texPass1OutputTexture, vertUV_x2.st ).rgb;
		
		// Save the rendering colour. This is the normal texture colour
		renderColour.rgb = vertexMaterialColour;
		renderColour.a = 1.0f;
		
		// Check for bloom
		if(bUseBloom)
		{
			// Essentially to do the bloom, we need to sample the adjacent pixels
			// from the current fragment pixel, just like below. we step through the adjacent
			// pixels and sample the colour in there. We want to bloom only what is "white" enough to bloom,
			// so we use dot the pixel colour in order to transform it into a grayscale value to check the whiteness.
		
			// * * * * *		   * 
			// * * * * *           *
			// * * * * *       * * * * *
			// * * * * *           *
			// * * * * *           *
			
			// Amount to step through the pixels
			float step = 0.001f;
			float size = 0.003f;
			
			// Current sampled colour total
			vec3 vecSample = vec3(0.0f, 0.0f, 0.0f );
			int count = 0;
			
			// Go through the adjacent pixels
			for ( float x = -size; x <= size; x += step )
			{
				for ( float y = -size; y <= size; y += step )
				{
					count++;
					
					// save the current sample
					vec3 testSample = texture( texPass1OutputTexture, vertUV_x2.st + vec2(x, y)).rgb;
					vec3 testSave = testSample;
					
					// Check the whiteness of the grayscale value and use it or black it
					float brightness = dot(testSample, vec3(0.2126f, 0.7152f, 0.0722f));
					if(brightness > 0.99f)
					{
						testSample = testSave;
						//testSample = vec3(0.0f, 0.0f, 0.0f);
					}
					else
					{
						testSample = vec3(0.0f, 0.0f, 0.0f);
					}
					
					// Add value to overall colour
					vecSample += testSample;
				}
			}
			
			// Set brightness value to the overall sampled value
			brightColour.rgb = (vecSample.rgb / float(count));
						
			// Blend the render and the brightness colours
			finalOutputColour = renderColour + brightColour;
			
			return;
		}
		else if(useBlur)
		{
			if(vertexMaterialColour.r > 0.5f && vertexMaterialColour.g > 0.5f && vertexMaterialColour.b > 0.5f)
			{
				// * * * * *		   * 
				// * * * * *           *
				// * * * * *       * * * * *
				// * * * * *           *
				// * * * * *           *
				float step = 0.001f;
				float size = 0.003f;
		//		
				vec3 vecSample = vec3(0.0f, 0.0f, 0.0f );
				int count = 0;
				
				for ( float x = -size; x <= size; x += step )
				{
					for ( float y = -size; y <= size; y += step )
					{
						count++;
						vecSample += texture( texPass1OutputTexture, vertUV_x2.st + vec2(x, y)).rgb;
					}
				}
				
		//	
				finalOutputColour.rgb = (vecSample.rgb / float(count));
				finalOutputColour.a = 1.0f;
			}
		}
		else if(bUseNightVision)
		{
			// We check each vertex colour, and use the grayscale value to check the brightness.
			// The values bright enough will be coloured green and the ones not will be black.
			float brightness = dot(renderColour.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
			if(brightness > 0.5f)
			{
				renderColour.rgb = vec3(0.0f, 1.0f, 0.0f);
			}
			else
			{
				renderColour.rgb = vec3(0.0f, 0.0f, 0.0f);
			}
			
			finalOutputColour.rgb = renderColour.rgb;
			finalOutputColour.a = 1.0f;
		}
		else
		{
			finalOutputColour.rgb = renderColour.rgb;
			finalOutputColour.a = 1.0f;
		}
		/* else if(useCelShading)
		{
			// The cel-shaded shader is done by checking the cosine of the angle
			// between the light's direction to the vertex and the vertex normal.
			// Depending on the angle, the colour is adjusted to more absolute values,
			// giving the colours a clear transition in tones.
			
			// Get the light direction and the normal
			vec3 lightToVertex = theLights[i].position.xyz - vertPosWorld.xyz; // Direction vector
			vec3 lightVector = normalize(lightToVertex); // Normalized direction vector
			vec3 vertexNormal = normalize(vertNormal);
			
			// Calculate the cosine angle
			float dotValue = dot(lightVector, vertexNormal);
			float cosine = dotValue;
			cosine = max(0.0f, cosine);
				
			// Use angle to create step colours
			if(cosine > 0.9f)
			{
				renderColour *= vec4(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else if(cosine > 0.5f)
			{
				renderColour *= vec4(0.5f, 0.5f, 0.5f, 1.0f);
			}
			else if(cosine > 0.1f)
			{
				renderColour *= vec4(0.1f, 0.1f, 0.1f, 1.0f);
			}
			else
			{
				renderColour *= vec4(0.0f, 0.0f, 0.0f, 1.0f);
			}
			
			finalOutputColour = renderColour;
			finalOutputColour.a = 1.0f;
		} */
		
		return;
	}
	
	vec4 materialDiffuse = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 materialSpecular = objectSpecular;
	
	// FOR NOW, I'm adding the bi-normal and tangent here
	// (so they won't be optimized out)
	materialDiffuse.xyz += ( vertTanXYZ.xyz * 0.001f);
	materialDiffuse.xyz += ( vertBiNormXYZ.xyz * 0.001f);
	
	// Check for the skybox first
	if(useSkyBoxTexture == true && bNoSkybox == false)
	{
		// Colouring with the skybox is done by using the pixel normal
		// like a raycast instead of uv coordinates to paint it according to the texture
		vec3 skyboxColourDay = texture(textureSkyBoxDay, vertNormal.xyz).rgb;
		vec3 skyboxColourNight = texture(textureSkyBoxNight, vertNormal.xyz).rgb;
		finalOutputColour.rgb = (skyboxColourDay * dayMix) + (skyboxColourNight * (1.0f - dayMix));
		finalOutputColour.a = 1.0f;
		return;
	}

	// Then check for reflection and refraction
	vec3 rgbReflect = vec3(0.0f,0.0f,0.0f);
	vec3 rgbRefract = vec3(0.0f,0.0f,0.0f);
	
	if(bUseReflect)
	{
		// Calculate direction from vertex to the eye
		vec3 viewVector = eyeLocation.xyz - vertPosWorld.xyz;
		viewVector = normalize(viewVector);
		
		// Calculate the reflection vector. This is literally the vector reflected.
		// The way it works is that we need to get the vector that is on the "other side" of the
		// normal of the current vertex, giving us the perfect reflected vector.
		vec3 reflectionVector = reflect(viewVector, vertNormal.xyz);
		rgbReflect = texture(textureSkyBoxDay, reflectionVector).rgb;
	}
	if(bUseRefract)
	{
		// Calculate direction from vertex to the eye
		vec3 viewVector = eyeLocation.xyz - vertPosWorld.xyz;
		viewVector = normalize(viewVector);
		
		// Calcualte the refraction vector
		vec3 refractionVector = refract(viewVector, vertNormal.xyz, 1.10);
		rgbRefract = texture(textureSkyBoxDay, refractionVector).rgb;
	}
	
	
	if ( useVertexColour )
	{
		materialDiffuse = color;
	}
	else if(bNoTextures == false)
	{
		/*if(waterTextureID == 1.0f)
		{
			uv.x = vertUV_x2.s + waterOffset.x;
			uv.y = vertUV_x2.t + waterOffset.y;
		}
		else if(waterTextureID == 2.0f)
		{
			uv.x = vertUV_x2.s + waterOffset.z;
			uv.y = vertUV_x2.t + waterOffset.w;
		}
		else if(waterTextureID == 0.0f)
		{
			uv.x = vertUV_x2.s;
			uv.y = vertUV_x2.t;
		}*/
		
		vec4 uv = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		uv.x = vertUV_x2.s;
		uv.y = vertUV_x2.t;
	
		// Add textures as vec4
		vec4 tex0Col = texture(texture00, uv.st).rgba;
		vec4 tex1Col = texture(texture01, uv.st).rgba;
		vec4 tex2Col = texture(texture02, uv.st).rgba;
		vec4 tex3Col = texture(texture03, uv.st).rgba;
		vec4 tex4Col = texture(texture04, uv.st).rgba;
		vec4 tex5Col = texture(texture05, uv.st).rgba;
		vec4 tex6Col = texture(texture06, uv.st).rgba;
		vec4 tex7Col = texture(texture07, uv.st).rgba;
		
		// Apply textures according to the weight
		materialDiffuse = objectDiffuse
						+ (tex0Col * texBlendWeights[0].x) 
						+ (tex1Col * texBlendWeights[0].y)
						+ (tex2Col * texBlendWeights[0].z)
						+ (tex3Col * texBlendWeights[0].w)
						+ (tex4Col * texBlendWeights[1].x)
						+ (tex5Col * texBlendWeights[1].y)
						+ (tex6Col * texBlendWeights[1].z)
						+ (tex7Col * texBlendWeights[1].w);
	}
	
	float alphaMapRatio = 1.0f;
	// Check for discard alpha. It could be from the map, or straight from the textures
	if(alphaMapDiscardOverride < 1.0f)
	{
		// We have discard coming from the alpha map
		if(alphaMapDiscardOverride < blackThreshold)
		{
			// Black value is too small, so it's too black
			// so it won't draw this pixel
			discard;
		}
		else
		{
			alphaMapRatio = alphaMapDiscardOverride * alphaTextureOverride;
		}
	}
	else if(alphaTextureOverride < 1.0f || blackThreshold != 0.0f)
	{
		float pixelBlackAndWhite = (0.3f * materialDiffuse.r) + (0.59f * materialDiffuse.g) + (0.11f * materialDiffuse.b);

		if(pixelBlackAndWhite < blackThreshold)
		{
			discard;
			return;
		}	
		
		materialDiffuse.a = pixelBlackAndWhite * alphaTextureOverride;
	}
	
	
	// Is this object affected by lighting?
	if(!isLightAffecting || bNoLighting == true)
	{
		// Just return the natural colour
		if(bNoTextures == true)
		{
			finalOutputColour = objectDiffuse;
		}
		else
		{
			finalOutputColour = materialDiffuse;
			finalOutputNormal.rgb = vertNormal.xyz;
			finalOutputNormal.a = 1.0f;
		}
		
		return;
	}
	
	// Calculate angle between normal and light position
	vec3 norm = normalize(vertNormal.xyz);
	vec4 finalObjectColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	for(int i = 0; i < numberOfLights; i++)
	{
		// Is the light on?
		if(theLights[i].param2.x == 0.0f)
		{
			// Off
			continue;
		}
		
		if(bUseCelShade)
		{
			// The cel-shaded shader is done by checking the cosine of the angle
			// between the light's direction to the vertex and the vertex normal.
			// Depending on the angle, the colour is adjusted to more absolute values,
			// giving the colours a clear transition in tones.
			
			// Get the light direction and the normal
			vec3 lightToVertex = theLights[i].position.xyz - vertPosWorld.xyz; // Direction vector
			vec3 lightVector = normalize(lightToVertex); // Normalized direction vector
			vec3 vertexNormal = normalize(vertNormal.xyz);
			
			// Calculate the cosine angle
			float dotValue = dot(lightVector, vertexNormal);
			float cosine = dotValue;
			cosine = max(0.0f, cosine);
				
			vec3 celShadeContrib = vec3(0.0f, 0.0f, 0.0f);
			// Use angle to create step colours
			if(cosine > 0.9f)
			{
				celShadeContrib = vec3(1.0f, 1.0f, 1.0f);
			}
			else if(cosine > 0.5f)
			{
				celShadeContrib = vec3(0.5f, 0.5f, 0.5f);
			}
			else if(cosine > 0.1f)
			{
				celShadeContrib = vec3(0.1f, 0.1f, 0.1f);
			}
			else
			{
				celShadeContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			
			finalObjectColour.rgb += (materialDiffuse.rgb * theLights[i].diffuse.rgb * celShadeContrib);
			
			finalOutputColour.rgb = finalObjectColour.rgb;
			finalOutputColour.a = 1.0f;
			
			continue;
		}
		
		// Cast the type of light
		int sLightTypeAsInt = int(theLights[i].param1.x); // Param x tells the type of light, but comes as a float
		
		// Begin with DIRECTIONAL light
		if(sLightTypeAsInt == sLightType_DIRECTIONAL) // Would be = 1
		{
			// Directional lights are like the sun, they don't have attenuation
			// It's the cheapest kind of light to calculate
			// Usually there's only 1 in a game, but more could create interesting effects
			
			vec3 lightDirectionalContrib = theLights[i].diffuse.rgb;
			// Get the dot product of the light and the normal, just like the point light
			float directionalDotProduct = dot(-theLights[i].direction.xyz, normalize(norm.xyz));
			directionalDotProduct = max(0.0f, directionalDotProduct); // Between 0 and 1
			lightDirectionalContrib *= directionalDotProduct;
			
			finalObjectColour.rgb += (materialDiffuse.rgb * theLights[i].diffuse.rgb * lightDirectionalContrib);
			finalOutputColour.rgb = finalObjectColour.rgb;
			finalOutputColour.a = objectDiffuse.a * alphaMapRatio;
			
			// Also output the normal to colour #1
			finalOutputNormal.rgb = vertNormal.xyz;
			finalOutputNormal.r = 1.0f;
			finalOutputNormal.a = 1.0f;
			continue;
		}
		
		// The spot lights use the same calculations, 
		// so begin by doing point like first, since calculations will be similar
		
		// How much the light contributes
		vec3 lightToVertex = theLights[i].position.xyz - vertPosWorld.xyz; // Direction vector
		float distanceToLight = length(lightToVertex);
		vec3 lightVector = normalize(lightToVertex); // Normalized direction vector
		float dotProduct = dot(lightVector, norm.xyz);
		dotProduct = max(0.0f, dotProduct); // Only need >= 0. Otherwise, we're behind the object
		vec3 lightDiffuseContrib = dotProduct * theLights[i].diffuse.rgb;
		
		// Specular
		vec3 lightSpecularContrib = vec3(0.0f);
		vec3 reflectVector = reflect(-lightVector, normalize(norm.xyz));
		vec3 eyeVector = normalize(eyeLocation.xyz - vertPosWorld.xyz);
		float objectSpecularPower = objectSpecular.w;
		lightSpecularContrib = pow(max(0.0f, dot(eyeVector, reflectVector)), objectSpecularPower) * objectSpecular.rgb;
		
		// Attenuation
		float attenuation = 1.0f / (theLights[i].atten.x + theLights[i].atten.y * distanceToLight + theLights[i].atten.z * distanceToLight * distanceToLight);
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		// Now check to see if it is a point light or a spot light
		if(sLightTypeAsInt == sLightType_SPOT) // Would be = 2
		{
			// Calculate light vector. Inverse of the previous one
			vec3 vertexToLight = vertPosWorld.xyz - theLights[i].position.xyz;
			vertexToLight = normalize(vertexToLight);
			
			// Now, we calculate the angle that each vertex is in relation to the direction the light is shining 
			float currentLightAngle = dot(vertexToLight.xyz, theLights[i].direction.xyz);
			currentLightAngle = max(0.0f, currentLightAngle); // Do not be negative in angles!
			
			// From param1, x = lightType, y = inner angle, z = outer angle, w = still unused
			float outerConeAngleCos = cos(radians(theLights[i].param1.z));
			float innerConeAngleCos = cos(radians(theLights[i].param1.y));
			
			// Is the vertex outside all angles?
			if(currentLightAngle < outerConeAngleCos)
			{
				// The vertex is outside the cones of light, 
				// because the dot results in 1 when the vector are
				// near, and 0 (meaning it get's lower) when they are farther apart.
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightAngle < innerConeAngleCos )
			{
				// The angle is between the cones of light
				// like in the penumbra of a shadow
				// This blends the brightness from full brightness
				// near the inner cone, to black near the outer cone
				float penumbraRatio = (currentLightAngle - outerConeAngleCos) / (innerConeAngleCos - outerConeAngleCos);
				
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
		}
		
		// Finalize
		finalObjectColour.rgb += (materialDiffuse.rgb * lightDiffuseContrib.rgb) + (materialSpecular.rgb * lightSpecularContrib.rgb);
	}
	
	finalOutputColour.rgb = finalObjectColour.rgb;
	finalOutputColour.a = objectDiffuse.a * alphaMapRatio;
	// Also output the normal to colour #1
	finalOutputNormal.rgb = vertNormal.xyz;
	finalOutputNormal.a = 1.0f;
	
	if(isSelected)
	{
		finalOutputColour.rgb += vec3(0.2f, 0.2f, 0.0f);
		finalOutputColour.a = objectDiffuse.a * alphaMapRatio;
	}
	
	// Finally, add the final reflect and refract
	if(bUseReflect)
	{
		// Lower amount of colour
		finalOutputColour *= (1.0f - reflectionRate);
		// Add as reflective colour
		finalOutputColour.rgb += (reflectionRate * rgbReflect);
	}
	if(bUseRefract)
	{
		// Lower amount of colour
		finalOutputColour *= (1.0f - refractionRate);
		// Add as reflective colour
		finalOutputColour.rgb += (refractionRate * rgbRefract);
	}
}


