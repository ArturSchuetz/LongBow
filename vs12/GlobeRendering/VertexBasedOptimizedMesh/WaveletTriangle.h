#pragma once
#include "BowMath.h"

class WaveletTriangle
{
public:
	WaveletTriangle();
	WaveletTriangle(int n1, int n2, int n3);

	void SetVertices(int n1, int n2, int n3);

	bool HasChildren();
	bool ChildrenAreWavelets();
	bool IsWavelet();

	// Triangle members
	int	m_VertexIndices[3]; // Indices into Vertex::Array

	WaveletTriangle	*m_Parent;

	WaveletTriangle	*m_T0;
	WaveletTriangle	*m_T1;
	WaveletTriangle	*m_T2;
	WaveletTriangle	*m_T3;

	void Analysis(std::vector<float> &values);
	void Synthesis(std::vector<float> &values);

private:
	bool isWavelet;

	void AnalysisI(std::vector<float> &values);
	void AnalysisII(std::vector<float> &values);

	void SynthesisI(std::vector<float> &values);
	void SynthesisII(std::vector<float> &values);
};
