#version 140
const float fScaleDepth = 0.25;
const float fInvScaleDepth = 1.0 / 0.25;
const float fSamples = 2.0;
const int nSamples = 2;

// The scale equation calculated by Vernier's Graphical Analysis
float scale(float fCos)
{
	float x = 1.0 - fCos;  
	return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

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

// Returns the near intersection point of a line and a sphere
float getNearIntersection(vec3 worldPosition, vec3 v3Ray, float fDistance2, float fRadius2)
{
	float B = 2.0 * dot(worldPosition, v3Ray);
	float C = fDistance2 - fRadius2;
	float fDet = max(0.0, B*B - 4.0 * C);
	return 0.5 * (-B - sqrt(fDet));
}

// Returns the far intersection point of a line and a sphere
float getFarIntersection(vec3 worldPosition, vec3 v3Ray, float fDistance2, float fRadius2)
{
	float B = 2.0 * dot(worldPosition, v3Ray);
	float C = fDistance2 - fRadius2;
	float fDet = max(0.0, B*B - 4.0 * C);
	return 0.5 * (-B + sqrt(fDet));
}

in vec3 in_Position;
in vec2 in_TexCoord;

uniform vec3 v3LightDir;			// Direction vector to the light source
uniform vec3 v3LightPos;			// The light's current position
uniform vec3 v3CameraPos;			// The camera's current position
uniform float fCameraHeight;		// The camera's current height
uniform float fCameraHeight2;		// fCameraHeight^2

uniform float fOuterRadius;			// The outer (atmosphere) radius
uniform float fOuterRadius2;		// fOuterRadius^2
uniform float fInnerRadius;			// The inner (planetary) radius
uniform float fInnerRadius2;		// fInnerRadius^2
uniform float fScaleOverScaleDepth;	// fScale / fScaleDepth
uniform float fScale;
uniform vec3 v3InvWavelength;

uniform float fKrESun;				// Kr * ESun
uniform float fKmESun;				// Km * ESun
uniform float fKr4PI;				// Kr * 4 * PI
uniform float fKm4PI;				// Km * 4 * PI

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 TextureCoord;
out vec3 Color;
out vec3 Attenuate;

void main(void)
{
	vec3 worldPosition = (vec4(in_Position, 1.0) * ModelMatrix).xyz;
	vec4 viewPosition = vec4(worldPosition, 1.0) * ViewMatrix;

	vec3 v3Ray = worldPosition - v3CameraPos.xyz;
	worldPosition = normalize(worldPosition);
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
	float fNear = getNearIntersection(v3CameraPos.xyz, v3Ray, fCameraHeight2, fOuterRadius2);

	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 v3Start = v3CameraPos.xyz + v3Ray * fNear;
	fFar -= fNear;
	float fDepth = exp((fInnerRadius - fOuterRadius) * fInvScaleDepth);
	float fCameraAngle = dot(-v3Ray, worldPosition);
	float fLightAngle = dot(v3LightDir, worldPosition);
	float fCameraScale = scale(fCameraAngle);
	float fLightScale = scale(fLightAngle);
	float fCameraOffset = fDepth*fCameraScale;
	float fTemp = (fLightScale + fCameraScale);

	// Initialize the scattering loop variables
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * fScale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	vec3 v3Attenuate;
	for(int i=0; i<nSamples; i++)
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
		float fScatter = fDepth*fTemp - fCameraOffset;
		v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	gl_Position = viewPosition * ProjectionMatrix;
	Color = v3FrontColor;
	Attenuate = v3Attenuate;
	TextureCoord = in_TexCoord;
}