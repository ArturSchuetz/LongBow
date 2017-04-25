#include "WaveletFace.h"

WaveletFace::WaveletFace()
{
	m_p0 = nullptr;
	m_p1 = nullptr;
	m_p2 = nullptr;

	m_e0 = nullptr;
	m_e1 = nullptr;
	m_e2 = nullptr;

	m_Value = nullptr;

	// Parent
	m_Parent = nullptr;

	// Children
	m_T0 = nullptr;
	m_T1 = nullptr;
	m_T2 = nullptr;
	m_T3 = nullptr;
}

WaveletFace::WaveletFace(std::shared_ptr<WaveletEdge<float>> &e0, std::shared_ptr<WaveletEdge<float>> &e1, std::shared_ptr<WaveletEdge<float>> &e2)
{
	m_e0 = e0;
	m_e1 = e1;
	m_e2 = e2;

	if (e1->m_p1 == e2->m_p1 || e1->m_p1 == e2->m_p2)
	{
		m_p0 = e1->m_p1;
	}
	else
	{
		m_p0 = e1->m_p2;
	}

	if (e2->m_p1 == e0->m_p1 || e2->m_p1 == e0->m_p2)
	{
		m_p1 = e2->m_p1;
	}
	else
	{
		m_p1 = e2->m_p2;
	}

	if (e1->m_p1 == e0->m_p1 || e1->m_p1 == e0->m_p2)
	{
		m_p2 = e1->m_p1;
	}
	else
	{
		m_p2 = e1->m_p2;
	}

	m_Value = nullptr;

	// Parent
	m_Parent = nullptr;

	// Children
	m_T0 = nullptr;
	m_T1 = nullptr;
	m_T2 = nullptr;
	m_T3 = nullptr;
}

WaveletFace::~WaveletFace()
{
}

void WaveletFace::SetEdges(std::shared_ptr<WaveletEdge<float>> &e0, std::shared_ptr<WaveletEdge<float>> &e1, std::shared_ptr<WaveletEdge<float>> &e2)
{
	m_e0 = e0;
	m_e1 = e1;
	m_e2 = e2;


	if (e1->m_p1 == e2->m_p1 || e1->m_p1 == e2->m_p2)
	{
		m_p0 = e1->m_p1;
	}
	else
	{
		m_p0 = e1->m_p2;
	}

	if (e2->m_p1 == e0->m_p1 || e2->m_p1 == e0->m_p2)
	{
		m_p1 = e2->m_p1;
	}
	else
	{
		m_p1 = e2->m_p2;
	}

	if (e1->m_p1 == e0->m_p1 || e1->m_p1 == e0->m_p2)
	{
		m_p2 = e1->m_p1;
	}
	else
	{
		m_p2 = e1->m_p2;
	}
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

	return	m_T0->IsWavelet() && m_T1->IsWavelet() && m_T2->IsWavelet() && m_T3->IsWavelet();
}

bool WaveletFace::IsWavelet()
{
	return	m_p0 == nullptr && m_p1 == nullptr && m_p2 == nullptr;
}

bool WaveletFace::IsNormalized()
{
	return	((m_p0->Length() + m_p1->Length() + m_p2->Length()) - 3.0) < FLT_EPSILON;
}

float WaveletFace::GetArea()
{
	Bow::Core::Vector3<float> b = (*m_p0 + *m_p1) * 0.5f;
	Bow::Core::Vector3<float> h = *m_p2 - b;
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
}

void WaveletFace::Synthesis(bool normalize)
{
	if (!HasChildren())
		return;

	if (m_Parent != nullptr && IsWavelet())
	{
		m_Parent->Synthesis();
	}

	if (ChildrenAreWavelets() && !IsWavelet())
	{
		std::shared_ptr<Bow::Core::Vector3<float>> v0 = m_e0->Split();
		std::shared_ptr<Bow::Core::Vector3<float>> v1 = m_e1->Split();
		std::shared_ptr<Bow::Core::Vector3<float>> v2 = m_e2->Split();

		std::shared_ptr<WaveletEdge<float>> e0 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v1, v2));
		std::shared_ptr<WaveletEdge<float>> e1 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v2, v0));
		std::shared_ptr<WaveletEdge<float>> e2 = std::shared_ptr<WaveletEdge<float>>(new WaveletEdge<float>(v0, v1));

		m_T0->SetEdges(e0, e1, e2);
		m_T0->m_e0->AddEdgeFace(m_T0);
		m_T0->m_e1->AddEdgeFace(m_T0);
		m_T0->m_e2->AddEdgeFace(m_T0);

		if (m_e1->m_child1->m_p1 == m_p0 || m_e1->m_child1->m_p2 == m_p0)
		{
			if (m_e2->m_child1->m_p1 == m_p0 || m_e2->m_child1->m_p2 == m_p0)
			{
				m_T1->SetEdges(e0, m_e1->m_child1, m_e2->m_child1);
			}
			else
			{
				m_T1->SetEdges(e0, m_e1->m_child1, m_e2->m_child2);
			}
		}
		else
		{
			if (m_e2->m_child1->m_p1 == m_p0 || m_e2->m_child1->m_p2 == m_p0)
			{
				m_T1->SetEdges(e0, m_e1->m_child2, m_e2->m_child1);
			}
			else
			{
				m_T1->SetEdges(e0, m_e1->m_child2, m_e2->m_child2);
			}
		}
		m_T1->m_e0->AddEdgeFace(m_T1);
		m_T1->m_e1->AddEdgeFace(m_T1);
		m_T1->m_e2->AddEdgeFace(m_T1);

		if (m_e0->m_child1->m_p1 == m_p1 || m_e0->m_child1->m_p2 == m_p1)
		{
			if (m_e2->m_child1->m_p1 == m_p1 || m_e2->m_child1->m_p2 == m_p1)
			{
				m_T2->SetEdges(m_e0->m_child1, e1, m_e2->m_child1);
			}
			else
			{
				m_T2->SetEdges(m_e0->m_child1, e1, m_e2->m_child2);
			}
		}
		else
		{
			if (m_e2->m_child1->m_p1 == m_p1 || m_e2->m_child1->m_p2 == m_p1)
			{
				m_T2->SetEdges(m_e0->m_child2, e1, m_e2->m_child1);
			}
			else
			{
				m_T2->SetEdges(m_e0->m_child2, e1, m_e2->m_child2);
			}
		}
		m_T2->m_e0->AddEdgeFace(m_T2);
		m_T2->m_e1->AddEdgeFace(m_T2);
		m_T2->m_e2->AddEdgeFace(m_T2);

		if (m_e0->m_child1->m_p1 == m_p2 || m_e0->m_child1->m_p2 == m_p2)
		{
			if (m_e1->m_child1->m_p1 == m_p2 || m_e1->m_child1->m_p2 == m_p2)
			{
				m_T3->SetEdges(m_e0->m_child1, m_e1->m_child1, e2);
			}
			else
			{
				m_T3->SetEdges(m_e0->m_child1, m_e1->m_child2, e2);
			}
		}
		else
		{
			if (m_e1->m_child1->m_p1 == m_p2 || m_e1->m_child1->m_p2 == m_p2)
			{
				m_T3->SetEdges(m_e0->m_child2, m_e1->m_child1, e2);
			}
			else
			{
				m_T3->SetEdges(m_e0->m_child2, m_e1->m_child2, e2);
			}
		}
		m_T3->m_e0->AddEdgeFace(m_T3);
		m_T3->m_e1->AddEdgeFace(m_T3);
		m_T3->m_e2->AddEdgeFace(m_T3);

		Bow::Core::Matrix4x4<float> mat = CalculateSynthesisMatrix();
		Bow::Core::Vector4<float> wavelet(*(m_T0->m_Value), *(m_T1->m_Value), *(m_T2->m_Value), *(m_T3->m_Value));
		Bow::Core::Vector4<float> scalar = mat * wavelet;

		*(m_T0->m_Value) = scalar[0];
		*(m_T1->m_Value) = scalar[1];
		*(m_T2->m_Value) = scalar[2];
		*(m_T3->m_Value) = scalar[3];
	}

	if (normalize)
	{
		SplitNeignbor(m_e0.get());
		SplitNeignbor(m_e1.get());
		SplitNeignbor(m_e2.get());

		m_T0->m_p0->Normalize();
		m_T0->m_p1->Normalize();
		m_T0->m_p2->Normalize();

		if (m_e0->m_T1 == this && m_e0->m_T2 != nullptr)
			m_e0->m_T2->CorrectConcaveCurvature();
		else if (m_e0->m_T2 == this && m_e0->m_T1 != nullptr)
			m_e0->m_T1->CorrectConcaveCurvature();

		if (m_e1->m_T1 == this && m_e1->m_T2 != nullptr)
			m_e1->m_T2->CorrectConcaveCurvature();
		else if (m_e1->m_T2 == this && m_e1->m_T1 != nullptr)
			m_e1->m_T1->CorrectConcaveCurvature();

		if (m_e2->m_T1 == this && m_e2->m_T2 != nullptr)
			m_e2->m_T2->CorrectConcaveCurvature();
		else if (m_e2->m_T2 == this && m_e2->m_T1 != nullptr)
			m_e2->m_T1->CorrectConcaveCurvature();
	}
}

void WaveletFace::SplitNeignbor(WaveletEdge<float> *edge)
{
	if (edge->m_T1 == this)
	{
		if (edge->m_T2 == nullptr)
		{
			m_Parent->SplitNeignbor(edge->m_parent);
		}
		edge->m_T2->Synthesis();
	}
	else if (edge->m_T2 == this)
	{
		if (edge->m_T1 == nullptr)
		{
			m_Parent->SplitNeignbor(edge->m_parent);
		}
		edge->m_T1->Synthesis();
	}
}

void WaveletFace::CorrectConcaveCurvature()
{
	if (!IsWavelet())
	{
		m_e0->CorrectConcaveCurvature();
		m_e1->CorrectConcaveCurvature();
		m_e2->CorrectConcaveCurvature();

		if (HasChildren())
		{
			m_T0->CorrectConcaveCurvature();
			m_T1->CorrectConcaveCurvature();
			m_T2->CorrectConcaveCurvature();
			m_T3->CorrectConcaveCurvature();
		}
	}
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
