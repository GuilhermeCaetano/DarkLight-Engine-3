#version 420

// vertexDebugLine.glsl

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;

in vec4 vPosition;
in vec4 vColour;

out vec4 gVertColour;

void main()
{
	gl_Position = vPosition;
	gVertColour = vColour;
}