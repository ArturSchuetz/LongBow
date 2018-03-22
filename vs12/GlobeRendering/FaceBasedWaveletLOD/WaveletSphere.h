#pragma once
#include "BowRenderer.h"
#include "BowInput.h"
#include "BowCore.h"
#include "BowResources.h"
#include "BowCorePredeclares.h"

#include "WaveletFace.h"

//
// Sphere Class
//
class WaveletSphere
{
public:
	WaveletSphere();
	~WaveletSphere();

	void GenerateData(unsigned int subdivisions);
	void Init(bow::ShaderProgramPtr shaderProgram);
	void Update(float deltaTime, bow::Camera* camera);
	void Render(bow::RenderContextPtr renderContext, bow::Texture2DPtr texture, bow::TextureSamplerPtr sampler, bow::Camera* camera, bool renderFilled = true);

	bool m_realtime;
	bool m_interpolateFaces;
	bool m_renderHeight;
	bool m_updateVertexAttributes;
	bool m_renderTextured;

private:
	void CalculateWaveletCoefficients(int numberOfSubdivisions);
	void CaluclateWaveletCoefficientsRecustive(WaveletFace* face, int level, const bow::Clipmap* heighMap, std::ofstream* outputFile);
	
	void CaluclateScalarCoefficientsRecustive(WaveletFace* triangle, bow::Frustum<double>* frustum, bow::Camera* camera, bow::Vector3<float> position, int minLevel, int maxLevel, float threshold, std::ifstream* outputFile);

	void RecursRender(WaveletFace* face, bow::VertexAttributeFloatVec4 *positionsAttribute, bow::VertexAttributeFloatVec3 *normalsAttribute);

	WaveletFace*	m_Faces;

	unsigned int	m_currentCount; // for progress
	unsigned int	m_notNullCount; // for progress
	double			m_percentage;// for progress
	float			m_maxValue;// for progress
	float			m_minValue;// for progress

	unsigned int	m_NumberOfSubdivions;
	unsigned int	m_NumberOfTriangles;
	unsigned int	m_NumberOfValues;
	unsigned char	m_NumberOfRootFaces;

	float m_heightMultiplicator;

	// ==========================================================================================
	// Rendering stuff (Not important for Algorithm)
	bool							m_isDirty;

	bow::ShaderProgramPtr m_shaderProgram;
	bow::VertexArrayPtr	m_VertexArray;

	bow::Matrix3D<float>		m_objectWorldMat;
	bow::RenderState		m_renderState;
};
