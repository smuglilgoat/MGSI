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
    float NdotL;

    vec3 CoolColor = vec3(0, 0, 0.6);
    vec3 WarmColor = vec3(0.6, 0.6, 0);

    NdotL = (dot(lightDir, norm) + 1.0) * 0.5;
    vec3 kcool    = min(CoolColor + objectColor, 1.0);
    vec3 kwarm    = min(WarmColor + objectColor, 1.0); 
    vec3 kfinal   = mix(kcool, kwarm, NdotL);

    finalColor = vec4(kfinal, 1.0);
    
    // if (dot(norm, cameraDir) < 0.2)
    //     finalColor = vec4(0);
}
