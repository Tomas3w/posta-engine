#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvposition;

out vec2 UV;
// out vec3 in_normal;
out vec3 frag_position;

uniform mat4 model;
// uniform mat3 normal_model;
uniform mat4 projection_view;

void main()
{
	UV = uvposition;
	// in_normal = normal_model * normal;
	
	vec4 world = model * vec4(position, 1.0);
	frag_position = vec3(world);
	
	gl_Position = projection_view * world;
}
