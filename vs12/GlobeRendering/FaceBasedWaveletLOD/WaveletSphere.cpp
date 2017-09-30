#include "WaveletSphere.h"
#include "WaveletFace.h"
#include "BowClipmap.h"
#include "FirstPersonCamera.h"

#include <memory>
#include <sstream>
#include <iostream>

#define WAVELET_TYPE short
#define WAVELET_TYPE_MAX_VALUE SHORT_MAX

WaveletSphere::WaveletSphere()
{
	m_objectWorldMat.Translate(Bow::Core::Vector3<float>(0.0f, 0.0f, 0.0f));
	m_objectWorldMat.RotateX(-(M_PI * 0.5f));
	m_percentage = -1.0f;
	m_isDirty = true;

	m_realtime = true;
	m_interpolateFaces = false;
	m_renderHeight = true;
	m_updateVertexAttributes = true;
	m_renderTextured = false;

	m_NumberOfRootFaces = 0;
	m_NumberOfSubdivions = 0;

	m_heightMultiplicator = 0.00138879297f; // (8.848 km) / (6371.0 km)
	m_Faces = nullptr;
}

WaveletSphere::~WaveletSphere()
{
	delete[] m_Faces;
}

void WaveletSphere::GenerateData(unsigned int subdivisions)
{
	m_NumberOfSubdivions = subdivisions;
	CalculateWaveletCoefficients(m_NumberOfSubdivions);
}

void WaveletSphere::Init(Bow::Renderer::ShaderProgramPtr shaderProgram)
{
	m_shaderProgram = shaderProgram;
}

void WaveletSphere::Update(float deltaTime, Bow::Renderer::Camera* camera)
{
	// Fill Array iterative (not recursive) to have higher levels on 
	// the start of the array and lower levels at the end to improve iteration speed

	Bow::Core::Frustum<double> frustum = camera->CalculateFrustum(m_objectWorldMat);
	Bow::Core::Vector3<float> position = m_objectWorldMat.Inverse() * ((FirstPersonCamera*)(camera))->GetPosition();

	std::ifstream waveletsFile("heighMap.dat", std::ios::binary);
	if (waveletsFile.is_open())
	{
		for (unsigned int i = 0; i < m_NumberOfRootFaces; ++i)
		{
				CaluclateScalarCoefficientsRecustive(&(m_Faces[i]), &frustum, camera, position, 4, m_NumberOfSubdivions, 0.0f, &waveletsFile);
		}			
		waveletsFile.close();
	}

	m_isDirty = true;
}

void WaveletSphere::CaluclateScalarCoefficientsRecustive(WaveletFace* triangle, Bow::Core::Frustum<double>* frustum, Bow::Renderer::Camera* camera, Bow::Core::Vector3<float> position, int minLevel, int maxLevel, float threshold, std::ifstream* inputFile)
{
	if (maxLevel > 0)
	{
		if (frustum->IsTriangleVisible(triangle->m_p0->GetVertex(m_heightMultiplicator), triangle->m_p1->GetVertex(m_heightMultiplicator), triangle->m_p2->GetVertex(m_heightMultiplicator)) || minLevel > 0)
		{		
			bool subdivide = true;
			if (m_realtime)
			{
				float fEdgeLength;
				fEdgeLength = (triangle->m_p0->GetVertex(m_heightMultiplicator) - triangle->m_p1->GetVertex(m_heightMultiplicator)).LengthSquared();
				fEdgeLength += (triangle->m_p1->GetVertex(m_heightMultiplicator) - triangle->m_p2->GetVertex(m_heightMultiplicator)).LengthSquared();
				fEdgeLength += (triangle->m_p2->GetVertex(m_heightMultiplicator) - triangle->m_p0->GetVertex(m_heightMultiplicator)).LengthSquared();
				Bow::Core::Vector3<float> center = ((triangle->m_p0->GetVertex(m_heightMultiplicator) + triangle->m_p1->GetVertex(m_heightMultiplicator) + triangle->m_p2->GetVertex(m_heightMultiplicator)) / 3);
				subdivide = fEdgeLength * 500 > (position - center).LengthSquared();
			}
			else
			{
				Bow::Core::Vector2<float> vec0 = camera->Transform3Dto2D(m_objectWorldMat * triangle->m_p0->GetVertex(m_heightMultiplicator));
				Bow::Core::Vector2<float> vec1 = camera->Transform3Dto2D(m_objectWorldMat * triangle->m_p1->GetVertex(m_heightMultiplicator));
				Bow::Core::Vector2<float> vec2 = camera->Transform3Dto2D(m_objectWorldMat * triangle->m_p2->GetVertex(m_heightMultiplicator));

				float fEdgeLength;
				fEdgeLength = (vec0 - vec1).LengthSquared();
				fEdgeLength += (vec1 - vec2).LengthSquared();
				fEdgeLength += (vec2 - vec0).LengthSquared();

				subdivide = fEdgeLength > 100.0;
			}

			if (subdivide || minLevel > 0)
			{
				float wavelet[3];
				wavelet[0] = wavelet[1] = wavelet[2] = 0.0;

				bool hasChildren = true;
				if (!triangle->HasChildren())
				{
					hasChildren = false;

					unsigned int newOffset = triangle->m_Index * 4;

					WAVELET_TYPE values[3];
					inputFile->seekg((newOffset + 1) * sizeof(WAVELET_TYPE));
					inputFile->read(reinterpret_cast<char *>(values), sizeof(WAVELET_TYPE) * 3);

					wavelet[0] = (float)(values[0] / (float)WAVELET_TYPE_MAX_VALUE);
					wavelet[1] = (float)(values[1] / (float)WAVELET_TYPE_MAX_VALUE);
					wavelet[2] = (float)(values[2] / (float)WAVELET_TYPE_MAX_VALUE);
				}

				if (hasChildren ||
					(wavelet[0] * wavelet[0]) + (wavelet[1] * wavelet[1]) + (wavelet[2] * wavelet[2]) > threshold * threshold || 
					minLevel > 0)
				{
					triangle->CreateChildren(wavelet[0], wavelet[1], wavelet[2]);

					minLevel--;
					maxLevel--;

					CaluclateScalarCoefficientsRecustive(triangle->m_T0, frustum, camera, position, minLevel, maxLevel, threshold, inputFile);
					CaluclateScalarCoefficientsRecustive(triangle->m_T1, frustum, camera, position, minLevel, maxLevel, threshold, inputFile);
					CaluclateScalarCoefficientsRecustive(triangle->m_T2, frustum, camera, position, minLevel, maxLevel, threshold, inputFile);
					CaluclateScalarCoefficientsRecustive(triangle->m_T3, frustum, camera, position, minLevel, maxLevel, threshold, inputFile);
				}
			}
			else
			{
				triangle->ReleaseChildren();
			}
		}
		else
		{
			triangle->ReleaseChildren();
		}
	}
}

void WaveletSphere::Render(Bow::Renderer::RenderContextPtr renderContext, Bow::Renderer::Texture2DPtr texture, Bow::Renderer::TextureSamplerPtr sampler, Bow::Renderer::Camera* camera, bool renderFilled)
{
	if (m_isDirty && m_updateVertexAttributes)
	{
		Bow::Core::Mesh mesh;

		Bow::Core::VertexAttributeFloatVec4 *positionsAttribute = new Bow::Core::VertexAttributeFloatVec4("in_Position");
		mesh.AddAttribute(Bow::Core::VertexAttributePtr(positionsAttribute));

		Bow::Core::VertexAttributeFloatVec3 *normalsAttribute = new Bow::Core::VertexAttributeFloatVec3("in_Normal");
		mesh.AddAttribute(Bow::Core::VertexAttributePtr(normalsAttribute));

		for (unsigned int i = 0; i < m_NumberOfRootFaces; ++i)
		{
			RecursRender(&(m_Faces[i]), positionsAttribute, normalsAttribute);
		}

		m_VertexArray = renderContext->VCreateVertexArray(mesh, m_shaderProgram->VGetVertexAttributes(), Bow::Renderer::BufferHint::StaticDraw);
		m_isDirty = false;
	}

	Bow::Core::Matrix3D<float> modelViewProj = (Bow::Core::Matrix3D<float>)camera->CalculateWorldViewProjection(m_objectWorldMat);
	Bow::Core::Matrix3D<float> modelView = (Bow::Core::Matrix3D<float>)camera->CalculateWorldView(m_objectWorldMat);

	int diffuse_Location = 0;
	renderContext->VSetTexture(diffuse_Location, texture);
	renderContext->VSetTextureSampler(diffuse_Location, sampler);
	m_shaderProgram->VSetUniform("diffuseTex", diffuse_Location); 
	m_shaderProgram->VSetUniform("u_ModelViewProj", (Bow::Core::Matrix4x4<float>)modelViewProj);
	m_shaderProgram->VSetUniform("u_ModelView", (Bow::Core::Matrix4x4<float>)modelView);
	m_shaderProgram->VSetUniform("u_LightDirection", (Bow::Core::Vector4<float>)(camera->CalculateView() * (Bow::Core::Vector4<float>(-50.0f, 25.0f, -100.0f, 1.0f))));
	m_shaderProgram->VSetUniform("u_renderMode", m_renderTextured ? 1.0f : 0.0f);
	
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

void WaveletSphere::CalculateWaveletCoefficients(int numberOfSubdivisions)
{
	LOG_ASSERT(numberOfSubdivisions >= 0, "numberOfSubdivisions has to be 0 or bigger");

	m_NumberOfRootFaces = 8;

	unsigned int currentFaceIndex = 0;

	if (m_Faces)
		delete[] m_Faces;

	m_Faces = new WaveletFace[m_NumberOfRootFaces];

	std::shared_ptr<WaveletVertex<float>> v0 = std::shared_ptr<WaveletVertex<float>>(new WaveletVertex<float>(1.0, 0.0, 0.0));
	std::shared_ptr<WaveletVertex<float>> v1 = std::shared_ptr<WaveletVertex<float>>(new WaveletVertex<float>(0.0, -1.0, 0.0));
	std::shared_ptr<WaveletVertex<float>> v2 = std::shared_ptr<WaveletVertex<float>>(new WaveletVertex<float>(-1.0, 0.0, 0.0));
	std::shared_ptr<WaveletVertex<float>> v3 = std::shared_ptr<WaveletVertex<float>>(new WaveletVertex<float>(0.0, 1.0, 0.0));
	std::shared_ptr<WaveletVertex<float>> v4 = std::shared_ptr<WaveletVertex<float>>(new WaveletVertex<float>(0.0, 0.0, 1.0));
	std::shared_ptr<WaveletVertex<float>> v5 = std::shared_ptr<WaveletVertex<float>>(new WaveletVertex<float>(0.0, 0.0, -1.0));

	// Upper Edges
	std::shared_ptr<WaveletEdge<float>> e0 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v4, v0));
	std::shared_ptr<WaveletEdge<float>> e1 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v4, v3));
	std::shared_ptr<WaveletEdge<float>> e2 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v4, v2));
	std::shared_ptr<WaveletEdge<float>> e3 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v4, v1));
	
	// Center Edges
	std::shared_ptr<WaveletEdge<float>> e4 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v0, v3));
	std::shared_ptr<WaveletEdge<float>> e5 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v3, v2));
	std::shared_ptr<WaveletEdge<float>> e6 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v2, v1));
	std::shared_ptr<WaveletEdge<float>> e7 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v1, v0));

	// Lower Edges
	std::shared_ptr<WaveletEdge<float>> e8 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v5, v0));
	std::shared_ptr<WaveletEdge<float>> e9 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v5, v3));
	std::shared_ptr<WaveletEdge<float>> e10 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v5, v2));
	std::shared_ptr<WaveletEdge<float>> e11 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v5, v1));

	m_Faces[currentFaceIndex].SetEdges(e1, e0, e4);
	m_Faces[currentFaceIndex].m_Index = 4;
	e1->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e0->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e4->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	currentFaceIndex++;

	m_Faces[currentFaceIndex].SetEdges(e2, e1, e5);
	m_Faces[currentFaceIndex].m_Index = 5;
	e2->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e1->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e5->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	currentFaceIndex++;

	m_Faces[currentFaceIndex].SetEdges(e3, e2, e6);
	m_Faces[currentFaceIndex].m_Index = 6;
	e3->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e2->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e6->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	currentFaceIndex++;

	m_Faces[currentFaceIndex].SetEdges(e0, e3, e7);
	m_Faces[currentFaceIndex].m_Index = 7;
	e0->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e3->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e7->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	currentFaceIndex++;

	m_Faces[currentFaceIndex].SetEdges(e8, e9, e4);
	m_Faces[currentFaceIndex].m_Index = 8;
	e8->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e9->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e4->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	currentFaceIndex++;

	m_Faces[currentFaceIndex].SetEdges(e9, e10, e5);
	m_Faces[currentFaceIndex].m_Index = 9;
	e9->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e10->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e5->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	currentFaceIndex++;

	m_Faces[currentFaceIndex].SetEdges(e10, e11, e6);
	m_Faces[currentFaceIndex].m_Index = 10;
	e10->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e11->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e6->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	currentFaceIndex++;

	m_Faces[currentFaceIndex].SetEdges(e11, e8, e7);
	m_Faces[currentFaceIndex].m_Index = 11;
	e11->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e8->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	e7->AddEdgeFace(&(m_Faces[currentFaceIndex]));
	currentFaceIndex++;

	m_currentCount = 0;
	if (FILE *file = fopen("heighMap.dat", "r"))
	{
		fclose(file);
	}
	else
	{
		Bow::Core::Clipmap map(4, 2);
		map.LoadFile(0, 1, "../Data/Textures/NASA/Earth/gebco_08_rev_elev_A1_grey_geo.tif");
		map.LoadFile(0, 0, "../Data/Textures/NASA/Earth/gebco_08_rev_elev_A2_grey_geo.tif");
		map.LoadFile(1, 1, "../Data/Textures/NASA/Earth/gebco_08_rev_elev_B1_grey_geo.tif");
		map.LoadFile(1, 0, "../Data/Textures/NASA/Earth/gebco_08_rev_elev_B2_grey_geo.tif");
		map.LoadFile(2, 1, "../Data/Textures/NASA/Earth/gebco_08_rev_elev_C1_grey_geo.tif");
		map.LoadFile(2, 0, "../Data/Textures/NASA/Earth/gebco_08_rev_elev_C2_grey_geo.tif");
		map.LoadFile(3, 1, "../Data/Textures/NASA/Earth/gebco_08_rev_elev_D1_grey_geo.tif");
		map.LoadFile(3, 0, "../Data/Textures/NASA/Earth/gebco_08_rev_elev_D2_grey_geo.tif");

		std::ofstream waveletsFile("heighMap.dat", std::ios::binary);
		if (waveletsFile.is_open())
		{
			for (unsigned int i = 0; i < m_NumberOfRootFaces; ++i)
			{
				CaluclateWaveletCoefficientsRecustive(m_Faces + i, numberOfSubdivisions, &map, &waveletsFile);
			}
			waveletsFile.close();
		}
	}
}

void WaveletSphere::CaluclateWaveletCoefficientsRecustive(WaveletFace* face, int level, const Bow::Core::Clipmap* heighMap, std::ofstream* outputFile)
{
	if (level > 0)
	{
		face->CreateChildren(0.0, 0.0, 0.0);

		--level;
		CaluclateWaveletCoefficientsRecustive(face->m_T0, level, heighMap, outputFile);
		CaluclateWaveletCoefficientsRecustive(face->m_T1, level, heighMap, outputFile);
		CaluclateWaveletCoefficientsRecustive(face->m_T2, level, heighMap, outputFile);
		CaluclateWaveletCoefficientsRecustive(face->m_T3, level, heighMap, outputFile);
		
		face->Analysis();

		WAVELET_TYPE values[3];

		values[0] = (WAVELET_TYPE)(face->m_T1->m_Value * (float)WAVELET_TYPE_MAX_VALUE);
		values[1] = (WAVELET_TYPE)(face->m_T2->m_Value * (float)WAVELET_TYPE_MAX_VALUE);
		values[2] = (WAVELET_TYPE)(face->m_T3->m_Value * (float)WAVELET_TYPE_MAX_VALUE);

		outputFile->seekp(face->m_T1->m_Index * sizeof(WAVELET_TYPE));
		outputFile->write(reinterpret_cast<char *>(values), sizeof(WAVELET_TYPE) * 3);

		if (values[0] != 0)
			m_notNullCount++;

		if (values[1] != 0)
			m_notNullCount++;

		if (values[2] != 0)
			m_notNullCount++;

		if (face->m_T1->m_Value > m_maxValue)
		{
			m_maxValue = face->m_T1->m_Value;
		}
		else if(face->m_T1->m_Value < m_minValue)
		{
			m_minValue = face->m_T1->m_Value;
		}

		if (face->m_T2->m_Value > m_maxValue)
		{
			m_maxValue = face->m_T2->m_Value;
		}
		else if (face->m_T2->m_Value < m_minValue)
		{
			m_minValue = face->m_T2->m_Value;
		}

		if (face->m_T3->m_Value > m_maxValue)
		{
			m_maxValue = face->m_T3->m_Value;
		}
		else if (face->m_T3->m_Value < m_minValue)
		{
			m_minValue = face->m_T3->m_Value;
		}

		face->m_Value = face->m_T0->m_Value;

		if (face->m_Parent == nullptr)
		{
			WAVELET_TYPE value = (WAVELET_TYPE)(face->m_Value * (float)WAVELET_TYPE_MAX_VALUE);
			outputFile->seekp(face->m_Index * sizeof(WAVELET_TYPE));
			outputFile->write(reinterpret_cast<char *>(&(face->m_Value)), sizeof(WAVELET_TYPE));
		}
		else
		{
			if (face->m_Parent->m_T0 != face)
			{
				WAVELET_TYPE value = (WAVELET_TYPE)(face->m_Value * (float)WAVELET_TYPE_MAX_VALUE);
				outputFile->seekp(face->m_Index * sizeof(WAVELET_TYPE));
				outputFile->write(reinterpret_cast<char *>(&(face->m_Value)), sizeof(WAVELET_TYPE));
			}
		}

		face->ReleaseChildren();
	}
	else
	{
		double PI = 3.1415926535897931;

		Bow::Core::Vector3<float> center = (face->m_p0->GetVertex(0.0) + face->m_p1->GetVertex(0.0) + face->m_p2->GetVertex(0.0)) * 1.0 / 3.0;
		center.Normalize();

		Bow::Core::Vector2<float> texCoord = Bow::Core::Vector2<float>((atan2(center.y, center.x) / (PI * 2.0)) + 0.5, (asin(center.z) / PI) + 0.5);

		float val = heighMap->GetPixel(texCoord.x, texCoord.y);
		face->m_Value = val;

		m_currentCount++;

		if (face->m_Parent->m_T0 == face)
			return;

		WAVELET_TYPE value = (WAVELET_TYPE)(face->m_Value * (float)WAVELET_TYPE_MAX_VALUE);
		outputFile->seekp(face->m_Index * sizeof(WAVELET_TYPE));
		outputFile->write(reinterpret_cast<char *>(&(face->m_Value)), sizeof(WAVELET_TYPE));

		unsigned int maxfacecount = (unsigned int)m_NumberOfRootFaces * (unsigned int)(pow(4, m_NumberOfSubdivions));
		double percentage = ((unsigned int)(((double)m_currentCount / (double)maxfacecount)*1000.0)) / 10.0;
		if (percentage != m_percentage)
		{
			m_percentage = percentage;
			std::cout << "Calculating Wavelets: " << m_currentCount << " / " << maxfacecount << " ( " << m_percentage <<  "% ) and " << (((unsigned int)(((double)m_notNullCount / (double)maxfacecount)*1000.0)) / 10.0) << "% not null " << m_minValue << " - " << m_maxValue << "\t\t\r" << std::flush;
		}
	}
}

void WaveletSphere::RecursRender(WaveletFace* face, Bow::Core::VertexAttributeFloatVec4 *positionsAttribute, Bow::Core::VertexAttributeFloatVec3 *normalsAttribute)
{
	float heightMultiplicator = m_heightMultiplicator;
	if (!m_renderHeight)
	{
		heightMultiplicator = 0.0f;
	}

	face->m_e0->CorrectConcaveCurvature(heightMultiplicator);
	face->m_e1->CorrectConcaveCurvature(heightMultiplicator);
	face->m_e2->CorrectConcaveCurvature(heightMultiplicator);

	if (face->HasChildren())
	{
		RecursRender(face->m_T0, positionsAttribute, normalsAttribute);
		RecursRender(face->m_T1, positionsAttribute, normalsAttribute);
		RecursRender(face->m_T2, positionsAttribute, normalsAttribute);
		RecursRender(face->m_T3, positionsAttribute, normalsAttribute);
	}
	else
	{
		if (!m_interpolateFaces)
		{
			positionsAttribute->Values.push_back(Bow::Core::Vector4<float>(face->m_p0->GetVertex(heightMultiplicator), face->m_Value));
			positionsAttribute->Values.push_back(Bow::Core::Vector4<float>(face->m_p1->GetVertex(heightMultiplicator), face->m_Value));
			positionsAttribute->Values.push_back(Bow::Core::Vector4<float>(face->m_p2->GetVertex(heightMultiplicator), face->m_Value));

			normalsAttribute->Values.push_back(face->GetFaceNormal(heightMultiplicator));
			normalsAttribute->Values.push_back(face->GetFaceNormal(heightMultiplicator));
			normalsAttribute->Values.push_back(face->GetFaceNormal(heightMultiplicator));
		}
		else
		{
			positionsAttribute->Values.push_back(Bow::Core::Vector4<float>(face->m_p0->GetVertex(heightMultiplicator), face->m_p0->GetHeight()));
			positionsAttribute->Values.push_back(Bow::Core::Vector4<float>(face->m_p1->GetVertex(heightMultiplicator), face->m_p1->GetHeight()));
			positionsAttribute->Values.push_back(Bow::Core::Vector4<float>(face->m_p2->GetVertex(heightMultiplicator), face->m_p2->GetHeight()));

			normalsAttribute->Values.push_back(face->GetFaceNormal(heightMultiplicator));
			normalsAttribute->Values.push_back(face->GetFaceNormal(heightMultiplicator));
			normalsAttribute->Values.push_back(face->GetFaceNormal(heightMultiplicator));
		}
	}
}