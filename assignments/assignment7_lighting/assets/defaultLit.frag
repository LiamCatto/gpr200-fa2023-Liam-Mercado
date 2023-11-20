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

uniform float _lightIntensity;		// light intensity
uniform vec3 _aColor;				// ambient light color
uniform float _aCoef;				// ambient light intensity
uniform float _shininess;			// shininess

uniform bool _Enable1; // unused
uniform bool _Enable2; // unused
uniform bool _Enable3; // unused
uniform bool _Enable4; // unused

// Function prototypes

float dotProduct(vec3 a, vec3 b);
float ambient();
float diffuse(Light light);
float specular(Light light);
float calcLight(Light light);
bool enableLights[4] = { _Enable1, _Enable2, _Enable3, _Enable4 }; // unused

void main(){
	float light = 0.0;
	vec4 color = vec4(0, 0, 0, 1);

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (_Lights[i].enable == 1)
		{
			color += vec4(_Lights[i].color, 1) * (calcLight(_Lights[i]) / 4);
		}
	}

	FragColor = texture(_Texture,fs_in.UV) + color;
}

float dotProduct(vec3 a, vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float ambient()
{
	return _aColor * _aCoef;
}

float diffuse(Light light)
{
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 incidence = normalize(light.position - fs_in.WorldPosition);
	return _lightIntensity * max(dotProduct(normal, incidence), 0);
}

float specular(Light light)
{
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 incidence = normalize(light.position - fs_in.WorldPosition);
	vec3 viewDirection = normalize(_ViewPos - fs_in.WorldPosition);

	vec3 halfVector = normalize(incidence + viewDirection);
	
	return _lightIntensity * pow(max(dotProduct(halfVector, normal), 0), _shininess);
}

float calcLight(Light light)
{
	return ambient() + diffuse(light) + specular(light);
}