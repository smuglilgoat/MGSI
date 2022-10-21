#version 450


uniform mat4 MVP;
uniform mat4 MODEL;

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in vec3 a_normal;

uniform float materialShininess;
uniform vec3 materialSpecularColor;

struct light_struct{
	float materialShininess;
	vec3 materialSpecularColor;
	vec3 position;
	vec3 intensities;
	float attenuation;
	float ambientCoefficient;
};

uniform light_struct light;

out float materialShininess_;
out vec3 materialSpecularColor_;
out vec3 position;
out vec3 normal;
out vec2 uv;
out vec3 light_position;
out vec3 light_intensities;
out float light_attenuation;
out float light_ambientCoefficient;

void main(){
	gl_Position = MVP*vec4(a_position, 1);
	position = mat3(MODEL)*a_position;
	normal = a_normal;
	uv = a_uv;
	materialShininess_ = materialShininess;
	materialSpecularColor_ = materialSpecularColor;
	light_position = light.position;
	light_intensities = light.intensities;
	light_attenuation = light.attenuation;
	light_ambientCoefficient = light.ambientCoefficient;
}
