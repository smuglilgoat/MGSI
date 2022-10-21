#version 450

in vec3 position;
in vec3 normal;

uniform mat4 MVP;
uniform mat4 MODEL;

uniform vec3 camera_position;
uniform vec3 light_position;

out vec4 finalColor;

void main() {

	float ambient = 0.4;
	float diffuse = max(dot(normalize(normal), normalize(light_position)), 0.0);
	vec3 view_direction = normalize(camera_position-position);
	vec3 light_reflection = reflect(-light_position, normal);
	float specular = pow(max(dot(normalize(view_direction), normalize(light_reflection)), 0.0), 4);
	float light = ambient+diffuse;
	vec3 color = light*vec3(0.7, 0.3, 0.4);
	finalColor = vec4(color, 1.0);
}
