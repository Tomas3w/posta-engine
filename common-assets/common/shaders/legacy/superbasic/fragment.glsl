#version 330 core

// uniform sampler2D intexture;

out vec4 color;

in vec2 UV;
// in vec3 in_normal;
in vec3 frag_position;

void main()
{
	color = vec4(1, 1, 1, 1);//texture(intexture, UV);
	// vec4 texture_color = texture(intexture, UV);

	// float ambient_strength = 0.01;
	// vec3 ambient = ambient_strength * ambient_color;

	// float gamma = 2.2;
	// vec3 result = pow(texture_color.rgb, vec3(1.0 / gamma));

	// vec4 shadow_pos = shadow_projection_view * vec4(frag_position, 1);
	// vec2 shadow_kno = vec2(shadow_pos) / 2 + vec2(0.5, 0.5);
	// color = vec4(result, texture_color.a);
}
