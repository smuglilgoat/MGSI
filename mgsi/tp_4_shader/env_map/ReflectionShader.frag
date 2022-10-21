#version 330 core

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform vec3 cameraPosition;
uniform mat4 MVP;
uniform mat4 MODEL;
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

out vec4 finalColor;



in vec3 textureDir;
uniform samplerCube cubemap;

void main() {
	vec3 norm = normalize(transpose(inverse(mat3(MODEL)))*normal);  
	float ambient = light.ambientCoefficient;
	float diffuse = max(dot(norm, light.position), 0.0);
	vec3 view_direction = normalize(cameraPosition-position);
	vec3 light_reflection = reflect(-light.position, norm);
	float specular = pow(max(dot(view_direction, light_reflection), 0.0), 512);
	float light = ambient+diffuse+specular;
	if(light >= 0.9999) light = 1.;
	if(light < 0.9999 && light >= 0.5) light = 0.8;
	if(light < 0.5  && light >= 0.2) light = 0.5;
	if(light < 0.2) light = 0.3;
	// vec3 color = vec3(1);
	// if(light < 1.) color = light*vec3(1, 0.2, 0);
	vec3 color = light*vec3(0.7, 0.3, 0.4);
	// if(dot(norm, view_direction) < 0.4) color = vec3(0);


	vec3 I = normalize(position-cameraPosition);
    vec3 R = reflect(I, normalize(normal));
    // vec3 R = refract(I, normalize(normal), 0.9);
    finalColor = vec4(texture(cubemap, R).rgb, 1.0);
}
