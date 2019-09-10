#version 420

// fragmentDebugLine.glsl

in vec4 fVertColour;

out vec4 outputColour;

void main()
{
	outputColour.rgb = fVertColour.rgb;
	outputColour.a = fVertColour.a;
}