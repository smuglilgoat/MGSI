#version 450

uniform mat4 MVP;
uniform mat4 MODEL;

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_uv;
layout(location = 3) in vec3 a_normal;

uniform float time;


out vec3 position;
out vec3 normal;
out vec2 uv;

float x0 = -2;
float x1 = 2;

float max_angle = 4.;

float r(float x){
	if(x < x0){
		return 0.;
	}
	else if(x >= x0 && x <= x1){
		return (x-x0)/(x1-x0)*max_angle;
	}
	else if(x > x1){
		return max_angle;
	}
}

float r_prime(float x){
	if(x < x0){
		return 0.;
	}
	else if(x >= x0 && x <= x1){
		return (x0)/(x1-x0)*max_angle;
	}
	else if(x > x1){
		return 0.;
	}
}

void main(){
	float x = a_position.x;
	float y = a_position.y;
	float z = a_position.z;
	mat3 twist = mat3(
		1, 0, 0,
		0, cos(r(x)), sin(r(x)),
		0, -sin(r(x)), cos(r(x))
	);
	mat3 twist_jacobian = mat3(
		1, -y*r_prime(x)*sin(r(x)) - z*r_prime(x)*cos(r(x)), y*cos(r(x))*r_prime(x) - z*sin(r(x))*r_prime(x),
		0, cos(r(x)), sin(r(x)),
		0, -sin(r(x)), cos(r(x))

		// 1, 0, 0,
		// -y*r_prime(x)*sin(r(x)) - z*r_prime(x)*cos(r(x)), cos(r(x)), -sin(r(x)),
		// -y*r_prime(x)*cos(r(x)) - z*r_prime(x)*cos(r(x)), sin(r(x)), cos(r(x))
	);
	// normal = transpose(inverse(mat3(MODEL)))*a_normal;
	normal = transpose(inverse(twist_jacobian))*a_normal;
	gl_Position = MVP*vec4(twist*a_position, 1);
	position = mat3(MODEL)*twist*a_position;
	uv = a_uv;
}
