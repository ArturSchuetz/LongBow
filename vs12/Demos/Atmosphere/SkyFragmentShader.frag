#version 130
precision highp float; // needed only for version 1.30

// Calculates the Mie phase function
float getMiePhase(float fCos, float fCos2, float g, float g2)
{
	return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos2) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
}

// Calculates the Rayleigh phase function
float getRayleighPhase(float fCos2)
{
	//return 1.0;
	return 0.75 + 0.75*fCos2;
}

const float g =-0.90f;
const float g2 = 0.81f;
const float fExposure = 2;

in vec3 LightDir;
in vec3 Direction;
in vec3 Color;
in vec3 Attenuate;

out vec3 out_Color;

void main(void)
{	
	vec3 ldir = normalize(Direction);
	float fCos = dot(LightDir, ldir);
	
	float fCos2 = fCos*fCos;
	out_Color = getRayleighPhase(fCos2) * Color + getMiePhase(fCos, fCos2, g, g2) * Attenuate;
}