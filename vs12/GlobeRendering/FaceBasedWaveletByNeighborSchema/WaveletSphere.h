#pragma once
#include "BowRenderer.h"
#include "BowInput.h"
#include "BowCore.h"
#include "BowResources.h"

#include "WaveletFace.h"

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
	void Subdivide(Bow::Core::Ray<double> mouseRay);

private:
	void ComputeFromOctahedron(int numberOfSubdivisions);
	void Subdivide(WaveletFace *face, unsigned int &currentFaceIndex, unsigned int &currentValueIndex);
	void ConnectNeighbors(WaveletFace *face);

	void RecursRender(WaveletFace* face, Bow::Core::VertexAttributeFloatVec3 *positionsAttribute, Bow::Core::VertexAttributeFloat *colorAttribute);
	void FillDataRecursive(WaveletFace* face);

	void SubdivideIntersection(WaveletFace* triangle, Bow::Core::Ray<double> mouseRay);
	void HighlightIntersection(WaveletFace* triangle, Bow::Core::Ray<double> mouseRay);
	void DeselectRecursive(WaveletFace* triangle);

	unsigned int NumberOfTriangles(int numberOfSubdivisions);

	WaveletFace						*m_Faces;
	float							*m_Values;
	unsigned char					m_NumberOfRootFaces;

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