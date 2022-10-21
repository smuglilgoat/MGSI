#version 450

uniform mat4 MVP;
uniform mat4 MODEL;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

out vec3 position;
out vec3 normal;

void main(){
	gl_Position = MVP*vec4(vec3(a_position), 1);
	normal = transpose(inverse(mat3(MODEL)))*a_normal;
}
