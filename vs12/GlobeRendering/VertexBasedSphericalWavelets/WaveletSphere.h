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

	void Init(Bow::Renderer::ShaderProgramPtr shaderProgram, const char* heighMapPath);
	void Render(Bow::Renderer::RenderContextPtr renderContext, Bow::Renderer::Camera* camera);
	void RenderDetail(Bow::Renderer::RenderContextPtr renderContext, Bow::Renderer::Camera* camera);

	void Update(float deltaTime);

	void LevelUp();
	void LevelDown();

private:
	void ComputeFromIcosahedron(int numberOfSubdivisions);

	void Subdivide(WaveletTriangle* triangle, int level);

	void RecursRender(WaveletTriangle* triangle, Bow::Core::IndicesUnsignedInt *indices);
	void RecursRenderDetail(WaveletTriangle* triangle, Bow::Core::IndicesUnsignedInt *indices);

	void RecursCalculateWavelet(WaveletTriangle* triangle);

	void FillData(WaveletTriangle* triangle);
	void RecursLevelUp(WaveletTriangle* triangle);
	void RecursLevelDown(WaveletTriangle* triangle);

	std::vector<WaveletTriangle>			m_RootTriangles;

	std::vector<Bow::Core::Vector3<float>>	m_Positions;
	std::vector<float>						m_Values;

	unsigned char					*m_HeightMap;
	unsigned int					m_MapHeight;
	unsigned int					m_MapWidth;
	unsigned int					m_MapSize;

	// ==========================================================================================
	// Rendering stuff (Not important for Algorithm)
	bool							m_isDirty; 
	bool							m_isDetailDirty;

	Bow::Renderer::ShaderProgramPtr m_shaderProgram;
	Bow::Renderer::VertexArrayPtr	m_VertexArray;

	Bow::Core::Matrix3D<float>		m_objectWorldMat;
	Bow::Renderer::RenderState		m_renderState;
};
