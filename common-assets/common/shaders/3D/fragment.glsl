#version 330 core

out vec4 color;

in vec2 UV;
in vec3 in_normal;
in vec3 frag_position;

uniform vec4 global_color;

uniform sampler2D intexture;

void main()
{
	vec4 tex = texture(intexture, UV);
	color = vec4((dot(-in_normal, normalize(vec3(-1, -2, -1))) + 1) / 2 * tex.rgb, tex.a) * global_color;
}
