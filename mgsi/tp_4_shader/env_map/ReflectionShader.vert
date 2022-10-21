#version 330 core

uniform mat4 MVP;
uniform mat4 MODEL;

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in vec3 a_normal;

uniform float time;


out vec3 position;
out vec3 normal;
out vec2 uv;

void main(){
	gl_Position = MVP*vec4(a_position, 1);
	position = mat3(MODEL)*a_position;
	normal = mat3(transpose(inverse(MODEL))) * a_normal;;
	uv = a_uv;
}
