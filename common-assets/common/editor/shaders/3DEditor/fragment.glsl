#version 330 core

out vec4 color;

in vec2 UV;
in vec3 in_normal;
in vec3 frag_position;

uniform vec4 global_color;

uniform sampler2D intexture;

uniform bool outline;
uniform float width;
uniform float height;

bool is_in_outline_range(vec2 uv)
{
	float val = 10;
	float xval = val / width;
	float yval = val / height;
	return uv.x > xval && uv.y > yval && uv.x < 1 - xval && uv.y < 1 - yval;
}

void main()
{
	vec4 tex = texture(intexture, UV);
	if (outline && is_in_outline_range(UV))
		discard;
	color = tex * global_color;
}
