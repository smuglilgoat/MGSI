#version 450

uniform vec3 cameraPosition;

uniform vec3 objectColor;
uniform vec3 lightPosition;
uniform vec3 lightIntensities;

uniform sampler2D myTextureSampler;
uniform mat4 MODEL;

in vec3 fragPosition;
in vec3 fragNormal;
out vec4 finalColor;

void main() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightIntensities;
    
    vec3 norm = normalize(transpose(inverse(mat3(MODEL))) * fragNormal);
    vec3 lightDir = normalize(lightPosition - fragPosition);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightIntensities;

    vec3 result = (ambient + diffuse) * objectColor;
    finalColor = vec4(result, 1.0);
}
