#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}fs_in;

struct Light
{
	vec3 position;
	vec3 color;
};
#define MAX_LIGHTS 4
uniform Light _Lights[MAX_LIGHTS];
uniform sampler2D _Texture;

void main(){
	vec3 normal = normalize(fs_in.WorldNormal);
	//vec2 lighting = _Texture * max(dot(normal, fs_in.WorldPosition), 0);
	FragColor = texture(_Texture,fs_in.UV) * max(dot(normal, fs_in.WorldPosition), 0);
}