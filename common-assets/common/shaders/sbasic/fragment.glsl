#version 330 core

out vec4 color;

in vec2 UV;
in vec3 in_normal;
in vec3 frag_position;

uniform vec3 global_color;

uniform sampler2D intexture;

float rand(vec3 co){
	return fract(sin(dot(co, vec3(12.9898, 78.233, 32.4285))) * 43758.5453);
}

void main()
{
	// float a = rand(in_normal);
	// float b = rand(in_normal + vec3(0.03152309393777675, 0.4763706696270824, 0.5907916637870673));
	// float c = rand(in_normal + vec3(0.08427185271166027, 0.20130414652706752, 0.7429966004593289));
	
	// vec3 pos = frag_position * 10.0f;
	// pos = vec3(floor(pos.x), floor(pos.y), floor(pos.z)) / 10.0f;
	// vec3 ka = vec3(rand(pos), rand(pos + vec3(1, 1, 1)), rand(pos + vec3(2, 2, 2)))/4;

	vec4 tex = texture(intexture, UV);// * dot(in_normal, in_normal) / dot(in_normal, in_normal) * dot(global_color, global_color) / dot(global_color, global_color);
	color = vec4((dot(-in_normal, normalize(vec3(-1, -2, -1))) + 1) / 2 * tex.rgb * global_color, tex.a);
	// color = tex;
} 
