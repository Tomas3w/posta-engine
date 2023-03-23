#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvposition;

out vec2 UV;
out vec3 in_normal;
out vec3 frag_position;

uniform mat4 model;
uniform mat4 projection_view;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
