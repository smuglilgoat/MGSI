#version 450
uniform mat4 MVP;
uniform mat4 MODEL;

layout(location = 0) in vec3 aPosition; // le location permet de dire de quel flux/canal on récupère les données (doit être en accord avec le location du code opengl)
layout(location = 3) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

void main()
{
    Normal = mat3(transpose(inverse(MODEL))) * aNormal;
    Position = vec3(MODEL * vec4(aPosition, 1.0));
    gl_Position = MVP * vec4(Position, 1.0);
} 


