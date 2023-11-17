#version 450

in vec3 normal;
in vec3 WorldPosition;

out vec4 FragColor;

uniform vec3 _Color;

void main(){
	vec3 _normal = normalize(normal);
	FragColor = vec4(_Color,1.0);
}
