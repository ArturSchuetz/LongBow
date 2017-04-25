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
	void Render(Bow::Renderer::RenderContextPtr renderContext, Bow::Renderer::Camera* camera, bool renderFilled = true);

	void Update(float deltaTime, Bow::Core::Ray<double> mouseRay);

private:
	void ComputeFromOctahedron(int numberOfSubdivisions);
	void ComputeFromIcosahedron(int numberOfSubdivisions);

	void Subdivide(WaveletTriangle* triangle, int level);
	void RecursRender(WaveletTriangle* triangle, Bow::Core::VertexAttributeFloatVec3 *positionsAttribute, Bow::Core::VertexAttributeFloat *colorAttribute, int maxLevel);

	void RecursCalculateWavelet(WaveletTriangle* triangle);
	void RecursCalculateScalar(WaveletTriangle* triangle, int forceLevel, double threshold);

	void FillData(WaveletTriangle* triangle);
	void SubdivideIntersection(WaveletTriangle* triangle, Bow::Core::Ray<double> mouseRay);

	std::vector<WaveletTriangle>	m_RootTriangles;
	std::vector<WaveletTriangle>	m_RootParents;

	unsigned char					*m_HeightMap;
	unsigned int					m_MapHeight;
	unsigned int					m_MapWidth;
	unsigned int					m_MapSize;

	// ==========================================================================================
	// Rendering stuff (Not important for Algorithm)
	bool							m_isDirty;

	Bow::Renderer::ShaderProgramPtr m_shaderProgram;
	Bow::Renderer::VertexArrayPtr	m_VertexArray;

	Bow::Core::Matrix3D<float>		m_objectWorldMat;
	Bow::Renderer::RenderState		m_renderState;
};
