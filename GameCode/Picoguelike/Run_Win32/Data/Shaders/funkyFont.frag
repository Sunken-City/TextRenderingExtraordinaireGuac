#version 410 core

uniform sampler2D gDiffuseTexture;
uniform bool gPop;
uniform vec4 gColor1;
uniform vec4 gColor2;
uniform bool gRainbow;
uniform float gTime;

in vec2 passUV0;
in vec2 passNormalizedGlyphPosition;
in vec2 passNormalizedStringPosition;

out vec4 outColor;

void main()
{
	vec4 diffuse = texture(gDiffuseTexture, passUV0);
	if (gTime < passNormalizedStringPosition.x * .1f)
	{
		outColor = vec4(0.f);
	}
	else
	{
	outColor = diffuse;
	}
}