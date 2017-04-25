#include "WaveletFace.h"

WaveletFace::WaveletFace()
{
	p0 = nullptr;
	p1 = nullptr;
	p2 = nullptr;
	m_Value = nullptr;

	// Parent
	m_Parent = nullptr;

	// Children
	m_T0 = nullptr;
	m_T1 = nullptr;
	m_T2 = nullptr;
	m_T3 = nullptr;

	// Neighbors
	m_T4 = nullptr;
	m_T5 = nullptr;
	m_T6 = nullptr;
}

WaveletFace::WaveletFace(std::shared_ptr<Bow::Core::Vector3<float>> &v0, std::shared_ptr<Bow::Core::Vector3<float>> &v1, std::shared_ptr<Bow::Core::Vector3<float>> &v2)
{
	p0 = v0;
	p1 = v1;
	p2 = v2;
	m_Value = nullptr;

	// Parent
	m_Parent = nullptr;

	// Children
	m_T0 = nullptr;
	m_T1 = nullptr;
	m_T2 = nullptr;
	m_T3 = nullptr;

	// Neighbors
	m_T4 = nullptr;
	m_T5 = nullptr;
	m_T6 = nullptr;
}


WaveletFace::~WaveletFace()
{
}

void WaveletFace::SetVertices(std::shared_ptr<Bow::Core::Vector3<float>> &v0, std::shared_ptr<Bow::Core::Vector3<float>> &v1, std::shared_ptr<Bow::Core::Vector3<float>> &v2)
{
	p0 = v0;
	p1 = v1;
	p2 = v2;
}

bool WaveletFace::HasChildren()
{
	return m_T0 != nullptr && m_T1 != nullptr && m_T2 != nullptr && m_T3 != nullptr;
}

bool WaveletFace::ChildrenAreWavelets()
{
	if (!HasChildren())
	{
		return false;
	}

	return	m_T0->p0 == nullptr && m_T0->p1 == nullptr && m_T0->p2 == nullptr &&
		m_T1->p0 == nullptr && m_T1->p1 == nullptr && m_T1->p2 == nullptr &&
		m_T2->p0 == nullptr && m_T2->p1 == nullptr && m_T2->p2 == nullptr &&
		m_T3->p0 == nullptr && m_T3->p1 == nullptr && m_T3->p2 == nullptr;
}

bool WaveletFace::IsWavelet()
{
	return	p0 == nullptr && p1 == nullptr && p2 == nullptr;
}

bool WaveletFace::IsNormalized()
{
	return	((p0->Length() + p1->Length() + p2->Length()) - 3.0) < FLT_EPSILON;
}

float WaveletFace::GetArea()
{
	Bow::Core::Vector3<float> b = (*p0 + *p1) * 0.5f;
	Bow::Core::Vector3<float> h = *p2 - b;
	return 0.5f * (b.Length() * h.Length());
}

void WaveletFace::Analysis()
{
	if (!HasChildren() || ChildrenAreWavelets())
		return;

	Bow::Core::Matrix4x4<float> mat = CalculateAnalysisMatrix();
	Bow::Core::Vector4<float> scalar(*(m_T0->m_Value), *(m_T1->m_Value), *(m_T2->m_Value), *(m_T3->m_Value));
	Bow::Core::Vector4<float> wavelet = mat * scalar;

	*(m_T0->m_Value) = wavelet.a[0];
	*(m_T1->m_Value) = wavelet.a[1];
	*(m_T2->m_Value) = wavelet.a[2];
	*(m_T3->m_Value) = wavelet.a[3];

	m_T0->p0 = nullptr;
	m_T0->p1 = nullptr;
	m_T0->p2 = nullptr;

	m_T1->p0 = nullptr;
	m_T1->p1 = nullptr;
	m_T1->p2 = nullptr;

	m_T2->p0 = nullptr;
	m_T2->p1 = nullptr;
	m_T2->p2 = nullptr;

	m_T3->p0 = nullptr;
	m_T3->p1 = nullptr;
	m_T3->p2 = nullptr;
}

void WaveletFace::Synthesis()
{
	if (m_Parent != nullptr && IsWavelet())
	{
		m_Parent->Synthesis();
	}

	if (HasChildren() || ChildrenAreWavelets())
	{
		std::shared_ptr<Bow::Core::Vector3<float>> v0;
		std::shared_ptr<Bow::Core::Vector3<float>> v1;
		std::shared_ptr<Bow::Core::Vector3<float>> v2;

		if (m_T4 != nullptr)
			v0 = m_T4->Synthesis(this);
		if (v0 == nullptr)
			v0 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>((*p1 + *p2) * 0.5f));
		
		if(m_T5 != nullptr)
			v1 = m_T5->Synthesis(this);
		if (v1 == nullptr)
			v1 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>((*p2 + *p0) * 0.5f));
			
		if(m_T6 != nullptr)
			v2 = m_T6->Synthesis(this);
		if (v2 == nullptr)
			v2 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>((*p0 + *p1) * 0.5f));

		v0->Normalize();
		v1->Normalize();
		v2->Normalize();

		m_T0->SetVertices(v0, v1, v2);
		m_T1->SetVertices(p0, v2, v1);
		m_T2->SetVertices(v2, p1, v0);
		m_T3->SetVertices(v1, v0, p2);

		Bow::Core::Matrix4x4<float> mat = CalculateSynthesisMatrix();
		Bow::Core::Vector4<float> wavelet(*(m_T0->m_Value), *(m_T1->m_Value), *(m_T2->m_Value), *(m_T3->m_Value));
		Bow::Core::Vector4<float> scalar = mat * wavelet;

		*(m_T0->m_Value) = scalar[0];
		*(m_T1->m_Value) = scalar[1];
		*(m_T2->m_Value) = scalar[2];
		*(m_T3->m_Value) = scalar[3];
	}
}

std::shared_ptr<Bow::Core::Vector3<float>> WaveletFace::Synthesis(WaveletFace *neighbour)
{
	if(!HasChildren())
		return nullptr;

	if (m_Parent != nullptr && IsWavelet())
	{
		m_Parent->Synthesis();
	}

	if (ChildrenAreWavelets() && !IsWavelet())
	{
		std::shared_ptr<Bow::Core::Vector3<float>> v0 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>((*p1 + *p2) * 0.5f));
		std::shared_ptr<Bow::Core::Vector3<float>> v1 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>((*p2 + *p0) * 0.5f));
		std::shared_ptr<Bow::Core::Vector3<float>> v2 = std::shared_ptr<Bow::Core::Vector3<float>>(new Bow::Core::Vector3<float>((*p0 + *p1) * 0.5f));

		m_T0->SetVertices(v0, v1, v2);
		m_T1->SetVertices(p0, v2, v1);
		m_T2->SetVertices(v2, p1, v0);
		m_T3->SetVertices(v1, v0, p2);

		Bow::Core::Matrix4x4<float> mat = CalculateSynthesisMatrix();
		Bow::Core::Vector4<float> wavelet(*(m_T0->m_Value), *(m_T1->m_Value), *(m_T2->m_Value), *(m_T3->m_Value));
		Bow::Core::Vector4<float> scalar = mat * wavelet;

		*(m_T0->m_Value) = scalar[0];
		*(m_T1->m_Value) = scalar[1];
		*(m_T2->m_Value) = scalar[2];
		*(m_T3->m_Value) = scalar[3];
	}

	if (neighbour == m_T4)
		return m_T0->p0;
	else if(neighbour == m_T5)
		return m_T0->p1;
	else if (neighbour == m_T6)
		return m_T0->p2;
	else
		return nullptr;
}

Bow::Core::Matrix4x4<float> WaveletFace::CalculateAnalysisMatrix()
{
	Bow::Core::Matrix4x4<float> mat = CalculateSynthesisMatrix();
	return mat.Inverse();
}

Bow::Core::Matrix4x4<float> WaveletFace::CalculateSynthesisMatrix()
{
	Bow::Core::Matrix4x4<float> mat;
	mat.SetIdentity();
	mat *= 2.0;

	float area_child_1 = m_T0->GetArea();

	mat.m[0][0] = 1.0f;
	mat.m[1][0] = 1.0f;
	mat.m[2][0] = 1.0f;
	mat.m[3][0] = 1.0f;

	mat.m[0][1] = -2.0f * m_T1->GetArea() / area_child_1;
	mat.m[0][2] = -2.0f * m_T2->GetArea() / area_child_1;
	mat.m[0][3] = -2.0f * m_T3->GetArea() / area_child_1;

	return mat;
}
