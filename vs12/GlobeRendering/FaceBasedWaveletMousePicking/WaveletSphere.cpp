#include "WaveletSphere.h"
#include "BowBitmap.h"

Bow::Core::Bitmap g_map;

WaveletSphere::WaveletSphere()
{
	m_objectWorldMat.Translate(Bow::Core::Vector3<float>(0.0f, 0.0f, 0.0f));
	//m_objectWorldMat.RotateX(-(M_PI * 0.5f));
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

	ComputeFromOctahedron(10);
	
	for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
	{
		FillData(&(m_RootTriangles[i]));
	}

	for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
	{
		RecursCalculateWavelet(&(m_RootTriangles[i]));
	}

	//for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
	//{
	//	RecursCalculateScalar(&(m_RootTriangles[i]), 0, 0.0f);
	//}
}

void WaveletSphere::Render(Bow::Renderer::RenderContextPtr renderContext, Bow::Renderer::Camera* camera, bool renderFilled)
{
	if (m_isDirty)
	{
		Bow::Core::Mesh mesh;

		Bow::Core::VertexAttributeFloatVec3 *positionsAttribute = new Bow::Core::VertexAttributeFloatVec3("in_Position");
		mesh.AddAttribute(Bow::Core::VertexAttributePtr(positionsAttribute));

		Bow::Core::VertexAttributeFloat *colorAttribute = new Bow::Core::VertexAttributeFloat("in_Color");
		mesh.AddAttribute(Bow::Core::VertexAttributePtr(colorAttribute));

		for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
		{
			RecursRender(&(m_RootTriangles[i]), positionsAttribute, colorAttribute, 99);
		}

		m_VertexArray = renderContext->VCreateVertexArray(mesh, m_shaderProgram->VGetVertexAttributes(), Bow::Renderer::BufferHint::StaticDraw);
		m_isDirty = false;
	}

	Bow::Core::Matrix3D<float> modelViewProj = (Bow::Core::Matrix3D<float>)camera->CalculateWorldViewProjection(m_objectWorldMat);
	m_shaderProgram->VSetUniform("u_ModelViewProj", (Bow::Core::Matrix4x4<float>)modelViewProj);

	if (renderFilled)
	{
		m_renderState.RasterizationMode = Bow::Renderer::RasterizationMode::Fill;
	}
	else
	{
		m_renderState.RasterizationMode = Bow::Renderer::RasterizationMode::Line;
	}

	renderContext->VDraw(Bow::Renderer::PrimitiveType::Triangles, m_VertexArray, m_shaderProgram, m_renderState);
}

void WaveletSphere::Update(float deltaTime, Bow::Core::Ray<double> mouseRay)
{
	//m_objectWorldMat.RotateZ(0.05f * deltaTime);

	mouseRay.Transform(m_objectWorldMat);

	for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
	{
		SubdivideIntersection(&(m_RootTriangles[i]), mouseRay);
	}
}

void WaveletSphere::ComputeFromOctahedron(int numberOfSubdivisions)
{
	Bow::Core::Vector3<double> vCenter = Bow::Core::Vector3<double>(1.0, 0.0, 0.0);

	Bow::Core::Vector3<double> v0 = Bow::Core::Vector3<double>(1.0, 0.0, 0.0); v0.Normalize();
	Bow::Core::Vector3<double> v1 = Bow::Core::Vector3<double>(0.0, -1.0, 0.0); v1.Normalize();
	Bow::Core::Vector3<double> v2 = Bow::Core::Vector3<double>(-1.0, 0.0, 0.0); v2.Normalize();
	Bow::Core::Vector3<double> v3 = Bow::Core::Vector3<double>(0.0, 1.0, 0.0); v3.Normalize();
	Bow::Core::Vector3<double> v4 = Bow::Core::Vector3<double>(0.0, 0.0, 1.0); v4.Normalize();
	Bow::Core::Vector3<double> v5 = Bow::Core::Vector3<double>(0.0, 0.0, -1.0); v5.Normalize();

	m_RootTriangles.push_back(WaveletTriangle(v0, v5, v3));
	m_RootTriangles.push_back(WaveletTriangle(v4, v0, v3));
	m_RootTriangles.push_back(WaveletTriangle(v2, v4, v3));
	m_RootTriangles.push_back(WaveletTriangle(v5, v2, v3));
	m_RootTriangles.push_back(WaveletTriangle(v5, v0, v1));
	m_RootTriangles.push_back(WaveletTriangle(v2, v5, v1));
	m_RootTriangles.push_back(WaveletTriangle(v4, v2, v1));
	m_RootTriangles.push_back(WaveletTriangle(v0, v4, v1));

	m_RootParents.push_back(WaveletTriangle());
	m_RootParents.push_back(WaveletTriangle());
	m_RootParents.push_back(WaveletTriangle());
	m_RootParents.push_back(WaveletTriangle());
	m_RootParents.push_back(WaveletTriangle());
	m_RootParents.push_back(WaveletTriangle());
	m_RootParents.push_back(WaveletTriangle());
	m_RootParents.push_back(WaveletTriangle());

	m_RootParents[0].m_T0 = &(m_RootTriangles[0]);
	m_RootParents[0].m_T1 = &(m_RootTriangles[1]);
	m_RootParents[0].m_T2 = &(m_RootTriangles[3]);
	m_RootParents[0].m_T3 = &(m_RootTriangles[4]);
	m_RootParents[0].m_T4 = m_RootParents[0].m_T5 = m_RootParents[0].m_T6 = &(m_RootParents[6]);
	m_RootTriangles[0].m_Parent = &(m_RootParents[0]);

	m_RootParents[1].m_T0 = &(m_RootTriangles[1]);
	m_RootParents[1].m_T1 = &(m_RootTriangles[2]);
	m_RootParents[1].m_T2 = &(m_RootTriangles[0]);
	m_RootParents[1].m_T3 = &(m_RootTriangles[7]);
	m_RootParents[1].m_T4 = m_RootParents[1].m_T5 = m_RootParents[1].m_T6 = &(m_RootParents[5]);
	m_RootTriangles[1].m_Parent = &(m_RootParents[1]);

	m_RootParents[2].m_T0 = &(m_RootTriangles[2]);
	m_RootParents[2].m_T1 = &(m_RootTriangles[3]);
	m_RootParents[2].m_T2 = &(m_RootTriangles[1]);
	m_RootParents[2].m_T3 = &(m_RootTriangles[6]);
	m_RootParents[2].m_T4 = m_RootParents[2].m_T5 = m_RootParents[2].m_T6 = &(m_RootParents[4]);
	m_RootTriangles[2].m_Parent = &(m_RootParents[2]);

	m_RootParents[3].m_T0 = &(m_RootTriangles[3]);
	m_RootParents[3].m_T1 = &(m_RootTriangles[0]);
	m_RootParents[3].m_T2 = &(m_RootTriangles[2]);
	m_RootParents[3].m_T3 = &(m_RootTriangles[5]);
	m_RootParents[3].m_T4 = m_RootParents[3].m_T5 = m_RootParents[3].m_T6 = &(m_RootParents[7]);
	m_RootTriangles[3].m_Parent = &(m_RootParents[3]);

	m_RootParents[4].m_T0 = &(m_RootTriangles[4]);
	m_RootParents[4].m_T1 = &(m_RootTriangles[5]);
	m_RootParents[4].m_T2 = &(m_RootTriangles[7]);
	m_RootParents[4].m_T3 = &(m_RootTriangles[0]);
	m_RootParents[4].m_T4 = m_RootParents[4].m_T5 = m_RootParents[4].m_T6 = &(m_RootParents[2]);
	m_RootTriangles[4].m_Parent = &(m_RootParents[4]);

	m_RootParents[5].m_T0 = &(m_RootTriangles[5]);
	m_RootParents[5].m_T1 = &(m_RootTriangles[6]);
	m_RootParents[5].m_T2 = &(m_RootTriangles[4]);
	m_RootParents[5].m_T3 = &(m_RootTriangles[3]);
	m_RootParents[5].m_T4 = m_RootParents[5].m_T5 = m_RootParents[5].m_T6 = &(m_RootParents[1]);
	m_RootTriangles[5].m_Parent = &(m_RootParents[5]);

	m_RootParents[6].m_T0 = &(m_RootTriangles[6]);
	m_RootParents[6].m_T1 = &(m_RootTriangles[7]);
	m_RootParents[6].m_T2 = &(m_RootTriangles[5]);
	m_RootParents[6].m_T3 = &(m_RootTriangles[2]);
	m_RootParents[6].m_T4 = m_RootParents[6].m_T5 = m_RootParents[6].m_T6 = &(m_RootParents[0]);
	m_RootTriangles[6].m_Parent = &(m_RootParents[6]);

	m_RootParents[7].m_T0 = &(m_RootTriangles[7]);
	m_RootParents[7].m_T1 = &(m_RootTriangles[4]);
	m_RootParents[7].m_T2 = &(m_RootTriangles[6]);
	m_RootParents[7].m_T3 = &(m_RootTriangles[1]);
	m_RootParents[7].m_T4 = m_RootParents[7].m_T5 = m_RootParents[7].m_T6 = &(m_RootParents[3]);
	m_RootTriangles[7].m_Parent = &(m_RootParents[7]);
	
	for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
	{
		Subdivide(&(m_RootTriangles[i]), numberOfSubdivisions);
	}
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

	m_RootTriangles.push_back(WaveletTriangle(v0, v1, v4));
	m_RootTriangles.push_back(WaveletTriangle(v0, v4, v9));
	m_RootTriangles.push_back(WaveletTriangle(v9, v4, v5));
	m_RootTriangles.push_back(WaveletTriangle(v4, v8, v5));
	m_RootTriangles.push_back(WaveletTriangle(v4, v1, v8));
	m_RootTriangles.push_back(WaveletTriangle(v8, v1, v10));
	m_RootTriangles.push_back(WaveletTriangle(v8, v10, v3));
	m_RootTriangles.push_back(WaveletTriangle(v5, v8, v3));
	m_RootTriangles.push_back(WaveletTriangle(v5, v3, v2));
	m_RootTriangles.push_back(WaveletTriangle(v2, v3, v7));
	m_RootTriangles.push_back(WaveletTriangle(v7, v3, v10));
	m_RootTriangles.push_back(WaveletTriangle(v7, v10, v6));
	m_RootTriangles.push_back(WaveletTriangle(v7, v6, v11));
	m_RootTriangles.push_back(WaveletTriangle(v11, v6, v0));
	m_RootTriangles.push_back(WaveletTriangle(v0, v6, v1));
	m_RootTriangles.push_back(WaveletTriangle(v6, v10, v1));
	m_RootTriangles.push_back(WaveletTriangle(v9, v11, v0));
	m_RootTriangles.push_back(WaveletTriangle(v9, v2, v11));
	m_RootTriangles.push_back(WaveletTriangle(v9, v5, v2));
	m_RootTriangles.push_back(WaveletTriangle(v7, v11, v2));

	for (unsigned int i = 0; i < m_RootTriangles.size(); ++i)
	{
		Subdivide(&(m_RootTriangles[i]), numberOfSubdivisions);
	}
}

void WaveletSphere::Subdivide(WaveletTriangle *triangle, int level)
{
	if (level > 0)
	{
		Bow::Core::Vector3<double> vec1 = (triangle->p0 + triangle->p1) * 0.5;
		Bow::Core::Vector3<double> vec2 = (triangle->p1 + triangle->p2) * 0.5;
		Bow::Core::Vector3<double> vec3 = (triangle->p2 + triangle->p0) * 0.5;

		vec1.Normalize();
		vec2.Normalize();
		vec3.Normalize();

		triangle->m_T0 = new WaveletTriangle(vec2, vec3, vec1);
		triangle->m_T1 = new WaveletTriangle(triangle->p0, vec1, vec3);
		triangle->m_T2 = new WaveletTriangle(vec1, triangle->p1, vec2);
		triangle->m_T3 = new WaveletTriangle(vec3, vec2, triangle->p2);

		triangle->m_T0->m_Parent = triangle->m_T1->m_Parent = triangle->m_T2->m_Parent = triangle->m_T3->m_Parent = triangle;
		
		if (triangle->m_Parent != nullptr)
		{
			triangle->m_T4 = triangle->m_Parent->m_T1;
			triangle->m_T5 = triangle->m_Parent->m_T2;
			triangle->m_T6 = triangle->m_Parent->m_T3;
		}
		else
		{
			triangle->m_T4 = triangle->m_T5 = triangle->m_T6 = nullptr;
		}

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

void WaveletSphere::RecursRender(WaveletTriangle* triangle, Bow::Core::VertexAttributeFloatVec3 *positionsAttribute, Bow::Core::VertexAttributeFloat *colorAttribute, int maxLevel)
{
	if (triangle->HasChildren() && !triangle->ChildrenAreWavelets() && maxLevel > 0)
	{
		maxLevel--;
		RecursRender(triangle->m_T0, positionsAttribute, colorAttribute, maxLevel);
		RecursRender(triangle->m_T1, positionsAttribute, colorAttribute, maxLevel);
		RecursRender(triangle->m_T2, positionsAttribute, colorAttribute, maxLevel);
		RecursRender(triangle->m_T3, positionsAttribute, colorAttribute, maxLevel);
	}
	else
	{
		positionsAttribute->Values.push_back(triangle->p0);
		positionsAttribute->Values.push_back(triangle->p1);
		positionsAttribute->Values.push_back(triangle->p2);

		colorAttribute->Values.push_back((float)(triangle->m_Value));
		colorAttribute->Values.push_back((float)(triangle->m_Value));
		colorAttribute->Values.push_back((float)(triangle->m_Value));
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

	triangle->Analysis();
}

void WaveletSphere::RecursCalculateScalar(WaveletTriangle* triangle, int forceLevel, double threshold)
{
	if (triangle->HasChildren() && triangle->ChildrenAreWavelets())
	{
		if (triangle->m_T1->m_Value * triangle->m_T1->m_Value +
			triangle->m_T2->m_Value * triangle->m_T2->m_Value +
			triangle->m_T3->m_Value * triangle->m_T3->m_Value > threshold * threshold || forceLevel > 0)
		{
			triangle->Synthesis();

			forceLevel--;
			RecursCalculateScalar(triangle->m_T0, forceLevel, threshold);
			RecursCalculateScalar(triangle->m_T1, forceLevel, threshold);
			RecursCalculateScalar(triangle->m_T2, forceLevel, threshold);
			RecursCalculateScalar(triangle->m_T3, forceLevel, threshold);
		}
	}
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

		Bow::Core::Vector3<double> center = (triangle->p0 + triangle->p1 + triangle->p2) * 1.0 / 3.0;

		texCoord = Bow::Core::Vector2<double>((atan2(center.y, center.x) / (PI * 2.0)) + 0.5, (asin(center.z) / PI) + 0.5);
		x = (int)(texCoord.x * (double)m_MapWidth);
		y = (int)(texCoord.y * (double)m_MapHeight);
		index = x + y * m_MapWidth;

		triangle->m_Value = (double)m_HeightMap[index % m_MapSize] / (double)255;
	}
}

void WaveletSphere::SubdivideIntersection(WaveletTriangle* triangle, Bow::Core::Ray<double> mouseRay)
{
	if (triangle->HasChildren())
	{
		double f;
		if (mouseRay.Intersects((Bow::Core::Triangle<double>)*(triangle), false, &f))
		{
			triangle->Synthesis();

			SubdivideIntersection(triangle->m_T0, mouseRay);
			SubdivideIntersection(triangle->m_T1, mouseRay);
			SubdivideIntersection(triangle->m_T2, mouseRay);
			SubdivideIntersection(triangle->m_T3, mouseRay);

			m_isDirty = true;
		}
	}
}
