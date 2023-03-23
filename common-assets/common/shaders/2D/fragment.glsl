#version 330 core

out vec4 color;

in vec2 UV;

uniform vec4 global_color;
uniform sampler2D intexture;

void main()
{
	vec4 tex = texture(intexture, UV);
	color = tex * global_color;
}
