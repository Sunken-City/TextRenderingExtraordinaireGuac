#version 410 core

uniform float gWave;
uniform bool gShake;
uniform float gTime;
uniform vec3 gUpVector;
uniform vec3 gRightVector;

in vec3 inPosition;
in vec4 inColor;
in vec2 inUV0;
in vec2 inNormalizedGlyphPosition;
in vec2 inNormalizedStringPosition;

out vec2 passUV0;
out vec2 passNormalizedGlyphPosition;
out vec2 passNormalizedStringPosition;


void main()
{
	passUV0 = inUV0;
	passNormalizedGlyphPosition = inNormalizedGlyphPosition;
	passNormalizedStringPosition = inNormalizedStringPosition;
	vec3 pos = 
	gl_Position = inPosition;
}