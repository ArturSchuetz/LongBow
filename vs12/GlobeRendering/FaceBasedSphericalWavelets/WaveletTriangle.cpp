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

void WaveletTriangle::SetValue(int value)
{
	m_ValueIndex = value;
}

double WaveletTriangle::GetArea(const std::vector<Bow::Core::Vector3<float>> &vertices)
{
	Bow::Core::Vector3<double> b = ((Bow::Core::Vector3<float>)vertices[m_VertexIndices[0]] + (Bow::Core::Vector3<float>)vertices[m_VertexIndices[1]]) * 0.5;
	Bow::Core::Vector3<double> h = (Bow::Core::Vector3<float>)vertices[m_VertexIndices[2]] - b;
	return 0.5 * (b.Length() * h.Length());
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

void WaveletTriangle::Analysis(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values)
{
	AnalysisI(vertices, values);
	AnalysisII(vertices, values);
}

void WaveletTriangle::Synthesis(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values)
{
	SynthesisI(vertices, values);
	SynthesisII(vertices, values);
}

Bow::Core::Matrix4x4<double> WaveletTriangle::CalculateAnalysisMatrix(const std::vector<Bow::Core::Vector3<float>> &vertices)
{
	/*
	Bow::Core::Matrix4x4<double> mat;

	double area_p = GetArea(vertices);

	double L1 = m_T0->GetArea(vertices) / area_p;
	double L2 = m_T1->GetArea(vertices) / area_p;
	double L3 = m_T2->GetArea(vertices) / area_p;
	double L4 = m_T3->GetArea(vertices) / area_p;
	
	// scaling basis function filter coefficients
	mat.m[0][0] = L1;
	mat.m[0][1] = L2;
	mat.m[0][2] = L3;
	mat.m[0][3] = L4;

	// wavelet basis function filter coefficients
	mat.m[1][0] = -0.5 * L1;
	mat.m[1][1] = 0.5 * (1.0 - L2);
	mat.m[1][2] = -0.5 * L3;
	mat.m[1][3] = -0.5 * L4;

	mat.m[2][0] = -0.5 * L1;
	mat.m[2][1] = -0.5 * L2;
	mat.m[2][2] = 0.5 * (1.0 - L3);
	mat.m[2][3] = -0.5 * L4;

	mat.m[3][0] = -0.5 * L1;
	mat.m[3][1] = -0.5 * L2;
	mat.m[3][2] = -0.5 * L3;
	mat.m[3][3] = 0.5 * (1.0 - L4);
	*/

	Bow::Core::Matrix4x4<double> mat = CalculateSynthesisMatrix(vertices);
	return mat.Inverse();
}

Bow::Core::Matrix4x4<double> WaveletTriangle::CalculateSynthesisMatrix(const std::vector<Bow::Core::Vector3<float>> &vertices)
{
	Bow::Core::Matrix4x4<double> mat;
	mat.SetIdentity();
	mat *= 2.0;

	double area_child_1 = m_T0->GetArea(vertices);

	mat.m[0][0] = 1.0;
	mat.m[1][0] = 1.0;
	mat.m[2][0] = 1.0;
	mat.m[3][0] = 1.0;

	mat.m[0][1] = -2.0 * m_T1->GetArea(vertices) / area_child_1;
	mat.m[0][2] = -2.0 * m_T2->GetArea(vertices) / area_child_1;
	mat.m[0][3] = -2.0 * m_T3->GetArea(vertices) / area_child_1;

	return mat;
}

void WaveletTriangle::AnalysisI(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values)
{
	if (m_T0 == nullptr || m_T1 == nullptr || m_T2 == nullptr || m_T3 == nullptr)
		return;

	m_ValueIndex = m_T0->m_ValueIndex;

	Bow::Core::Matrix4x4<double> mat = CalculateAnalysisMatrix(vertices);
	Bow::Core::Vector4<double> scalar(values[m_T0->m_ValueIndex], values[m_T1->m_ValueIndex], values[m_T2->m_ValueIndex], values[m_T3->m_ValueIndex]);
	Bow::Core::Vector4<double> wavelet = mat * scalar;

	values[m_T0->m_ValueIndex] = wavelet.a[0];
	values[m_T1->m_ValueIndex] = wavelet.a[1];
	values[m_T2->m_ValueIndex] = wavelet.a[2];
	values[m_T3->m_ValueIndex] = wavelet.a[3];

	m_T0->isWavelet = true;
	m_T1->isWavelet = true;
	m_T2->isWavelet = true;
	m_T3->isWavelet = true;
}

void WaveletTriangle::AnalysisII(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values)
{
	if (m_T0 == nullptr || m_T1 == nullptr || m_T2 == nullptr || m_T3 == nullptr)
		return;
}

void WaveletTriangle::SynthesisI(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values)
{
	if (m_T0 == nullptr || m_T1 == nullptr || m_T2 == nullptr || m_T3 == nullptr)
		return;
}

void WaveletTriangle::SynthesisII(const std::vector<Bow::Core::Vector3<float>> &vertices, std::vector<double> &values)
{
	if (m_T0 == nullptr || m_T1 == nullptr || m_T2 == nullptr || m_T3 == nullptr)
		return;

	Bow::Core::Matrix4x4<double> mat = CalculateSynthesisMatrix(vertices);
	Bow::Core::Vector4<double> wavelet(values[m_T0->m_ValueIndex], values[m_T1->m_ValueIndex], values[m_T2->m_ValueIndex], values[m_T3->m_ValueIndex]);
	Bow::Core::Vector4<double> scalar = mat * wavelet;

	values[m_T0->m_ValueIndex] = scalar.a[0];
	values[m_T1->m_ValueIndex] = scalar.a[1];
	values[m_T2->m_ValueIndex] = scalar.a[2];
	values[m_T3->m_ValueIndex] = scalar.a[3];

	m_T0->isWavelet = false;
	m_T1->isWavelet = false;
	m_T2->isWavelet = false;
	m_T3->isWavelet = false;
}
