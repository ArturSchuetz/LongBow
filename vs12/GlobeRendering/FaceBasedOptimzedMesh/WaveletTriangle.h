#pragma once
#include "BowMath.h"

class WaveletTriangle
{
public:
	WaveletTriangle();
	WaveletTriangle(int n1, int n2, int n3);

	void SetVertices(int n1, int n2, int n3);
	void SetValue(int value);

	double GetArea(const std::vector<Bow::Core::Vector3<float>> &vertices);
	
	bool HasChildren();
	bool ChildrenAreWavelets();
	bool IsWavelet();

	// Triangle members
	int	m_VertexIndices[3]; // Indices into Vertex::Array
	int m_ValueIndex; // Indices into Value::Array

	WaveletTriangle	*m_Parent;

	WaveletTriangle	*m_T0;
	WaveletTriangle	*m_T1;
	WaveletTriangle	*m_T2;
	WaveletTriangle	*m_T3;

	void Analysis(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values);
	void Synthesis(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values);

private:
	bool isWavelet;
	Bow::Core::Matrix4x4<double> CalculateAnalysisMatrix(const std::vector<Bow::Core::Vector3<float>> &vertices);
	Bow::Core::Matrix4x4<double> CalculateSynthesisMatrix(const std::vector<Bow::Core::Vector3<float>> &vertices);

	void AnalysisI(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values);
	void AnalysisII(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values);

	void SynthesisI(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values);
	void SynthesisII(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values);
};
