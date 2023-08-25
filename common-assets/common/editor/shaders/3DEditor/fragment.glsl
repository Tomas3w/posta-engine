#version 330 core

out vec4 color;

in vec2 UV;
in vec3 in_normal;
in vec3 frag_position;

uniform vec4 global_color;

uniform sampler2D intexture;

uniform bool line;
uniform vec2 linei;
uniform vec2 linef;
uniform float line_width;
uniform float width;
uniform float height;

bool is_inside_point(vec2 uv)
{
	uv.y = 1 - uv.y;
	uv.x *= width;
	uv.y *= height;

	if (dot(normalize(linei - linef), normalize(linei - uv)) < 0)
		return false;
	if (dot(normalize(linef - linei), normalize(linef - uv)) < 0)
		return false;

	float d = dot(normalize(uv - linei), normalize(linef - linei));
	float dist = sin(acos(d)) * length(uv - linei);
	return dist < line_width;
}

void main()
{
	vec4 tex = texture(intexture, UV);
	if (line && !is_inside_point(UV))
		discard;
	color = tex * global_color;
}
