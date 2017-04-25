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

	ComputeFromOctahedron(5);
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

	mouseRay.Transform(m_objectWorldMat);

	for (unsigned int i = 0; i < m_NumberOfRootFaces; ++i)
	{
		DeselectRecursive(&(m_Faces[i]));
	}

	for (unsigned int i = 0; i < m_NumberOfRootFaces; ++i)
	{
		HighlightIntersection(&(m_Faces[i]), mouseRay);
	}
}

void WaveletSphere::Subdivide(Bow::Core::Ray<double> mouseRay)
{
	mouseRay.Transform(m_objectWorldMat);

	for (unsigned int i = 0; i < m_NumberOfRootFaces; ++i)
	{
		SubdivideIntersection(&(m_Faces[i]), mouseRay);
	}
}

void WaveletSphere::ComputeFromOctahedron(int numberOfSubdivisions)
{
	LOG_ASSERT(numberOfSubdivisions >= 0, "numberOfSubdivisions has to be 0 or bigger");

	m_NumberOfRootFaces = 4;

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
	m_Values = new float[numberOfValues];

	memset(m_Values, 0, sizeof(float) * numberOfValues);

	std::shared_ptr<Bow::Core::Vector3<float>> v0 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>(1.0, 0.0, 0.0));
	std::shared_ptr<Bow::Core::Vector3<float>> v1 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>(0.0, -1.0, 0.0));
	std::shared_ptr<Bow::Core::Vector3<float>> v2 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>(-1.0, 0.0, 0.0));
	std::shared_ptr<Bow::Core::Vector3<float>> v3 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>(0.0, 1.0, 0.0));
	std::shared_ptr<Bow::Core::Vector3<float>> v4 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>(0.0, 0.0, 1.0));
	std::shared_ptr<Bow::Core::Vector3<float>> v5 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>(0.0, 0.0, -1.0));
	
	m_Faces[currentFaceIndex] = WaveletFace(v0, v3, v4);
	m_Faces[currentFaceIndex].m_T4 = &(m_Faces[1]);
	m_Faces[currentFaceIndex].m_T5 = &(m_Faces[3]);
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	m_Faces[currentFaceIndex] = WaveletFace(v2, v4, v3);
	m_Faces[currentFaceIndex].m_T4 = &(m_Faces[0]);
	m_Faces[currentFaceIndex].m_T6 = &(m_Faces[2]);
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	m_Faces[currentFaceIndex] = WaveletFace(v4, v2, v1);
	m_Faces[currentFaceIndex].m_T6 = &(m_Faces[1]);
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	m_Faces[currentFaceIndex] = WaveletFace(v4, v1, v0);
	m_Faces[currentFaceIndex].m_T5 = &(m_Faces[0]);
	m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	//m_Faces[currentFaceIndex] = WaveletFace(v0, v1, v5);
	//m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	//m_Faces[currentFaceIndex] = WaveletFace(v1, v2, v5);
	//m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	//m_Faces[currentFaceIndex] = WaveletFace(v2, v3, v5);
	//m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	//m_Faces[currentFaceIndex] = WaveletFace(v3, v0, v5);
	//m_Faces[currentFaceIndex++].m_Value = &(m_Values[currentValueIndex++]);

	// Fill Array iterative (not recursive) to have higher levels on 
	// the start of the array and lower levels at the end to improve iteration speed
	for (unsigned int i = 0; i < m_NumberOfRootFaces * NumberOfTriangles(numberOfSubdivisions-1); ++i)
	{
		Subdivide(&(m_Faces[i]), currentFaceIndex, currentValueIndex);
	}

	for (unsigned int i = 0; i < m_NumberOfRootFaces * NumberOfTriangles(numberOfSubdivisions - 1); ++i)
	{
		ConnectNeighbors(&(m_Faces[i]));
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

void WaveletSphere::ConnectNeighbors(WaveletFace *face)
{
	face->m_T0->m_T4 = face->m_T1;
	face->m_T0->m_T5 = face->m_T2;
	face->m_T0->m_T6 = face->m_T3;

	face->m_T1->m_T4 = face->m_T0;
	face->m_T2->m_T5 = face->m_T0;
	face->m_T3->m_T6 = face->m_T0;

	if (face->m_T4 != nullptr)
	{
		face->m_T2->m_T4 = face->m_T4->m_T3;
		face->m_T3->m_T4 = face->m_T4->m_T2;
	}

	if (face->m_T5 != nullptr)
	{
		face->m_T1->m_T5 = face->m_T5->m_T3;
		face->m_T3->m_T5 = face->m_T5->m_T1;
	}

	if (face->m_T6 != nullptr)
	{
		face->m_T1->m_T6 = face->m_T6->m_T2;
		face->m_T2->m_T6 = face->m_T6->m_T1;
	}
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
		positionsAttribute->Values.push_back(*(face->p0));
		positionsAttribute->Values.push_back(*(face->p1));
		positionsAttribute->Values.push_back(*(face->p2));

		colorAttribute->Values.push_back(*face->m_Value);
		colorAttribute->Values.push_back(*face->m_Value);
		colorAttribute->Values.push_back(*face->m_Value);
	}
}

void WaveletSphere::FillDataRecursive(WaveletFace* face)
{

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

void WaveletSphere::SubdivideIntersection(WaveletFace* triangle, Bow::Core::Ray<double> mouseRay)
{
	if (triangle->HasChildren())
	{
		double f;
		if (mouseRay.Intersects(*(triangle->p0), *(triangle->p1), *(triangle->p2), false, &f))
		{
			if (triangle->ChildrenAreWavelets())
			{
				triangle->Synthesis();

				m_isDirty = true;
			}
			else
			{
				SubdivideIntersection(triangle->m_T0, mouseRay);
				SubdivideIntersection(triangle->m_T1, mouseRay);
				SubdivideIntersection(triangle->m_T2, mouseRay);
				SubdivideIntersection(triangle->m_T3, mouseRay);
			}
		}
	}
}

void WaveletSphere::HighlightIntersection(WaveletFace* triangle, Bow::Core::Ray<double> mouseRay)
{
	double f;
	if (mouseRay.Intersects(*(triangle->p0), *(triangle->p1), *(triangle->p2), false, &f))
	{
		if (triangle->HasChildren() && !triangle->ChildrenAreWavelets())
		{
			HighlightIntersection(triangle->m_T0, mouseRay);
			HighlightIntersection(triangle->m_T1, mouseRay);
			HighlightIntersection(triangle->m_T2, mouseRay);
			HighlightIntersection(triangle->m_T3, mouseRay);
		}
		else
		{
			*(triangle->m_Value) = 1.0f;
			if(triangle->m_T4 != nullptr)
				*(triangle->m_T4->m_Value) = 0.5f;
			if (triangle->m_T5 != nullptr)
				*(triangle->m_T5->m_Value) = 0.5f;
			if (triangle->m_T6 != nullptr)
				*(triangle->m_T6->m_Value) = 0.5f;
		}
	}
	m_isDirty = true;
}

void WaveletSphere::DeselectRecursive(WaveletFace* triangle)
{
	*(triangle->m_Value) = 0.0f;
	if (triangle->HasChildren() && !triangle->ChildrenAreWavelets())
	{
		DeselectRecursive(triangle->m_T0);
		DeselectRecursive(triangle->m_T1);
		DeselectRecursive(triangle->m_T2);
		DeselectRecursive(triangle->m_T3);
	}
	m_isDirty = true;
}