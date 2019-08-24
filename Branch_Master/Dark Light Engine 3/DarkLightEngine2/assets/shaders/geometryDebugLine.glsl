#version 400

layout(lines)               in;
layout(line_strip)          out;
layout(max_vertices = 2)    out;

in vec4 gVertColour[];
out vec4 fVertColour;

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;

void main()
{
	mat4 MVP = mProjection * mView * mModel;

	gl_Position = MVP * gl_in[0].gl_Position;
	fVertColour = gVertColour[0];
	EmitVertex();

	gl_Position = MVP * gl_in[1].gl_Position;
	fVertColour = gVertColour[1];
	EmitVertex();

	EndPrimitive();
}