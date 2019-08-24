#version 420
// fragment01.glsl

// Text
in float isText;
varying vec2 tPos;
uniform sampler2D tText;
uniform vec4 textColor;

out vec4 finalOutputColour;			// GL_COLOR_ATTACHMENT0

void main()
{
	if(isText == 1.0f)
	{
		finalOutputColour = vec4(1.0f, 1.0f, 1.0f, texture2D(tText, tPos).r) * textColor;
		return;
	}
}


