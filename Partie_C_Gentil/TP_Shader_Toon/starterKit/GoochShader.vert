#version 450


uniform mat4 MVP;
uniform mat4 MODEL;

layout(location = 0) in vec3 position; // le location permet de dire de quel flux/canal on récupère les données (doit être en accord avec le location du code opengl)
layout(location = 3) in vec3 aNormal;

out vec3 normal;
out vec3 fragPosition;
out vec3 fragNormal;

void main(){
	gl_Position = MVP * vec4(position,1.0);
    fragPosition = vec3(MODEL * vec4(position, 1.0));
    fragNormal = aNormal;
}


