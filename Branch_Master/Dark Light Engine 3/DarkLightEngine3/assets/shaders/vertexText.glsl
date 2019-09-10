#version 420 
// vertex01.glsl

// Text
uniform bool bIsText;
attribute vec4 tCoord;
varying vec2 tPos;
out float isText;

void main()
{
	if(bIsText == true)
	{
		isText = 1.0f;
		gl_Position = vec4(tCoord.xy, 0, 1);
		tPos = tCoord.zw;
		return;
	}
}



