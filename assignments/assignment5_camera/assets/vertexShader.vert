#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

out vec3 Normal;
uniform mat4 _Model;

void main(){
	Normal = vNormal;

	// Note to self:
	// Multiply by ViewMatrix (camera.cpp) to get to view, then multiply by ProjectionMatrix (camera.cpp) to get to clip. 
	// Below the program already gets from model to world. 
	// Just pass the View and Projection matrices as uniforms. Keep in mind that these will require a Camera object.

	gl_Position = _Model * vec4(vPos,1.0);
}