#shader vertex
#version 330 core
layout(location = 0) in vec4 position;

uniform mat4 u_MVP;
void main()
{
	gl_Position = vec4(position * u_MVP);
	//gl_Position = vec4(position);
};

#shader fragment
#version 330 core

out vec4 color;
uniform vec4 u_Color;
void main()
{
	color = u_Color;
};