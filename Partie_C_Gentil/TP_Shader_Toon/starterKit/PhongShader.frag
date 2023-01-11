#version 450

uniform vec3 cameraPosition;

uniform vec3 objectColor;
uniform vec3 lightPosition;
uniform vec3 lightIntensities;

uniform sampler2D myTextureSampler;
uniform mat4 MODEL;
uniform mat4 VIEW;

in vec3 fragPosition;
in vec3 fragNormal;
out vec4 finalColor;

void main() {
    vec3 norm = normalize(transpose(inverse(mat3(MODEL))) * fragNormal);
    vec3 lightDir = normalize(lightPosition - fragPosition);  
    vec3 cameraDir = normalize(cameraPosition - fragPosition);
    
    float diff = max(dot(norm, lightDir), 0.0);     

    vec4 color;
	if (diff > 0.95)
		color = vec4(1.0,0.5,0.5,1.0);
	else if (diff > 0.5)
		color = vec4(0.6,0.3,0.3,1.0);
	else if (diff > 0.25)
		color = vec4(0.4,0.2,0.2,1.0);
	else
		color = vec4(0.2,0.1,0.1,1.0);

    if (dot(norm, cameraDir) < 0.65)
        color = vec4(0);

    finalColor = color;
}
