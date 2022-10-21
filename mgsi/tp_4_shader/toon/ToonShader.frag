#version 450

uniform vec3 cameraPosition;
uniform mat4 MODEL;

uniform sampler2D myTextureSampler;

in float materialShininess_;
in vec3 materialSpecularColor_;
in vec3 position;
in vec3 normal;
in vec2 uv;
in vec3 light_position;
in vec3 light_intensities;
in float light_attenuation;
in float light_ambientCoefficient;

out vec4 finalColor;

void main() {
	vec3 norm = normalize(transpose(inverse(mat3(MODEL)))*normal);  
	float ambient = light_ambientCoefficient;
	float diffuse = max(dot(norm, light_position), 0.0);
	vec3 view_direction = normalize(cameraPosition-position);
	vec3 light_reflection = reflect(-light_position, norm);
	float specular = pow(max(dot(view_direction, light_reflection), 0.0), 512);
	float light = ambient+diffuse+specular;
	if(light >= 0.9999) light = 1.;
	if(light < 0.9999 && light >= 0.5) light = 0.8;
	if(light < 0.5  && light >= 0.2) light = 0.5;
	if(light < 0.2) light = 0.3;
	vec3 color = vec3(1);
	if(light < 1.) color = light*vec3(1, 0.2, 0);
	if(dot(norm, view_direction) < 0.4) color = vec3(0);
	finalColor = vec4(color, 1.);


	//vec3 norm = mat3(transpose(inverse(model)))*normal;  
	// vec3 norm = normalize(transpose(inverse(mat3(MODEL)))*normal);  
	// vec3 view_direction = normalize(cameraPosition-position);
	// float diffuse = max(dot(norm, light_position), 0.0);
	// vec3 color = mix(vec3(0, 0, 1), vec3(1, 1, 0), 1-diffuse)*0.9;
	// if(dot(norm, view_direction) < 0.3) color = vec3(0);
	// finalColor = vec4(color, 1.);	
}
