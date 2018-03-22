#pragma once
#include "BowMath.h"
#include "WaveletEdge.h"

class WaveletFace
{
public:
	WaveletFace();
	~WaveletFace();
	void Release();

	float GetArea();
	bow::Vector3<float> GetFaceNormal(float height);
	void SetEdges(std::shared_ptr<WaveletEdge<float>> e0, std::shared_ptr<WaveletEdge<float>> e1, std::shared_ptr<WaveletEdge<float>> e2);
	void CreateChildren(float wavelet1, float wavelet2, float wavelet3);
	void ReleaseChildren();
	bool HasChildren();

	std::shared_ptr<WaveletVertex<float>>	m_p0, m_p1, m_p2;
	std::shared_ptr<WaveletEdge<float>>		m_e0, m_e1, m_e2;

	void Analysis();
	void Synthesis();

	float m_Value;
	unsigned int m_Index;

	// Parent
	WaveletFace	*m_Parent;

	// Children
	WaveletFace	*m_T0;
	WaveletFace	*m_T1;
	WaveletFace	*m_T2;
	WaveletFace	*m_T3;

private:
	bow::Matrix4x4<float> CalculateAnalysisMatrix();
	bow::Matrix4x4<float> CalculateSynthesisMatrix();
};
