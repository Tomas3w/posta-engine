#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvposition;
layout (location = 3) in vec4 bones_weights; // and indices

out vec2 UV;
out vec3 in_normal;
out vec3 frag_position;

uniform mat4 model;
uniform mat3 normal_model;
uniform mat4 projection_view;

uniform mat4 bones_matrices[21];

void main()
{
	vec4 final_position = vec4(position, 1.0f);
	for (int i = 0; i < 4; i++)
	{
		if (bones_weights[i] > 0)
			final_position += bones_matrices[int(bones_weights[i])] * vec4(position, 1.0f) * (bones_weights[i] - int(bones_weights[i]));
	}

	UV = uvposition;
	vec4 world = model * final_position;
	frag_position = vec3(world);

	in_normal = normal_model * normal;
	
	gl_Position = projection_view * world;
}
