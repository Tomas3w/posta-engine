#version 330 core

out vec4 color;

in vec2 UV;
in vec3 in_normal;
in vec3 frag_position;

uniform vec4 global_color;

uniform sampler2D intexture;

uniform bool moving_object;
uniform bool outline;
uniform bool line;
uniform vec2 linei;
uniform vec2 linef;
uniform float line_width;
uniform float width;
uniform float height;
uniform float time;

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

vec2 cap_uv(vec2 uv)
{
	return clamp(uv, vec2(1.0f / width, 1.0f / height), vec2(1, 1) - vec2(1.0f / width, 1.0f / height));
}

float pixelate(float n, float factor)
{
	return round(n * factor) / factor;
}

bool on_border(vec2 pos)
{
	return pos.x < 2 || pos.y < 2 || pos.x > width - 2 || pos.y > height - 2;
}

void main()
{
	vec4 tex = texture(intexture, UV);
	if (line && !is_inside_point(UV))
		discard;
	vec2 texel_size = vec2(1 / width, 1 / height);
	if (outline)
	{
		vec2 pos = vec2(UV.x, 1 - UV.y);
		tex = vec4(0, 0, 0, 1);
		int times = 0;
		const int square_size = 2;
		vec4 original_pixel = texture(intexture, cap_uv(pos));
		for (int x = -square_size; x <= square_size; x++)
		{
			for (int y = -square_size; y <= square_size; y++)
			{
				vec4 tex_test = texture(intexture, cap_uv(pos + texel_size * vec2(x, y)));
				if (tex_test.r > tex.r)
					tex = tex_test;
				times++;
			}
		}
		//tex /= times;
		if (tex.r == 0 || (tex == original_pixel && !on_border(gl_FragCoord.xy)))
			discard;
	}
	if (moving_object)
	{
		float blue = (sin((floor(gl_FragCoord.y / 10) + floor(gl_FragCoord.x / 10) + time * 10) / 2.0f) + 1) / 2;
		blue = pixelate(blue, 3);
		tex = vec4(0, 0, blue, 1);
		tex.xyz *= max(0.3f, dot(in_normal, normalize(vec3(1, 1, 1))));
	}
	color = tex * global_color;
}
