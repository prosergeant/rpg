#version 330 core

#define NUM_LIGHTS 5

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace[NUM_LIGHTS];

// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform mat4 V;
uniform vec3 LightPosition_worldspace[NUM_LIGHTS];

void main(){
	
	vec3 LightColor = vec3(1,1,0.5);
	float LightPower = 20.0 * NUM_LIGHTS;
	
	vec3 MaterialDiffuseColor = texture2D( myTextureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.13,0.13,0.13) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);
	
	vec3 tempColor = vec3(0,0,0); //vec3(0.1, 0.1, 0.1);
	
	for(int i = 0; i < NUM_LIGHTS; i++)
	{	
	float distance = length( LightPosition_worldspace[i] - Position_worldspace );

	vec3 n = normalize( Normal_cameraspace );
	
	vec3 l = normalize( LightDirection_cameraspace[i] );
	
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	vec3 E = normalize(EyeDirection_cameraspace);
	
	vec3 R = reflect(-l,n);
	
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	//color = 
	tempColor = tempColor +
		MaterialAmbientColor +
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);
	}
	color = vec4(tempColor,1);
	
}