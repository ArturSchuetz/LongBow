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
	void Init(Bow::Renderer::ShaderProgramPtr shaderProgram);
	void Update(float deltaTime, Bow::Renderer::Camera* camera);
	void Render(Bow::Renderer::RenderContextPtr renderContext, Bow::Renderer::Texture2DPtr texture, Bow::Renderer::TextureSamplerPtr sampler, Bow::Renderer::Camera* camera, bool renderFilled = true);

	bool m_realtime;
	bool m_interpolateFaces;
	bool m_renderHeight;
	bool m_updateVertexAttributes;
	bool m_renderTextured;

private:
	void CalculateWaveletCoefficients(int numberOfSubdivisions);
	void CaluclateWaveletCoefficientsRecustive(WaveletFace* face, int level, const Bow::Core::Clipmap* heighMap, std::ofstream* outputFile);
	
	void CaluclateScalarCoefficientsRecustive(WaveletFace* triangle, Bow::Core::Frustum<double>* frustum, Bow::Renderer::Camera* camera, Bow::Core::Vector3<float> position, int minLevel, int maxLevel, float threshold, std::ifstream* outputFile);

	void RecursRender(WaveletFace* face, Bow::Core::VertexAttributeFloatVec4 *positionsAttribute, Bow::Core::VertexAttributeFloatVec3 *normalsAttribute);

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

	Bow::Renderer::ShaderProgramPtr m_shaderProgram;
	Bow::Renderer::VertexArrayPtr	m_VertexArray;

	Bow::Core::Matrix3D<float>		m_objectWorldMat;
	Bow::Renderer::RenderState		m_renderState;
};
