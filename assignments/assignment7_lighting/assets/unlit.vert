#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out vec3 normal;
out vec3 WorldPosition;

uniform mat4 _Model;
uniform mat4 _ViewProjection;

void main(){
	WorldPosition = mat3(_Model) * vPos;
	normal = transpose(inverse(mat3(_Model))) * vNormal;
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}
