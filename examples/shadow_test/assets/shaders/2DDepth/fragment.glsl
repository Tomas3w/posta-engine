#version 330 core

out vec4 color;

in vec2 UV;

uniform vec4 global_color;
uniform sampler2D intexture;

uniform float far_plane;
uniform float near_plane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{
	vec4 tex = texture(intexture, UV);

	float val = tex.r;
	color = vec4(vec3(LinearizeDepth(val) / far_plane), 1.0) * global_color;
	color = vec4(vec3(val), 1) * global_color;
}
