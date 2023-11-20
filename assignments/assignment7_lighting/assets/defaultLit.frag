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
	int enable;
};

#define MAX_LIGHTS 4
uniform Light _Lights[MAX_LIGHTS];
uniform sampler2D _Texture;

uniform vec3 _ViewPos;

uniform vec3 _ambientColor;			// ambient light color
uniform float _ambientK;		// ambient light intensity
uniform float _diffuseK;		// diffuse intensity
uniform float _specularK;		// specular intensity
uniform float _shininess;		// shininess

// Function prototypes

float dotProduct(vec3 a, vec3 b);
float ambient();
float diffuse(Light light);
float specular(Light light);
float calcLight(Light light);

void main(){
	float light = 0.0;
	vec3 color = vec3(0, 0, 0);
	
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (_Lights[i].enable == 1)
		{
			color += _Lights[i].color * calcLight(_Lights[i]);
		}
	}

	FragColor = texture(_Texture,fs_in.UV) * vec4(color, 1);
}

float dotProduct(vec3 a, vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float ambient()
{
	return _ambientColor * _ambientK;
}

float diffuse(Light light)
{
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 incidence = normalize(light.position - fs_in.WorldPosition);
	return _diffuseK * max(dotProduct(normal, incidence), 0);
}

float specular(Light light)
{
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 incidence = normalize(light.position - fs_in.WorldPosition);
	vec3 viewDirection = normalize(_ViewPos - fs_in.WorldPosition);

	vec3 halfVector = normalize(incidence + viewDirection);
	
	return _specularK * pow(max(dotProduct(halfVector, normal), 0), _shininess);
}

float calcLight(Light light)
{
	return ambient() + diffuse(light) + specular(light);
}