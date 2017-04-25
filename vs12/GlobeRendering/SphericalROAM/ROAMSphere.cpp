#include "ROAMSphere.h"
#include "ROAMTriangle.h"
#include "BowBitmap.h"

int	ROAMSphere::m_NextTriNode;
ROAMTriangle ROAMSphere::m_TriPool[POOL_SIZE];

Bow::Core::Bitmap g_map;

ROAMSphere::ROAMSphere()
{
	//m_renderState.RasterizationMode = Bow::Renderer::RasterizationMode::Line;
}

ROAMSphere::~ROAMSphere()
{
}

void ROAMSphere::Init(Bow::Renderer::RenderContextPtr context, Bow::Renderer::ShaderProgramPtr shaderProgramm, Bow::Renderer::Texture2DPtr texture, Bow::Renderer::TextureSamplerPtr sampler, const char* heighMapPath)
{
	g_map.LoadFile(heighMapPath);
	m_HeightMap = g_map.GetData();
	m_MapHeight = g_map.GetHeight();
	m_MapWidth = g_map.GetWidth();
	m_MapSize = g_map.GetSizeInBytes();

	m_renderContext = context;
	m_shaderProgram = shaderProgramm;

	///////////////////////////////////////////////////////////////////
	// Vertex Array from generated Mesh

	int diffuse_Location = 0;
	m_renderContext->VSetTexture(diffuse_Location, texture);
	m_renderContext->VSetTextureSampler(diffuse_Location, sampler);
	m_shaderProgram->VSetUniform("diffuseTex", diffuse_Location);

	//m_objectWorldMat.Translate(Bow::Core::Vector3<float>(0.0f, 0.0f, 0.0f));
	//m_objectWorldMat.RotateX(-(M_PI * 0.5f));

	BuildCube();
}

void ROAMSphere::Reset()
{
	BuildCube();
}

void ROAMSphere::ToggleRenderMode()
{
	if (m_renderState.RasterizationMode == Bow::Renderer::RasterizationMode::Line)
	{
		m_renderState.RasterizationMode = Bow::Renderer::RasterizationMode::Fill;
	}
	else if (m_renderState.RasterizationMode == Bow::Renderer::RasterizationMode::Fill)
	{
		m_renderState.RasterizationMode = Bow::Renderer::RasterizationMode::Line;
	}
}

void ROAMSphere::Tessellate(Bow::Core::Vector3<float> vPosition, float fMaxError)
{
	for (int i = 0; i < 12; ++i)
	{
		RecursTessellate(&(m_RootTriangles[i]), vPosition, fMaxError);
	}
}

void ROAMSphere::RecursTessellate(ROAMTriangle* triangle, Bow::Core::Vector3<float> vPosition, float fMaxError)
{
	Bow::Core::Vector3<float> center = (triangle->m_Vertex[0] + triangle->m_Vertex[1] + triangle->m_Vertex[2]) / 3.0;
	center.Normalize();

	float fEdgeLength;
	fEdgeLength = (triangle->m_Vertex[0] - triangle->m_Vertex[1]).LengthSquared();
	fEdgeLength += (triangle->m_Vertex[1] - triangle->m_Vertex[2]).LengthSquared();
	fEdgeLength += (triangle->m_Vertex[2] - triangle->m_Vertex[0]).LengthSquared();

	if (fEdgeLength * 50 > (vPosition - center).LengthSquared())
	{
		Split(triangle);

		if(triangle->m_LeftChild)
			RecursTessellate(triangle->m_LeftChild, vPosition, fMaxError);

		if (triangle->m_RightChild)
			RecursTessellate(triangle->m_RightChild, vPosition, fMaxError);
	}
}

void ROAMSphere::Render(Bow::Renderer::RenderContextPtr context, Bow::Renderer::Camera* camera)
{
	Bow::Core::Mesh mesh;

	Bow::Core::VertexAttributeFloatVec3 *positionsAttribute = new Bow::Core::VertexAttributeFloatVec3("in_Position");
	mesh.AddAttribute(Bow::Core::VertexAttributePtr(positionsAttribute));

	for (int i = 0; i < 12; ++i)
	{
		RecursRender(&(m_RootTriangles[i]), positionsAttribute);
	}

	Bow::Renderer::VertexArrayPtr vertexArray = m_renderContext->VCreateVertexArray(mesh, m_shaderProgram->VGetVertexAttributes(), Bow::Renderer::BufferHint::StaticDraw);

	Bow::Core::Matrix3D<float> modelViewProj = (Bow::Core::Matrix3D<float>)camera->CalculateWorldViewProjection(m_objectWorldMat);
	m_shaderProgram->VSetUniform("u_ModelViewProj", (Bow::Core::Matrix4x4<float>)modelViewProj);
	m_renderContext->VDraw(Bow::Renderer::PrimitiveType::Triangles, vertexArray, m_shaderProgram, m_renderState);
}


void ROAMSphere::RecursRender(ROAMTriangle* triangle, Bow::Core::VertexAttributeFloatVec3 *attribute)
{
	if (triangle->m_LeftChild == nullptr || triangle->m_RightChild == nullptr)
	{
		double PI = 3.1415926535897931;
		Bow::Core::Vector2<float> texCoord;
		int x, y, index;
		float value;

		Bow::Core::Vector3<float> v0 = triangle->m_Vertex[0]; v0.Normalize();
		texCoord = Bow::Core::Vector2<float>((atan2(v0.y, v0.x) / (PI * 2.0)) + 0.5, (asin(v0.z) / PI) + 0.5);
		x = texCoord.x * (float)m_MapWidth;
		y = texCoord.y * (float)m_MapHeight;
		index = x + y * m_MapWidth;
		value = (float)m_HeightMap[index % m_MapSize] / (float)255;
		v0 = v0 + v0 * (value * 0.001f);

		Bow::Core::Vector3<float> v1 = triangle->m_Vertex[1]; v1.Normalize();
		texCoord = Bow::Core::Vector2<float>((atan2(v1.y, v1.x) / (PI * 2.0)) + 0.5, (asin(v1.z) / PI) + 0.5);
		x = texCoord.x * (float)m_MapWidth;
		y = texCoord.y * (float)m_MapHeight;
		index = x + y * m_MapWidth;
		value = (float)m_HeightMap[index % m_MapSize] / (float)255;
		v1 = v1 + v1 *(value * 0.001f);

		Bow::Core::Vector3<float> v2 = triangle->m_Vertex[2]; v2.Normalize();
		texCoord = Bow::Core::Vector2<float>((atan2(v2.y, v2.x) / (PI * 2.0)) + 0.5, (asin(v2.z) / PI) + 0.5);
		x = texCoord.x * (float)m_MapWidth;
		y = texCoord.y * (float)m_MapHeight;
		index = x + y * m_MapWidth;
		value = (float)m_HeightMap[index % m_MapSize] / (float)255;
		v2 = v2 + v2 * (value * 0.001f);

		attribute->Values.push_back(v0);
		attribute->Values.push_back(v1);
		attribute->Values.push_back(v2);
	}
	else
	{
		RecursRender(triangle->m_LeftChild, attribute);
		RecursRender(triangle->m_RightChild, attribute);
	}
}

void ROAMSphere::BuildCube()
{
	Bow::Core::Vector3<float> nVertex[4];
	m_NextTriNode = 0;

	// Create front face vertices and triangles   
	nVertex[0] = Bow::Core::Vector3<float>(-1, 1, 1);
	nVertex[1] = Bow::Core::Vector3<float>(-1, -1, 1);
	nVertex[2] = Bow::Core::Vector3<float>(1, -1, 1);
	nVertex[3] = Bow::Core::Vector3<float>(1, 1, 1);

	m_RootTriangles[0] = ROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	m_RootTriangles[1] = ROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	m_RootTriangles[0].m_BaseNeighbor = &(m_RootTriangles[1]);
	m_RootTriangles[1].m_BaseNeighbor = &(m_RootTriangles[0]);

	// Create back face vertices and triangles
	nVertex[0] = Bow::Core::Vector3<float>(1, 1, -1);
	nVertex[1] = Bow::Core::Vector3<float>(1, -1, -1);
	nVertex[2] = Bow::Core::Vector3<float>(-1, -1, -1);
	nVertex[3] = Bow::Core::Vector3<float>(-1, 1, -1);

	m_RootTriangles[2] = ROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	m_RootTriangles[3] = ROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	m_RootTriangles[2].m_BaseNeighbor = &(m_RootTriangles[3]);
	m_RootTriangles[3].m_BaseNeighbor = &(m_RootTriangles[2]);

	// Create left face vertices and triangles
	nVertex[0] = Bow::Core::Vector3<float>(-1, 1, -1);
	nVertex[1] = Bow::Core::Vector3<float>(-1, -1, -1);
	nVertex[2] = Bow::Core::Vector3<float>(-1, -1, 1);
	nVertex[3] = Bow::Core::Vector3<float>(-1, 1, 1);

	m_RootTriangles[4] = ROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	m_RootTriangles[5] = ROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	m_RootTriangles[4].m_BaseNeighbor = &(m_RootTriangles[5]);
	m_RootTriangles[5].m_BaseNeighbor = &(m_RootTriangles[4]);

	// Create right face vertices and triangles
	nVertex[0] = Bow::Core::Vector3<float>(1, 1, 1);
	nVertex[1] = Bow::Core::Vector3<float>(1, -1, 1);
	nVertex[2] = Bow::Core::Vector3<float>(1, -1, -1);
	nVertex[3] = Bow::Core::Vector3<float>(1, 1, -1);

	m_RootTriangles[6] = ROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	m_RootTriangles[7] = ROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	m_RootTriangles[6].m_BaseNeighbor = &(m_RootTriangles[7]);
	m_RootTriangles[7].m_BaseNeighbor = &(m_RootTriangles[6]);

	// Create top face vertices and triangles
	nVertex[0] = Bow::Core::Vector3<float>(-1, 1, -1);
	nVertex[1] = Bow::Core::Vector3<float>(-1, 1, 1);
	nVertex[2] = Bow::Core::Vector3<float>(1, 1, 1);
	nVertex[3] = Bow::Core::Vector3<float>(1, 1, -1);

	m_RootTriangles[8] = ROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	m_RootTriangles[9] = ROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	m_RootTriangles[8].m_BaseNeighbor = &(m_RootTriangles[9]);
	m_RootTriangles[9].m_BaseNeighbor = &(m_RootTriangles[8]);

	// Create bottom face vertices and triangles
	nVertex[0] = Bow::Core::Vector3<float>(-1, -1, 1);
	nVertex[1] = Bow::Core::Vector3<float>(-1, -1, -1);
	nVertex[2] = Bow::Core::Vector3<float>(1, -1, -1);
	nVertex[3] = Bow::Core::Vector3<float>(1, -1, 1);

	m_RootTriangles[10] = ROAMTriangle(nVertex[0], nVertex[1], nVertex[2]);
	m_RootTriangles[11] = ROAMTriangle(nVertex[2], nVertex[3], nVertex[0]);
	m_RootTriangles[10].m_BaseNeighbor = &(m_RootTriangles[11]);
	m_RootTriangles[11].m_BaseNeighbor = &(m_RootTriangles[10]);
}

void ROAMSphere::Split(ROAMTriangle *tri)
{
	// We are already split, no need to do it again.
	if (tri->m_LeftChild)
		return;

	// If this triangle is not in a proper diamond, force split our base neighbor
	if (tri->m_BaseNeighbor && (tri->m_BaseNeighbor->m_BaseNeighbor != tri))
		Split(tri->m_BaseNeighbor);

	// Create children and link into mesh
	tri->m_LeftChild = AllocateTri();
	// If creation failed, just exit.
	if (!tri->m_LeftChild)
		return;

	tri->m_RightChild = AllocateTri();
	// If creation failed, just exit.
	if (!tri->m_RightChild)
		return;

	tri->m_LeftChild->m_Vertex[0] = tri->m_Vertex[2];
	tri->m_LeftChild->m_Vertex[1] = (tri->m_Vertex[0] + tri->m_Vertex[2]) * 0.5;
	tri->m_LeftChild->m_Vertex[2] = tri->m_Vertex[1];

	tri->m_RightChild->m_Vertex[0] = tri->m_Vertex[1];
	tri->m_RightChild->m_Vertex[1] = (tri->m_Vertex[0] + tri->m_Vertex[2]) * 0.5;
	tri->m_RightChild->m_Vertex[2] = tri->m_Vertex[0];

	// Fill in the information we can get from the parent (neighbor pointers)
	tri->m_LeftChild->m_BaseNeighbor = tri->m_LeftNeighbor;
	tri->m_LeftChild->m_LeftNeighbor = tri->m_RightChild;

	tri->m_RightChild->m_BaseNeighbor = tri->m_RightNeighbor;
	tri->m_RightChild->m_RightNeighbor = tri->m_LeftChild;

	// Link our Left Neighbor to the new children
	if (tri->m_LeftNeighbor != NULL)
	{
		if (tri->m_LeftNeighbor->m_BaseNeighbor == tri)
			tri->m_LeftNeighbor->m_BaseNeighbor = tri->m_LeftChild;
		else if (tri->m_LeftNeighbor->m_LeftNeighbor == tri)
			tri->m_LeftNeighbor->m_LeftNeighbor = tri->m_LeftChild;
		else if (tri->m_LeftNeighbor->m_RightNeighbor == tri)
			tri->m_LeftNeighbor->m_RightNeighbor = tri->m_LeftChild;
		else
			;// Illegal Left Neighbor!
	}

	// Link our Right Neighbor to the new children
	if (tri->m_RightNeighbor != NULL)
	{
		if (tri->m_RightNeighbor->m_BaseNeighbor == tri)
			tri->m_RightNeighbor->m_BaseNeighbor = tri->m_RightChild;
		else if (tri->m_RightNeighbor->m_RightNeighbor == tri)
			tri->m_RightNeighbor->m_RightNeighbor = tri->m_RightChild;
		else if (tri->m_RightNeighbor->m_LeftNeighbor == tri)
			tri->m_RightNeighbor->m_LeftNeighbor = tri->m_RightChild;
		else
			;// Illegal Right Neighbor!
	}

	// Link our Base Neighbor to the new children
	if (tri->m_BaseNeighbor != NULL)
	{
		if (tri->m_BaseNeighbor->m_LeftChild)
		{
			tri->m_BaseNeighbor->m_LeftChild->m_RightNeighbor = tri->m_RightChild;
			tri->m_BaseNeighbor->m_RightChild->m_LeftNeighbor = tri->m_LeftChild;
			tri->m_LeftChild->m_RightNeighbor = tri->m_BaseNeighbor->m_RightChild;
			tri->m_RightChild->m_LeftNeighbor = tri->m_BaseNeighbor->m_LeftChild;
		}
		else
			Split(tri->m_BaseNeighbor);  // Base Neighbor (in a diamond with us) was not split yet, so do that now.
	}
	else
	{
		// An edge triangle, trivial case.
		tri->m_LeftChild->m_RightNeighbor = NULL;
		tri->m_RightChild->m_LeftNeighbor = NULL;
	}
}

ROAMTriangle *ROAMSphere::AllocateTri()
{
	ROAMTriangle *pTri;

	// IF we've run out of TriTreeNodes, just return NULL (this is handled gracefully)
	if (m_NextTriNode >= POOL_SIZE)
		return NULL;

	pTri = &(m_TriPool[m_NextTriNode++]);
	pTri->m_LeftChild = pTri->m_RightChild = NULL;

	return pTri;
}
