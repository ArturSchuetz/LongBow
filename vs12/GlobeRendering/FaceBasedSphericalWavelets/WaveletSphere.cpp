#include "WaveletSphere.h"
#include "BowBitmap.h"

Bow::Core::Bitmap g_map;

WaveletSphere::WaveletSphere()
{
	m_objectWorldMat.Translate(Bow::Core::Vector3<float>(0.0f, 0.0f, 0.0f));
	m_objectWorldMat.RotateX(-(M_PI * 0.5f));

	m_isDirty = true;
}

WaveletSphere::~WaveletSphere()
{
}

void WaveletSphere::Init(Bow::Renderer::ShaderProgramPtr shaderProgram, const char* heighMapPath)
{
	g_map.LoadFile(heighMapPath);
	m_HeightMap = g_map.GetData();
	m_MapHeight = g_map.GetHeight();
	m_MapWidth = g_map.GetWidth();
	m_MapSize = g_map.GetSizeInBytes();

	m_shaderProgram = shaderProgram;

	ComputeFromIcosahedron(8);

	m_Heights.resize(m_Positions.size());
	memset(&(m_Heights[0]), 0, m_Heights.size() * sizeof(float));
	
	for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
	{
		FillData(&(m_RootTriangles[i]));
	}

	for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
	{
		RecursCalculateWavelet(&(m_RootTriangles[i]));
	}
}

void WaveletSphere::Render(Bow::Renderer::RenderContextPtr renderContext, Bow::Renderer::Camera* camera)
{
	if (m_isDirty)
	{
		Bow::Core::Mesh mesh;

		Bow::Core::VertexAttributeFloatVec3 *positionsAttribute = new Bow::Core::VertexAttributeFloatVec3("in_Position");
		mesh.AddAttribute(Bow::Core::VertexAttributePtr(positionsAttribute));

		Bow::Core::VertexAttributeFloat *heightAttribute = new Bow::Core::VertexAttributeFloat("in_Height");
		mesh.AddAttribute(Bow::Core::VertexAttributePtr(heightAttribute));

		Bow::Core::VertexAttributeFloat *colorAttribute = new Bow::Core::VertexAttributeFloat("in_Color");
		mesh.AddAttribute(Bow::Core::VertexAttributePtr(colorAttribute));

		for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
		{
			RecursRender(&(m_RootTriangles[i]), positionsAttribute, heightAttribute, colorAttribute);
		}

		m_VertexArray = renderContext->VCreateVertexArray(mesh, m_shaderProgram->VGetVertexAttributes(), Bow::Renderer::BufferHint::StaticDraw);
		m_isDirty = false;
		m_isDetailDirty = true;
	}

	Bow::Core::Matrix3D<float> modelViewProj = (Bow::Core::Matrix3D<float>)camera->CalculateWorldViewProjection(m_objectWorldMat);
	m_shaderProgram->VSetUniform("u_ModelViewProj", (Bow::Core::Matrix4x4<float>)modelViewProj);
	m_shaderProgram->VSetUniform("u_terrainHeight", 0.3f);
	renderContext->VDraw(Bow::Renderer::PrimitiveType::Triangles, m_VertexArray, m_shaderProgram, m_renderState);
}

void WaveletSphere::RenderDetail(Bow::Renderer::RenderContextPtr renderContext, Bow::Renderer::Camera* camera)
{
	if (m_isDetailDirty)
	{
		Bow::Core::Mesh mesh;

		Bow::Core::VertexAttributeFloatVec3 *positionsAttribute = new Bow::Core::VertexAttributeFloatVec3("in_Position");
		mesh.AddAttribute(Bow::Core::VertexAttributePtr(positionsAttribute));

		Bow::Core::VertexAttributeFloat *heightAttribute = new Bow::Core::VertexAttributeFloat("in_Height");
		mesh.AddAttribute(Bow::Core::VertexAttributePtr(heightAttribute));

		Bow::Core::VertexAttributeFloat *colorAttribute = new Bow::Core::VertexAttributeFloat("in_Color");
		mesh.AddAttribute(Bow::Core::VertexAttributePtr(colorAttribute));

		for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
		{
			RecursRenderDetail(&(m_RootTriangles[i]), positionsAttribute, heightAttribute, colorAttribute);
		}

		m_VertexArray = renderContext->VCreateVertexArray(mesh, m_shaderProgram->VGetVertexAttributes(), Bow::Renderer::BufferHint::StaticDraw);
		m_isDirty = true;
		m_isDetailDirty = false;
	}

	Bow::Core::Matrix3D<float> modelViewProj = (Bow::Core::Matrix3D<float>)camera->CalculateWorldViewProjection(m_objectWorldMat);
	m_shaderProgram->VSetUniform("u_ModelViewProj", (Bow::Core::Matrix4x4<float>)modelViewProj);
	m_shaderProgram->VSetUniform("u_terrainHeight", 0.0f);
	renderContext->VDraw(Bow::Renderer::PrimitiveType::Triangles, m_VertexArray, m_shaderProgram, m_renderState);
}

void WaveletSphere::Update(float deltaTime)
{
	//m_objectWorldMat.RotateZ(0.05f * deltaTime);
}

void WaveletSphere::ComputeFromIcosahedron(int numberOfSubdivisions)
{
	LOG_ASSERT(numberOfSubdivisions >= 0, "numberOfSubdivisions has to be 0 or bigger");

	const double X = 0.525731112119133606;
	const double Z = 0.850650808352039932;

	Bow::Core::Vector3<double> v0 = Bow::Core::Vector3<double>(-X, 0.0, Z); v0.Normalize();
	Bow::Core::Vector3<double> v1 = Bow::Core::Vector3<double>(X, 0.0, Z); v1.Normalize();
	Bow::Core::Vector3<double> v2 = Bow::Core::Vector3<double>(-X, 0.0, -Z); v2.Normalize();
	Bow::Core::Vector3<double> v3 = Bow::Core::Vector3<double>(X, 0.0, -Z); v3.Normalize();
	Bow::Core::Vector3<double> v4 = Bow::Core::Vector3<double>(0.0, Z, X); v4.Normalize();
	Bow::Core::Vector3<double> v5 = Bow::Core::Vector3<double>(0.0, Z, -X); v5.Normalize();
	Bow::Core::Vector3<double> v6 = Bow::Core::Vector3<double>(0.0, -Z, X); v6.Normalize();
	Bow::Core::Vector3<double> v7 = Bow::Core::Vector3<double>(0.0, -Z, -X); v7.Normalize();
	Bow::Core::Vector3<double> v8 = Bow::Core::Vector3<double>(Z, X, 0.0); v8.Normalize();
	Bow::Core::Vector3<double> v9 = Bow::Core::Vector3<double>(-Z, X, 0.0); v9.Normalize();
	Bow::Core::Vector3<double> v10 = Bow::Core::Vector3<double>(Z, -X, 0.0); v10.Normalize();
	Bow::Core::Vector3<double> v11 = Bow::Core::Vector3<double>(-Z, -X, 0.0); v11.Normalize();

	m_Positions.push_back(v0);
	m_Positions.push_back(v1);
	m_Positions.push_back(v2);
	m_Positions.push_back(v3);
	m_Positions.push_back(v4);
	m_Positions.push_back(v5);
	m_Positions.push_back(v6);
	m_Positions.push_back(v7);
	m_Positions.push_back(v8);
	m_Positions.push_back(v9);
	m_Positions.push_back(v10);
	m_Positions.push_back(v11);

	m_RootTriangles.push_back(WaveletTriangle(0, 1, 4));
	m_RootTriangles.push_back(WaveletTriangle(0, 4, 9));
	m_RootTriangles.push_back(WaveletTriangle(9, 4, 5));
	m_RootTriangles.push_back(WaveletTriangle(4, 8, 5));
	m_RootTriangles.push_back(WaveletTriangle(4, 1, 8));
	m_RootTriangles.push_back(WaveletTriangle(8, 1, 10));
	m_RootTriangles.push_back(WaveletTriangle(8, 10, 3));
	m_RootTriangles.push_back(WaveletTriangle(5, 8, 3));
	m_RootTriangles.push_back(WaveletTriangle(5, 3, 2));
	m_RootTriangles.push_back(WaveletTriangle(2, 3, 7));
	m_RootTriangles.push_back(WaveletTriangle(7, 3, 10));
	m_RootTriangles.push_back(WaveletTriangle(7, 10, 6));
	m_RootTriangles.push_back(WaveletTriangle(7, 6, 11));
	m_RootTriangles.push_back(WaveletTriangle(11, 6, 0));
	m_RootTriangles.push_back(WaveletTriangle(0, 6, 1));
	m_RootTriangles.push_back(WaveletTriangle(6, 10, 1));
	m_RootTriangles.push_back(WaveletTriangle(9, 11, 0));
	m_RootTriangles.push_back(WaveletTriangle(9, 2, 11));
	m_RootTriangles.push_back(WaveletTriangle(9, 5, 2));
	m_RootTriangles.push_back(WaveletTriangle(7, 11, 2));

	for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
	{
		Subdivide(&(m_RootTriangles[i]), numberOfSubdivisions);
	}
}

void WaveletSphere::Subdivide(WaveletTriangle *triangle, int level)
{
	if (level > 0)
	{
		Bow::Core::Vector3<float> vec1 = (m_Positions[triangle->m_VertexIndices[0]] + m_Positions[triangle->m_VertexIndices[1]]) * 0.5;
		Bow::Core::Vector3<float> vec2 = (m_Positions[triangle->m_VertexIndices[1]] + m_Positions[triangle->m_VertexIndices[2]]) * 0.5;
		Bow::Core::Vector3<float> vec3 = (m_Positions[triangle->m_VertexIndices[2]] + m_Positions[triangle->m_VertexIndices[0]]) * 0.5;

		vec1.Normalize();
		vec2.Normalize();
		vec3.Normalize();

		m_Positions.push_back(vec1);
		m_Positions.push_back(vec2);
		m_Positions.push_back(vec3);

		int i1 = m_Positions.size() - 3;
		int i2 = m_Positions.size() - 2;
		int i3 = m_Positions.size() - 1;

		triangle->m_T0 = new WaveletTriangle(i1, i2, i3);
		triangle->m_T1 = new WaveletTriangle(triangle->m_VertexIndices[0], i1, i3);
		triangle->m_T2 = new WaveletTriangle(i1, triangle->m_VertexIndices[1], i2);
		triangle->m_T3 = new WaveletTriangle(i3, i2, triangle->m_VertexIndices[2]);

		triangle->m_T0->m_Parent = triangle->m_T1->m_Parent = triangle->m_T2->m_Parent = triangle->m_T3->m_Parent = triangle;

		//
		// Subdivide input triangle into four triangles
		//
		--level;
		Subdivide(triangle->m_T0, level);
		Subdivide(triangle->m_T1, level);
		Subdivide(triangle->m_T2, level);
		Subdivide(triangle->m_T3, level);
	}
}

void WaveletSphere::RecursRender(WaveletTriangle* triangle, Bow::Core::VertexAttributeFloatVec3 *positionsAttribute, Bow::Core::VertexAttributeFloat *heightAttribute, Bow::Core::VertexAttributeFloat *colorAttribute)
{
	if (triangle->HasChildren() && !triangle->ChildrenAreWavelets())
	{
		RecursRender(triangle->m_T0, positionsAttribute, heightAttribute, colorAttribute);
		RecursRender(triangle->m_T1, positionsAttribute, heightAttribute, colorAttribute);
		RecursRender(triangle->m_T2, positionsAttribute, heightAttribute, colorAttribute);
		RecursRender(triangle->m_T3, positionsAttribute, heightAttribute, colorAttribute);
	}
	else
	{
		positionsAttribute->Values.push_back(m_Positions[triangle->m_VertexIndices[0]]);
		positionsAttribute->Values.push_back(m_Positions[triangle->m_VertexIndices[1]]);
		positionsAttribute->Values.push_back(m_Positions[triangle->m_VertexIndices[2]]);

		heightAttribute->Values.push_back(m_Heights[triangle->m_VertexIndices[0]]);
		heightAttribute->Values.push_back(m_Heights[triangle->m_VertexIndices[1]]);
		heightAttribute->Values.push_back(m_Heights[triangle->m_VertexIndices[2]]);

		colorAttribute->Values.push_back((float)(m_Values[triangle->m_ValueIndex]));
		colorAttribute->Values.push_back((float)(m_Values[triangle->m_ValueIndex]));
		colorAttribute->Values.push_back((float)(m_Values[triangle->m_ValueIndex]));
	}
}

void WaveletSphere::RecursRenderDetail(WaveletTriangle* triangle, Bow::Core::VertexAttributeFloatVec3 *positionsAttribute, Bow::Core::VertexAttributeFloat *heightAttribute, Bow::Core::VertexAttributeFloat *colorAttribute)
{
	if (triangle->HasChildren())
	{
		RecursRenderDetail(triangle->m_T0, positionsAttribute, heightAttribute, colorAttribute);
		RecursRenderDetail(triangle->m_T1, positionsAttribute, heightAttribute, colorAttribute);
		RecursRenderDetail(triangle->m_T2, positionsAttribute, heightAttribute, colorAttribute);
		RecursRenderDetail(triangle->m_T3, positionsAttribute, heightAttribute, colorAttribute);
	}
	else
	{
		positionsAttribute->Values.push_back(m_Positions[triangle->m_VertexIndices[0]]);
		positionsAttribute->Values.push_back(m_Positions[triangle->m_VertexIndices[1]]);
		positionsAttribute->Values.push_back(m_Positions[triangle->m_VertexIndices[2]]);

		heightAttribute->Values.push_back(0.0f);
		heightAttribute->Values.push_back(0.0f);
		heightAttribute->Values.push_back(0.0f);

		colorAttribute->Values.push_back((float)(m_Values[triangle->m_ValueIndex]));
		colorAttribute->Values.push_back((float)(m_Values[triangle->m_ValueIndex]));
		colorAttribute->Values.push_back((float)(m_Values[triangle->m_ValueIndex]));
	}
}

void WaveletSphere::RecursCalculateWavelet(WaveletTriangle* triangle)
{
	if (triangle->HasChildren())
	{
		RecursCalculateWavelet(triangle->m_T0);
		RecursCalculateWavelet(triangle->m_T1);
		RecursCalculateWavelet(triangle->m_T2);
		RecursCalculateWavelet(triangle->m_T3);
	}

	triangle->Analysis(m_Positions, m_Values);
}

void WaveletSphere::FillData(WaveletTriangle* triangle)
{
	if (triangle->HasChildren())
	{
		FillData(triangle->m_T0);
		FillData(triangle->m_T1);
		FillData(triangle->m_T2);
		FillData(triangle->m_T3);
	}

	if (!triangle->HasChildren())
	{
		double PI = 3.1415926535897931;
		Bow::Core::Vector2<double> texCoord;
		int x, y, index;

		Bow::Core::Vector3<double> center = (m_Positions[triangle->m_VertexIndices[0]] + m_Positions[triangle->m_VertexIndices[1]] + m_Positions[triangle->m_VertexIndices[2]]) * 1.0 / 3.0;

		texCoord = Bow::Core::Vector2<double>((atan2(center.y, center.x) / (PI * 2.0)) + 0.5, (asin(center.z) / PI) + 0.5);
		x = (int)(texCoord.x * (double)m_MapWidth);
		y = (int)(texCoord.y * (double)m_MapHeight);
		index = x + y * m_MapWidth;

		triangle->SetValue(m_Values.size());
		m_Values.push_back((double)m_HeightMap[index % m_MapSize] / (double)255);

		texCoord = Bow::Core::Vector2<double>((atan2(m_Positions[triangle->m_VertexIndices[0]].y, m_Positions[triangle->m_VertexIndices[0]].x) / (PI * 2.0)) + 0.5, (asin(m_Positions[triangle->m_VertexIndices[0]].z) / PI) + 0.5);
		x = (int)(texCoord.x * (double)m_MapWidth);
		y = (int)(texCoord.y * (double)m_MapHeight);
		index = x + y * m_MapWidth;
		m_Heights[triangle->m_VertexIndices[0]] = (float)m_HeightMap[index % m_MapSize] / (float)255;

		texCoord = Bow::Core::Vector2<double>((atan2(m_Positions[triangle->m_VertexIndices[1]].y, m_Positions[triangle->m_VertexIndices[1]].x) / (PI * 2.0)) + 0.5, (asin(m_Positions[triangle->m_VertexIndices[1]].z) / PI) + 0.5);
		x = (int)(texCoord.x * (double)m_MapWidth);
		y = (int)(texCoord.y * (double)m_MapHeight);
		index = x + y * m_MapWidth;
		m_Heights[triangle->m_VertexIndices[1]] = (float)m_HeightMap[index % m_MapSize] / (float)255;

		texCoord = Bow::Core::Vector2<double>((atan2(m_Positions[triangle->m_VertexIndices[2]].y, m_Positions[triangle->m_VertexIndices[2]].x) / (PI * 2.0)) + 0.5, (asin(m_Positions[triangle->m_VertexIndices[2]].z) / PI) + 0.5);
		x = (int)(texCoord.x * (double)m_MapWidth);
		y = (int)(texCoord.y * (double)m_MapHeight);
		index = x + y * m_MapWidth;
		m_Heights[triangle->m_VertexIndices[2]] = (float)m_HeightMap[index % m_MapSize] / (float)255;
	}
}

void WaveletSphere::LevelUp()
{
	for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
	{
		RecursLevelUp(&(m_RootTriangles[i]));
	}
}

void WaveletSphere::RecursLevelUp(WaveletTriangle* triangle)
{
	if (triangle->HasChildren() && !triangle->ChildrenAreWavelets())
	{
		if (triangle->m_T0->HasChildren() && triangle->m_T1->HasChildren() && triangle->m_T2->HasChildren() && triangle->m_T3->HasChildren()
			&& !(triangle->m_T0->ChildrenAreWavelets() && triangle->m_T1->ChildrenAreWavelets() && triangle->m_T2->ChildrenAreWavelets() && triangle->m_T3->ChildrenAreWavelets()))
		{
			RecursLevelUp(triangle->m_T0);
			RecursLevelUp(triangle->m_T1);
			RecursLevelUp(triangle->m_T2);
			RecursLevelUp(triangle->m_T3);
		}
		else
		{
			triangle->Analysis(m_Positions, m_Values);
			m_isDirty = true;
			m_isDetailDirty = true;
		}
	}
}

void WaveletSphere::LevelDown()
{
	for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
	{
		RecursLevelDown(&(m_RootTriangles[i]));
	}
}

void WaveletSphere::RecursLevelDown(WaveletTriangle* triangle)
{
	if (triangle->HasChildren() && triangle->ChildrenAreWavelets())
	{
		triangle->Synthesis(m_Positions, m_Values);
		m_isDirty = true;
		m_isDetailDirty = true;
	}
	else if(triangle->HasChildren())
	{
		RecursLevelDown(triangle->m_T0);
		RecursLevelDown(triangle->m_T1);
		RecursLevelDown(triangle->m_T2);
		RecursLevelDown(triangle->m_T3);
	}
}
