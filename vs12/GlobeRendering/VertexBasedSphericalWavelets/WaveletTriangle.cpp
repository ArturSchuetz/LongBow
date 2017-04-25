#include "WaveletTriangle.h"

WaveletTriangle::WaveletTriangle()
{
	isWavelet = false;
	memset((char *)this, 0, sizeof(WaveletTriangle));
}

WaveletTriangle::WaveletTriangle(int n1, int n2, int n3)
{
	isWavelet = false;
	memset((char *)this, 0, sizeof(WaveletTriangle));

	SetVertices(n1, n2, n3);
}

void WaveletTriangle::SetVertices(int n1, int n2, int n3)
{
	m_VertexIndices[0] = n1;
	m_VertexIndices[1] = n2;
	m_VertexIndices[2] = n3;
}

bool WaveletTriangle::HasChildren()
{
	return m_T0 != nullptr && m_T1 != nullptr && m_T2 != nullptr && m_T3 != nullptr;
}

bool WaveletTriangle::ChildrenAreWavelets()
{
	bool hasChildren = m_T0 != nullptr && m_T1 != nullptr && m_T2 != nullptr && m_T3 != nullptr;
	if (!hasChildren)
	{
		return true;
	}

	return m_T0->IsWavelet() || m_T1->IsWavelet() || m_T2->IsWavelet() || m_T3->IsWavelet();
}

bool WaveletTriangle::IsWavelet()
{
	return isWavelet;
}

void WaveletTriangle::Analysis(std::vector<float> &values)
{
	AnalysisI(values);
	AnalysisII(values);
}

void WaveletTriangle::Synthesis(std::vector<float> &values)
{
	SynthesisI(values);
	SynthesisII(values);
}

void WaveletTriangle::AnalysisI(std::vector<float> &values)
{
	if (m_T0 == nullptr || m_T1 == nullptr || m_T2 == nullptr || m_T3 == nullptr)
		return;

	float vec1 = (values[m_VertexIndices[0]] + values[m_VertexIndices[1]]) * 0.5f;
	float vec2 = (values[m_VertexIndices[1]] + values[m_VertexIndices[2]]) * 0.5f;
	float vec3 = (values[m_VertexIndices[2]] + values[m_VertexIndices[0]]) * 0.5f;

	//m_T0 = new WaveletTriangle(vec1, vec2, vec3);
	values[m_T0->m_VertexIndices[0]] = values[m_T0->m_VertexIndices[0]] - vec1;
	values[m_T0->m_VertexIndices[1]] = values[m_T0->m_VertexIndices[1]] - vec2;
	values[m_T0->m_VertexIndices[2]] = values[m_T0->m_VertexIndices[2]] - vec3;

	m_T0->isWavelet = true;
	m_T1->isWavelet = true;
	m_T2->isWavelet = true;
	m_T3->isWavelet = true;
}

void WaveletTriangle::AnalysisII(std::vector<float> &values)
{
	if (m_T0 == nullptr || m_T1 == nullptr || m_T2 == nullptr || m_T3 == nullptr)
		return;
}

void WaveletTriangle::SynthesisI(std::vector<float> &values)
{
	if (m_T0 == nullptr || m_T1 == nullptr || m_T2 == nullptr || m_T3 == nullptr)
		return;
}

void WaveletTriangle::SynthesisII(std::vector<float> &values)
{
	if (m_T0 == nullptr || m_T1 == nullptr || m_T2 == nullptr || m_T3 == nullptr)
		return;

	float vec1 = (values[m_VertexIndices[0]] + values[m_VertexIndices[1]]) * 0.5f;
	float vec2 = (values[m_VertexIndices[1]] + values[m_VertexIndices[2]]) * 0.5f;
	float vec3 = (values[m_VertexIndices[2]] + values[m_VertexIndices[0]]) * 0.5f;

	//m_T0 = new WaveletTriangle(vec1, vec2, vec3);
	values[m_T0->m_VertexIndices[0]] = values[m_T0->m_VertexIndices[0]] + vec1;
	values[m_T0->m_VertexIndices[1]] = values[m_T0->m_VertexIndices[1]] + vec2;
	values[m_T0->m_VertexIndices[2]] = values[m_T0->m_VertexIndices[2]] + vec3;

	m_T0->isWavelet = false;
	m_T1->isWavelet = false;
	m_T2->isWavelet = false;
	m_T3->isWavelet = false;
}
