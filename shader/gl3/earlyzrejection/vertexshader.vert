#version 330 core

// standard layouts
layout (location = 0) in vec3 inVertex;
layout (location = 2) in vec2 inTextureUV;

// indexed rendering
layout (location = 6) in mat4 inModelMatrix;

// render groups
layout (location = 12) in vec3 inOrigin;

// uniforms
uniform mat4 cameraMatrix;
uniform mat4 projectionMatrix;
uniform mat3 textureMatrix;
uniform float time;

{$DEFINITIONS}

// will be passed to fragment shader
out vec2 vsFragTextureUV;

{$FUNCTIONS}

void main() {
	// transformations matrices
	mat4 mvpMatrix = projectionMatrix * cameraMatrix * inModelMatrix;

	// texure UV
	vsFragTextureUV = vec2(textureMatrix * vec3(inTextureUV, 1.0));

	// gl position
	gl_Position = mvpMatrix * vec4(inVertex, 1.0);
}
