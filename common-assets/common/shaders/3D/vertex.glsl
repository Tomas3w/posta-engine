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
uniform bool use_bones;

uniform mat4 bones_matrices[101];

void main()
{
	vec4 final_position = vec4(0.0f);
	mat4 final_matrix = mat4(1.0f);
	if (use_bones)
	{
		final_matrix[0] = vec4(0.0f);
		final_matrix[1] = vec4(0.0f);
		final_matrix[2] = vec4(0.0f);
		final_matrix[3] = vec4(0.0f);
		for (int i = 0; i < 4; i++)
		{
			if (bones_weights[i] > 0)// && (bones_matrices[0] != bones_matrices[0] * 1))
				final_matrix += bones_matrices[int(floor(bones_weights[i]))] * (bones_weights[i] - floor(bones_weights[i]));
				//final_position += bones_matrices[int(floor(bones_weights[i]))] * vec4(position, 1.0f) * (bones_weights[i] - floor(bones_weights[i]));
		}
		final_position = final_matrix * vec4(position, 1.0f);

	}
	else
		final_position = vec4(position, 1.0f);

	UV = uvposition;
	vec4 world = model * final_position;
	frag_position = vec3(world);

	in_normal = normal_model * transpose(inverse(mat3(final_matrix))) * normal;
	
	vec4 glpos = projection_view * world;
	gl_Position = glpos;
}
