#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _Texture;
uniform sampler2D _CharTexture;

void main(){
	vec4 col = vec4(0.5, 0.0, 0.5, 1.0);
	FragColor =  texture(_CharTexture,UV);
}