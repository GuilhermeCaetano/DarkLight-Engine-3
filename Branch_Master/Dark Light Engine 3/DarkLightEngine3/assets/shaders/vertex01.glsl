#version 420 
// vertex01.glsl

//uniform mat4 MVP;		
uniform mat4 matModelInvTrans;   // inverse(transpose(matModel))
uniform mat4 matModel;			// M
uniform mat4 matView;			// V
uniform mat4 matProj;			// P

// Normal Mesh
in vec4 vColour;		// rgb   	was "attribute"
in vec4 vPosition;		// xyz		was "attribute"
in vec4 vNormal;        // nx ny nz
in vec4 vUV_x2; 		// texture coordinates

// Skinned Mesh
in vec4 vTanXYZ;		// Tangent to the surface
in vec4 vBiNormXYZ;		// bi-normal (or bi-tangent) to the surface
in vec4 vBoneID;		// really passing it as 4 values
in vec4 vBoneWeight;	// Vertex bone weight

// Height Map
uniform bool bUseHeightMap;
uniform bool bFlatten;
uniform sampler2D heightMapTexture;
uniform float heightMapRatio;

// Alpha Map
uniform bool bUseAlphaMap;
uniform sampler2D alphaMapTexture;

out vec4 color;			// exit to fragment
out vec4 vertPosWorld;  // world space
out vec4 vertNormal;    // model space
out vec4 vertUV_x2;  	// exit to fragment
out vec4 vertTanXYZ;	// Tangent to the surface
out vec4 vertBiNormXYZ;	// bi-normal (or bi-tangent) to the surface

out float blackThreshold;
out float alphaTextureOverride;
out float alphaMapDiscardOverride;

// Skinned mesh
const int MAXNUMBEROFBONES = 100;
uniform mat4 bones[MAXNUMBEROFBONES];
// Pass the acutal number you are using to control how often this loops
uniform int numBonesUsed;			
uniform bool bIsSkinnedMesh;	// True to do the skinned mesh

void main()
{
	// Must copy values, can't directly alter in variables
	vec3 posTemp = vPosition.xyz;
	float alphaTemp = 1.0f;
	
	// Check for the height map
	if(bUseHeightMap)
	{
		// Since this is black and white (like a noise), we only need to check for 1 colour between 0 and 1
		float height = texture(heightMapTexture, vUV_x2.st).r;
		height = height * heightMapRatio; // Multiply by the ratio we want
		
		// Do we want to flatten the mesh or adjust it?
		if(bFlatten) // Flatten
		{
			posTemp.y = 0.0f;
			posTemp.y = height;
		}
		else
		{
			posTemp.y += height; // Adjust the mesh
		}
	}
	
	// Check for alpha map
	if(bUseAlphaMap)
	{
		// Note "greyscale", that humans see, isn't even over the colours. 
		// (google "RGB to greyscale" to get this equation:)
		vec4 alphaTex = texture( alphaMapTexture, vUV_x2.st ).rgba;
		float pixelBlackAndWhite = (0.3f * alphaTex.r) + (0.59f * alphaTex.g) + (0.11f * alphaTex.b);
		alphaTemp = pixelBlackAndWhite;
	}
	else
	{
		alphaTemp = 1.0f;
	}
	
	// Will go out to the fragment
    color = vColour;
	vertUV_x2.xy = vUV_x2.xy;
	alphaMapDiscardOverride = alphaTemp;
	
	// Also pass the bi-tangent (bi-normal) and tangent to fragment
	vertTanXYZ = vTanXYZ;			// Tangent to the surface
	vertBiNormXYZ = vBiNormXYZ;		// bi-normal (or bi-tangent) to the surface
	
	// Check if this is a vertex from a skinned mesh
	if(bIsSkinnedMesh)
	{
		//*****************************************************************
		// Before I do the full MVP (screen space) calculation, 
		// I'm going to adjust where the vertex is based on the 
		// the bone locations
		
		// We need to take into account the bone transformation for this vertex,
		// based on all the bones that affect it and the percentage(weight) that they
		// affect this particular vertex.
		
		mat4 BoneTransform = bones[ int(vBoneID[0]) ] * vBoneWeight[0];
			BoneTransform += bones[ int(vBoneID[1]) ] * vBoneWeight[1];
			BoneTransform += bones[ int(vBoneID[2]) ] * vBoneWeight[2];
			BoneTransform += bones[ int(vBoneID[3]) ] * vBoneWeight[3];
				
		vec4 vertPositionFromBones = BoneTransform * vec4(posTemp.xyz, 1.0);
		//*****************************************************************
	
		mat4 MVP = matProj * matView * matModel;
		gl_Position = MVP * vertPositionFromBones;			// Apply the bone transformation to the MVP
		
		vertPosWorld = matModel * vec4(posTemp, 1.0);
		
		// Transforms the normal into "world space"
		// Remove all scaling and transformation from model
		// Leaving ONLY rotation... 
		
		// This only leaves rotation (translation and scale are removed)
		mat4 matBoneTransform_InTrans = inverse(transpose(BoneTransform));
		
		vec4 vNormBone = matBoneTransform_InTrans * vec4(normalize(vNormal.xyz), 1.0f);
		
		vertNormal = matModelInvTrans * vNormBone;
		
		// And then you do the same for the binormal and bitangent
		vec4 vTanXYZ_Bone = matBoneTransform_InTrans * vTanXYZ;
		vertTanXYZ = matModelInvTrans * vTanXYZ_Bone;
		
		vec4 vBiNormXYZ_Bone = matBoneTransform_InTrans * vBiNormXYZ;
		vertBiNormXYZ = matModelInvTrans * vBiNormXYZ_Bone;
		
		// Debug: make green if this flag is set
		//color.rgb = vec3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		// Pass as a regular mesh
		
		// These are actually calculated backwards, so MVP is actually PVM
		mat4 MVP = matProj * matView * matModel;
		gl_Position = MVP * vec4(posTemp, 1.0);
		
		vertPosWorld = matModel * vec4(posTemp, 1.0);
		
		// Transforms the normal into "world space"
		// Remove all scaling and transformation from model
		// Leaving ONLY rotation... 
		// This works, because multiplying a matrix by it's inverse
		// leaves us with a one matrix, which means, no alterations matrix
		vertNormal = matModelInvTrans * vec4(normalize(vNormal.xyz), 1.0f);
	}
}



