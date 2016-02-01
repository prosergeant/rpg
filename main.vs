#version 330 core

#define NUM_LIGHTS 5

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace[NUM_LIGHTS];

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace[NUM_LIGHTS];

void main(){


	gl_Position = MVP * vec4(vertexPosition_modelspace,1);
	
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
	
	vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	vec3 LightPosition_cameraspace[NUM_LIGHTS];
	
	for(int i = 0; i < NUM_LIGHTS; i++)
	{
		LightPosition_cameraspace[i] = ( V * vec4(LightPosition_worldspace[i],1)).xyz;
		LightDirection_cameraspace[i] = LightPosition_cameraspace[i] + EyeDirection_cameraspace;
	}
	
	//LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;
	//LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
	
	Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	
	UV = vertexUV;
}

