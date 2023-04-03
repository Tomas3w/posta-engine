#version 330 core

out vec4 color;

in vec2 UV;
in vec3 in_normal;
in vec3 frag_position;

uniform vec4 global_color;

uniform sampler2D intexture;
uniform sampler2D depthtexture;

uniform mat4 sun_light_projection_view;
uniform vec3 sun_light_position;
uniform vec3 sun_light_direction;
uniform float sun_depth_distance_ratio;

float get_distance_to_plane(vec3 pos, vec3 plane_pos, vec3 plane_dir)
{
	vec3 to_plane = plane_pos - pos;
	float len = length(to_plane);
	vec3 unit_to_plane = to_plane / len;
	vec3 inverse_plane_dir = -plane_dir;
	
	return dot(unit_to_plane, inverse_plane_dir) * len;// / 5.0f;
}

float get_distance_in_depth_map()
{
	vec4 frag_view_space = sun_light_projection_view * vec4(frag_position, 1.0f);
	if (frag_view_space.x > 1.0f || frag_view_space.y > 1.0f || frag_view_space.z > 1.0f ||
		frag_view_space.x < -1.0f || frag_view_space.y < -1.0f || frag_view_space.z < -1.0f)
		return 1000; // just big enough number
	vec2 uv = vec2((frag_view_space.x + 1) / 2.0f, (frag_view_space.y + 1) / 2.0f);
	return texture(depthtexture, uv).r * sun_depth_distance_ratio;
}

bool shadow_gradient()
{
	float distance_to_light_plane = get_distance_to_plane(frag_position, sun_light_position, sun_light_direction);
	float distance_in_depth_map = get_distance_in_depth_map();

	float bias = 0;//(1 - max(dot(in_normal, -sun_light_direction), 0)) * 0.5;
	if (dot(in_normal, sun_light_direction) > 0)
		return false;
	if (distance_in_depth_map < distance_to_light_plane - bias)
		return true;
	return false;
}

void main()
{
	vec4 tex = texture(intexture, UV);
	//color = vec4((dot(-in_normal, normalize(vec3(-1, -2, -1))) + 1) / 2 * tex.rgb, tex.a) * global_color;
	vec4 tex2 = tex * global_color;

	vec3 no_direct_light_color = tex2.rgb * vec3(min(max(dot(vec3(0, -1, 0), sun_light_direction), 0), 0.4f));
	if (shadow_gradient())
		color = vec4(mix(no_direct_light_color, tex2.rgb, 0.2f), tex2.a);
	else
		color = vec4(mix(no_direct_light_color, tex2.rgb, max(dot(-in_normal, sun_light_direction), 0.2)), tex2.a);
}
