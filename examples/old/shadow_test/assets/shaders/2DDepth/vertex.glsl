#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvposition;

out vec2 UV;
uniform mat4 model;

void main()
{
	UV = uvposition;
	gl_Position = model * vec4(position, 1.0f);
}
