#pragma once
#include "BowRenderer.h"
#include "BowInput.h"
#include "BowCore.h"
#include "BowResources.h"

#include "WaveletTriangle.h"

//
// Sphere Class
//
class WaveletSphere
{
public:
	WaveletSphere();
	~WaveletSphere();

	void Init(bow::ShaderProgramPtr shaderProgram, const char* heighMapPath);
	void Render(bow::RenderContextPtr renderContext, bow::Camera* camera, bool renderFilled = true);

	void Update(float deltaTime);

private:
	void ComputeFromIcosahedron(int numberOfSubdivisions);

	void Subdivide(WaveletTriangle* triangle, int level);
	void RecursRender(WaveletTriangle* triangle, bow::IndicesUnsignedInt *indices);

	void RecursCalculateScalar(WaveletTriangle* triangle, int forceLevel, float threshold);
	void RecursCalculateWavelet(WaveletTriangle* triangle);

	void FillData(WaveletTriangle* triangle);

	std::vector<WaveletTriangle>			m_RootTriangles;

	std::vector<bow::Vector3<float>>	m_Positions;
	std::vector<float>						m_Values;

	unsigned char					*m_HeightMap;
	unsigned int					m_MapHeight;
	unsigned int					m_MapWidth;
	unsigned int					m_MapSize;

	// ==========================================================================================
	// Rendering stuff (Not important for Algorithm)
	bool							m_isDirty; 

	bow::ShaderProgramPtr m_shaderProgram;
	bow::VertexArrayPtr	m_VertexArray;

	bow::Matrix3D<float>		m_objectWorldMat;
	bow::RenderState		m_renderState;
};
