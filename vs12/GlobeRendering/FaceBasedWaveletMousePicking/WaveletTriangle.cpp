#include "WaveletTriangle.h"

WaveletTriangle::WaveletTriangle()
{
	isWavelet = false;
	isRoot = true;
	m_Parent = m_T0 = m_T1 = m_T2 = m_T3 = m_T4 = m_T5 = m_T6 = nullptr;
}

WaveletTriangle::WaveletTriangle(const Bow::Core::Vector3<double> & v1, const Bow::Core::Vector3<double> & v2, const Bow::Core::Vector3<double> &v3)
{
	isWavelet = false;
	isRoot = false;
	m_Parent = m_T0 = m_T1 = m_T2 = m_T3 = m_T4 = m_T5 = m_T6 = nullptr;

	Set(v1, v2, v3);
}

double WaveletTriangle::GetArea()
{
	Bow::Core::Vector3<double> b = (p0 + p1) * 0.5;
	Bow::Core::Vector3<double> h = p2 - b;
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

void WaveletTriangle::Analysis()
{
	if (m_T0 == nullptr || m_T1 == nullptr || m_T2 == nullptr || m_T3 == nullptr)
		return;

	if (m_T0->isWavelet && m_T1->isWavelet && m_T2->isWavelet && m_T3->isWavelet)
		return;

	Bow::Core::Matrix4x4<double> mat = CalculateAnalysisMatrix();
	Bow::Core::Vector4<double> scalar(m_T0->m_Value, m_T1->m_Value, m_T2->m_Value, m_T3->m_Value);
	Bow::Core::Vector4<double> wavelet = mat * scalar;

	m_T0->m_Value = wavelet.a[0];
	m_T1->m_Value = wavelet.a[1];
	m_T2->m_Value = wavelet.a[2];
	m_T3->m_Value = wavelet.a[3];

	m_Value = m_T0->m_Value;

	m_T0->isWavelet = true;
	m_T1->isWavelet = true;
	m_T2->isWavelet = true;
	m_T3->isWavelet = true;
}

void WaveletTriangle::Synthesis()
{
	if (m_T0 == nullptr || m_T1 == nullptr || m_T2 == nullptr || m_T3 == nullptr)
		return;

	if (!m_T0->isWavelet || !m_T1->isWavelet || !m_T2->isWavelet || !m_T3->isWavelet)
		return;

	Bow::Core::Matrix4x4<double> mat = CalculateSynthesisMatrix();
	Bow::Core::Vector4<double> wavelet(m_T0->m_Value, m_T1->m_Value, m_T2->m_Value, m_T3->m_Value);
	Bow::Core::Vector4<double> scalar = mat * wavelet;

	m_T0->m_Value = scalar.a[0];
	m_T1->m_Value = scalar.a[1];
	m_T2->m_Value = scalar.a[2];
	m_T3->m_Value = scalar.a[3];

	m_T0->isWavelet = false;
	m_T1->isWavelet = false;
	m_T2->isWavelet = false;
	m_T3->isWavelet = false;
}

Bow::Core::Matrix4x4<double> WaveletTriangle::CalculateAnalysisMatrix()
{
	Bow::Core::Matrix4x4<double> mat = CalculateSynthesisMatrix();
	return mat.Inverse();
}

Bow::Core::Matrix4x4<double> WaveletTriangle::CalculateSynthesisMatrix()
{
	Bow::Core::Matrix4x4<double> mat;
	mat.SetIdentity();
	mat *= 2.0;

	double area_child_1 = m_T0->GetArea();

	mat.m[0][0] = 1.0;
	mat.m[1][0] = 1.0;
	mat.m[2][0] = 1.0;
	mat.m[3][0] = 1.0;

	mat.m[0][1] = -2.0 * m_T1->GetArea() / area_child_1;
	mat.m[0][2] = -2.0 * m_T2->GetArea() / area_child_1;
	mat.m[0][3] = -2.0 * m_T3->GetArea() / area_child_1;

	return mat;
}