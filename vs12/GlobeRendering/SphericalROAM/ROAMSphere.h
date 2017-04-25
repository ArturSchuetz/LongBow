#pragma once
#include "BowRenderer.h"
#include "BowInput.h"
#include "BowCore.h"
#include "BowResources.h"

#include "ROAMTriangle.h"

// How many TriTreeNodes should be allocated?
#define POOL_SIZE (65535)

//
// Sphere Class
//
class ROAMSphere
{
public:
	ROAMSphere();
	~ROAMSphere();

	void Init(Bow::Renderer::RenderContextPtr context, Bow::Renderer::ShaderProgramPtr shaderProgramm, Bow::Renderer::Texture2DPtr texture, Bow::Renderer::TextureSamplerPtr sampler, const char* heighMapPath);
	void Reset();
	void ToggleRenderMode();
	void Tessellate(Bow::Core::Vector3<float> vPosition, float fMaxError);
	void Render(Bow::Renderer::RenderContextPtr context, Bow::Renderer::Camera* camera);

private:
	void							BuildCube();
	void							RecursRender(ROAMTriangle* triangle, Bow::Core::VertexAttributeFloatVec3 *attribute);
	void							RecursTessellate(ROAMTriangle* triangle, Bow::Core::Vector3<float> vPosition, float fMaxError);
	void							Split(ROAMTriangle *tri);
	ROAMTriangle*					AllocateTri();

	ROAMTriangle					m_RootTriangles[12];

	static int						m_NextTriNode;
	static ROAMTriangle				m_TriPool[POOL_SIZE];

	unsigned char					*m_HeightMap;
	unsigned int					m_MapHeight;
	unsigned int					m_MapWidth;
	unsigned int					m_MapSize;

	// ==========================================================================================
	// Rendering stuff (Not important for Algorithm)

	Bow::Renderer::RenderContextPtr	m_renderContext;
	Bow::Renderer::ShaderProgramPtr	m_shaderProgram;
	Bow::Renderer::Texture2DPtr		m_terrainDiffuseTexture;

	Bow::Core::Matrix3D<float>		m_objectWorldMat;
	Bow::Renderer::RenderState		m_renderState;
};