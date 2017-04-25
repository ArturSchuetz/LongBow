#pragma once
#include "BowMath.h"

class WaveletFace
{
public:
	WaveletFace();
	WaveletFace(std::shared_ptr<Bow::Core::Vector3<float>> &v0, std::shared_ptr<Bow::Core::Vector3<float>> &v1, std::shared_ptr<Bow::Core::Vector3<float>> &v2);
	~WaveletFace();

	bool HasChildren();
	bool ChildrenAreWavelets();
	bool IsWavelet();
	bool IsNormalized();

	float GetArea();

	void Analysis();
	void Synthesis();

	std::shared_ptr<Bow::Core::Vector3<float>> p0, p1, p2;
	float *m_Value;

	// Parent
	WaveletFace	*m_Parent;

	// Children
	WaveletFace	*m_T0;
	WaveletFace	*m_T1;
	WaveletFace	*m_T2;
	WaveletFace	*m_T3;

	// Neighbors
	WaveletFace	*m_T4;
	WaveletFace	*m_T5;
	WaveletFace	*m_T6;

private:
	std::shared_ptr<Bow::Core::Vector3<float>> Synthesis(WaveletFace *neighbour);
	void SetVertices(std::shared_ptr<Bow::Core::Vector3<float>> &v0, std::shared_ptr<Bow::Core::Vector3<float>> &v1, std::shared_ptr<Bow::Core::Vector3<float>> &v2);

	Bow::Core::Matrix4x4<float> CalculateAnalysisMatrix();
	Bow::Core::Matrix4x4<float> CalculateSynthesisMatrix();
};
