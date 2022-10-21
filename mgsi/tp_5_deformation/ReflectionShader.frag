#version 450

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
	float ambient = 0.4;
	float diffuse = max(dot(normalize(normal), normalize(light.position)), 0.0);
	vec3 view_direction = normalize(cameraPosition-position);
	vec3 light_reflection = reflect(-light.position, normal);
	float specular = pow(max(dot(normalize(view_direction), normalize(light_reflection)), 0.0), 4)/2.;
	float light = ambient+diffuse+specular;
	vec3 color = light*vec3(0.7, 0.3, 0.4);
	finalColor = vec4(color, 1.0);

	// finalColor = vec4(vec3(dot(normal, vec3(0, 1, 0))), 1.0);

	// vec3 I = normalize(position-cameraPosition);
	// vec3 R = reflect(I, normalize(normal));
	// finalColor = vec4(texture(cubemap, R).rgb, 1.0);
}
