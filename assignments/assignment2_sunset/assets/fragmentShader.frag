#version 450
vec4 sun(vec2 uv, float settingRate, vec2 center, vec3 color, float radius, float gradSize);
vec4 hills(vec2 uv, vec3 color, float height, float pos, float freq);

out vec4 FragColor;
in vec2 UV;

// World
uniform float _Time;
uniform float _Speed;
uniform vec2 _Resolution;

// Sky
uniform vec3 _SkyTopColor;
uniform vec3 _SkyBottomColor;

// Sun
uniform float _SunRadius;
uniform float _SunGradientFactor; // Changes the size of the sun's gradient.
uniform vec2 _SunPosition;
uniform vec3 _SunColor;

// Hills
uniform float _HillHeight;
uniform float _HillPosition;
uniform float _HillFrequency;
uniform vec3 _HillColor;

void main(){
    //***** Variables *****//
    
    //- Sky -//
    float settingRate = sin(_Time * _Speed)*0.5 + 0.5;
    
    //- Sun -//
    float sunRad = _SunRadius;
    float sunGradFactor = _SunGradientFactor;
    vec2 sunPos = _SunPosition;
    vec3 sunColor = _SunColor;
        
    //- Hills -//
    float hillHeight = _HillHeight;
    float hillPos = _HillPosition;
    float hillFreq = _HillFrequency;
    vec3 hillColor = _HillColor;

    //***** Layers *****//
    
    //- Background -//
    /*
    vec3 col = vec3(1.0, settingRate * 1.0 - 0.3, 0.0);
    vec3 col2 = vec3(0.5, settingRate * 1.0 - 0.3, 1.0);
    */
        
    vec3 col = vec3(_SkyTopColor.x, settingRate * 1.0 - (1.2 - _SkyTopColor.y), _SkyTopColor.z);
    vec3 col2 = vec3(_SkyBottomColor.x, settingRate * 1.0 - (1.2 - _SkyBottomColor.y), _SkyBottomColor.z);
    col = mix(col, col2, UV.y);

    //- Sun -//
        
    vec4 layer2 = sun(UV, settingRate, sunPos, sunColor, sunRad, sunGradFactor);
    col = mix(col, sunColor, layer2.a);
        
    //- Hills -//
        
    // Hill 1
            
    hillColor = vec3(0.15, 0.4, 0.15);
    vec4 layer3 = hills(UV, hillColor, hillHeight, hillPos, hillFreq);
    col = mix(col, hillColor, layer3.a);
            
    // Hill 1 Shadow
            
    hillColor = vec3(0.15, 0.3, 0.15);
    vec4 layer4 = hills(UV, hillColor, hillHeight - 0.005, hillPos + 0.005, hillFreq + 0.1);
    col = mix(col, hillColor, layer4.a);
        
    // Hill 2
            
    hillColor = vec3(0.15, 0.25, 0.15);
    vec4 layer5 = hills(UV, hillColor, hillHeight - 0.2, hillPos + 9.0, hillFreq + 1.88);
    col = mix(col, hillColor, layer5.a);
            
    // Hill 2 Shadow
            
    hillColor = vec3(0.1, 0.2, 0.1);
    vec4 layer6 = hills(UV, hillColor, hillHeight - 0.205, hillPos + 8.85, hillFreq + 1.98);
    col = mix(col, hillColor, layer6.a);
    
    //***** Output *****//

    FragColor = vec4(col.x, col.y * settingRate, col.z, 1.0);
}

vec4 sun(vec2 UV, float settingRate, vec2 center, vec3 color, float radius, float gradSize) 
{
    // Center of sun
    UV.x -= center.x;
    UV.y -= center.y * settingRate;
    UV.x *= _Resolution.x / _Resolution.y;
    
    // Draw sun 
    float d = length(UV);
    float t = smoothstep(radius - gradSize * 0.0375, radius + gradSize, d); // To retain the sun's radius, parameter 1 should be subtracted by 3/80 of what parameter 2 is increased by.
    return vec4(color, 1.0 - t);
}

vec4 hills(vec2 UV, vec3 color, float height, float pos, float freq)
{
    // Draw hill
    float curve = sin((UV.x + pos) * freq)*height + 0.4;
    float h = smoothstep(curve, curve + 0.01, UV.y);
    return vec4(color, 1.0 - h);
}