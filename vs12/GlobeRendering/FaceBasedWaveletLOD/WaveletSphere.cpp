#include "WaveletSphere.h"
#include "BowBitmap.h"

Bow::Core::Bitmap g_map;

WaveletSphere::WaveletSphere()
{
	m_objectWorldMat.Translate(Bow::Core::Vector3<float>(0.0f, 0.0f, 0.0f));
	m_objectWorldMat.RotateX(-(M_PI * 0.5f));
	m_isDirty = true;

	m_NumberOfRootFaces = 0;

	m_Faces = nullptr;
	m_Values = nullptr;
}

WaveletSphere::~WaveletSphere()
{
	delete[] m_Faces;
	delete[] m_Values;
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

	for (unsigned int i = 0; i < m_NumberOfRootFaces; ++i)
	{
		FillDataRecursive(&(m_Faces[i]));
	}

	m_isDirty = true;
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

		for (unsigned int i = 0; i < m_NumberOfRootFaces; ++i)
		{
			RecursRender(&(m_Faces[i]), positionsAttribute, colorAttribute);
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
}

void WaveletSphere::Subdivide(Bow::Renderer::Camera* camera, Bow::Core::Ray<double> mouseRay)
{
	mouseRay.Transform(m_objectWorldMat);

	for (unsigned int i = 0; i < m_NumberOfRootFaces; ++i)
	{
		RecursCalculateWavelet(&(m_Faces[i]));
	}

	Bow::Core::Frustum<double> frustum = camera->CalculateFrustum(m_objectWorldMat);

	for (unsigned int i = 0; i < m_NumberOfRootFaces; ++i)
	{
		RecursCalculateScalar(&(m_Faces[i]), &frustum, 4, 0.0);
	}

	m_isDirty = true;
}

void WaveletSphere::ComputeFromOctahedron(int numberOfSubdivisions)
{
	LOG_ASSERT(numberOfSubdivisions >= 0, "numberOfSubdivisions has to be 0 or bigger");

	m_NumberOfRootFaces = 8;

	unsigned int numberOfTriangles = m_NumberOfRootFaces * NumberOfTriangles(numberOfSubdivisions);
	unsigned int numberOfValues = m_NumberOfRootFaces * (unsigned int)pow(4, numberOfSubdivisions);
	unsigned int currentFaceIndex = 0;
	unsigned int currentValueIndex = 0;
	unsigned int currentVertexIndex = 0;

	if (m_Faces)
		delete[] m_Faces;

	if (m_Values)
		delete[] m_Values;

	m_Faces = new WaveletFace[numberOfTriangles];
	m_Values = new double[numberOfValues];

	memset(m_Values, 0, sizeof(double) * numberOfValues);

	std::shared_ptr<Bow::Core::Vector3<double>> v0 = std::shared_ptr<Bow::Core::Vector3<double>>(new Bow::Core::Vector3<double>(1.0, 0.0, 0.0));
	std::shared_ptr<Bow::Core::Vector3<double>> v1 = std::shared_ptr<Bow::Core::Vector3<double>>(new Bow::Core::Vector3<double>(0.0, -1.0, 0.0));
	std::shared_ptr<Bow::Core::Vector3<double>> v2 = std::shared_ptr<Bow::Core::Vector3<double>>(new Bow::Core::Vector3<double>(-1.0, 0.0, 0.0));
	std::shared_ptr<Bow::Core::Vector3<double>> v3 = std::shared_ptr<Bow::Core::Vector3<double>>(new Bow::Core::Vector3<double>(0.0, 1.0, 0.0));
	std::shared_ptr<Bow::Core::Vector3<double>> v4 = std::shared_ptr<Bow::Core::Vector3<double>>(new Bow::Core::Vector3<double>(0.0, 0.0, 1.0));
	std::shared_ptr<Bow::Core::Vector3<double>> v5 = std::shared_ptr<Bow::Core::Vector3<double>>(new Bow::Core::Vector3<double>(0.0, 0.0, -1.0));

	// Upper Edges
	std::shared_ptr<WaveletEdge<double>> e0 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v4, v0));
	std::shared_ptr<WaveletEdge<double>> e1 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v4, v3));
	std::shared_ptr<WaveletEdge<double>> e2 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v4, v2));
	std::shared_ptr<WaveletEdge<double>> e3 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v4, v1));

	// Center Edges
	std::shared_ptr<WaveletEdge<double>> e4 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v0, v3));
	std::shared_ptr<WaveletEdge<double>> e5 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v3, v2));
	std::shared_ptr<WaveletEdge<double>> e6 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v2, v1));
	std::shared_ptr<WaveletEdge<double>> e7 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v1, v0));

	// Lower Edges
	std::shared_ptr<WaveletEdge<double>> e8 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v5, v0));
	std::shared_ptr<WaveletEdge<double>> e9 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v5, v3));
	std::shared_ptr<WaveletEdge<double>> e10 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v5, v2));
	std::shared_ptr<WaveletEdge<double>> e11 = std::shared_ptr<WaveletEdge<double>>(new WaveletEdge<double>(v5, v1));

	m_Faces[currentFaceIndex] = WaveletFace(e1, e0, e4);
	e1->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e0->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e4->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	m_Faces[currentFaceIndex] = WaveletFace(e2, e1, e5);
	e2->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e1->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e5->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	m_Faces[currentFaceIndex] = WaveletFace(e3, e2, e6);
	e3->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e2->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e6->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	m_Faces[currentFaceIndex] = WaveletFace(e0, e3, e7);
	e0->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e3->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e7->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	m_Faces[currentFaceIndex] = WaveletFace(e8, e9, e4);
	e8->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e9->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e4->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	m_Faces[currentFaceIndex] = WaveletFace(e9, e10, e5);
	e9->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e10->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e5->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	m_Faces[currentFaceIndex] = WaveletFace(e10, e11, e6);
	e10->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e11->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e6->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	m_Faces[currentFaceIndex] = WaveletFace(e11, e8, e7);
	e11->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e8->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e7->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	// Fill Array iterative (not recursive) to have higher levels on 
	// the start of the array and lower levels at the end to improve iteration speed
	for (unsigned int i = 0; i < m_NumberOfRootFaces * NumberOfTriangles(numberOfSubdivisions-1); ++i)
	{
		Subdivide(&(m_Faces[i]), currentFaceIndex, currentValueIndex);
	}
}

void WaveletSphere::Subdivide(WaveletFace *face, unsigned int &currentFaceIndex, unsigned int &currentValueIndex)
{
	face->m_T0 = &(m_Faces[currentFaceIndex]);
	m_Faces[currentFaceIndex++] = WaveletFace();

	face->m_T1 = &(m_Faces[currentFaceIndex]);
	m_Faces[currentFaceIndex++] = WaveletFace();

	face->m_T2 = &(m_Faces[currentFaceIndex]);
	m_Faces[currentFaceIndex++] = WaveletFace();

	face->m_T3 = &(m_Faces[currentFaceIndex]);
	m_Faces[currentFaceIndex++] = WaveletFace();

	face->m_T0->m_Value = face->m_Value;
	face->m_T1->m_Value = &(m_Values[currentValueIndex++]);
	face->m_T2->m_Value = &(m_Values[currentValueIndex++]);
	face->m_T3->m_Value = &(m_Values[currentValueIndex++]);

	face->m_T0->m_Parent = face->m_T1->m_Parent = face->m_T2->m_Parent = face->m_T3->m_Parent = face;
}

void WaveletSphere::RecursRender(WaveletFace* face, Bow::Core::VertexAttributeFloatVec3 *positionsAttribute, Bow::Core::VertexAttributeFloat *colorAttribute)
{
	if (face->HasChildren() && !face->ChildrenAreWavelets())
	{
		RecursRender(face->m_T0, positionsAttribute, colorAttribute);
		RecursRender(face->m_T1, positionsAttribute, colorAttribute);
		RecursRender(face->m_T2, positionsAttribute, colorAttribute);
		RecursRender(face->m_T3, positionsAttribute, colorAttribute);
	}
	else
	{
		positionsAttribute->Values.push_back(*(face->m_p0));
		positionsAttribute->Values.push_back(*(face->m_p1));
		positionsAttribute->Values.push_back(*(face->m_p2));

		colorAttribute->Values.push_back((float)*face->m_Value);
		colorAttribute->Values.push_back((float)*face->m_Value);
		colorAttribute->Values.push_back((float)*face->m_Value);
	}
}

void WaveletSphere::RecursCalculateWavelet(WaveletFace* triangle)
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

void WaveletSphere::RecursCalculateScalar(WaveletFace* triangle, Bow::Core::Frustum<double>* frustum, int forceLevel, float threshold)
{
	if (triangle->HasChildren())
	{
		if ((*triangle->m_T1->m_Value) * (*triangle->m_T1->m_Value) +
			(*triangle->m_T2->m_Value) * (*triangle->m_T2->m_Value) +
			(*triangle->m_T3->m_Value) * (*triangle->m_T3->m_Value) > threshold * threshold || forceLevel > 0)
		{
			if (forceLevel > 0 || frustum->IsTriangleVisible(triangle->m_p0->Normalized(), triangle->m_p1->Normalized(), triangle->m_p2->Normalized()))
			{
				triangle->Synthesis(true);

				forceLevel--;
				RecursCalculateScalar(triangle->m_T0, frustum, forceLevel, threshold);
				RecursCalculateScalar(triangle->m_T1, frustum, forceLevel, threshold);
				RecursCalculateScalar(triangle->m_T2, frustum, forceLevel, threshold);
				RecursCalculateScalar(triangle->m_T3, frustum, forceLevel, threshold);
			}
		}
	}
}

void WaveletSphere::FillDataRecursive(WaveletFace* face)
{
	face->m_p0->Normalize();
	face->m_p1->Normalize();
	face->m_p2->Normalize();

	if (face->HasChildren())
	{
		face->Synthesis();

		FillDataRecursive(face->m_T0);
		FillDataRecursive(face->m_T1);
		FillDataRecursive(face->m_T2);
		FillDataRecursive(face->m_T3);

		face->Analysis();
	} 
	else if (!face->HasChildren())
	{
		double PI = 3.1415926535897931;
		Bow::Core::Vector2<double> texCoord;
		int x, y, index;

		Bow::Core::Vector3<double> center = (*face->m_p0 + *face->m_p1 + *face->m_p2) * 1.0 / 3.0;
		center.Normalize();

		texCoord = Bow::Core::Vector2<double>((atan2(center.y, center.x) / (PI * 2.0)) + 0.5, (asin(center.z) / PI) + 0.5);
		x = (int)(texCoord.x * (double)m_MapWidth);
		y = (int)(texCoord.y * (double)m_MapHeight);
		index = x + y * m_MapWidth;

		*face->m_Value = (double)m_HeightMap[index % m_MapSize] / (double)255;
	}
}

unsigned int WaveletSphere::NumberOfTriangles(int numberOfSubdivisions)
{
	unsigned int numberOfTriangles = 0;
	for (int i = 0; i <= numberOfSubdivisions; ++i)
	{
		numberOfTriangles += (unsigned int)(pow(4, i));
	}

	return numberOfTriangles;
}
