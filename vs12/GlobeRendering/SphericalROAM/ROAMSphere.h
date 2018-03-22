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

	void Init(bow::RenderContextPtr context, bow::ShaderProgramPtr shaderProgramm, bow::Texture2DPtr texture, bow::TextureSamplerPtr sampler, const char* heighMapPath);
	void Reset();
	void ToggleRenderMode();
	void Tessellate(bow::Vector3<float> vPosition, float fMaxError);
	void Render(bow::RenderContextPtr context, bow::Camera* camera);

private:
	void							BuildCube();
	void							RecursRender(ROAMTriangle* triangle, bow::VertexAttributeFloatVec3 *attribute);
	void							RecursTessellate(ROAMTriangle* triangle, bow::Vector3<float> vPosition, float fMaxError);
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

	bow::RenderContextPtr	m_renderContext;
	bow::ShaderProgramPtr	m_shaderProgram;
	bow::Texture2DPtr		m_terrainDiffuseTexture;

	bow::Matrix3D<float>		m_objectWorldMat;
	bow::RenderState		m_renderState;
};