#include "WaveletFace.h"

WaveletFace::WaveletFace()
{
	m_p0 = nullptr;
	m_p1 = nullptr;
	m_p2 = nullptr;

	m_e0 = nullptr;
	m_e1 = nullptr;
	m_e2 = nullptr;

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

void WaveletFace::Release()
{
	ReleaseChildren();

	m_e0->RemoveEdgeFace(this);
	m_e1->RemoveEdgeFace(this);
	m_e2->RemoveEdgeFace(this);

	m_p0->RemoveFace(this);
	m_p1->RemoveFace(this);
	m_p2->RemoveFace(this);

	m_p0 = nullptr;
	m_p1 = nullptr;
	m_p2 = nullptr;

	m_e0 = nullptr;
	m_e1 = nullptr;
	m_e2 = nullptr;
}

void WaveletFace::ReleaseChildren()
{
	if (HasChildren())
	{
		m_T0->Release();
		m_T1->Release();
		m_T2->Release();
		m_T3->Release();

		m_p0->AddFace(this);
		m_p1->AddFace(this);
		m_p2->AddFace(this);

		delete m_T0;
		delete m_T1;
		delete m_T2;
		delete m_T3;

		m_T0 = nullptr;
		m_T1 = nullptr;
		m_T2 = nullptr;
		m_T3 = nullptr;
	}
}

void WaveletFace::SetEdges(std::shared_ptr<WaveletEdge<float>> e0, std::shared_ptr<WaveletEdge<float>> e1, std::shared_ptr<WaveletEdge<float>> e2)
{
	m_e0 = e0;
	m_e1 = e1;
	m_e2 = e2;

	if (m_p0 != nullptr)
	{
		m_p0->RemoveFace(this);
	}
	if (m_p1 != nullptr)
	{
		m_p1->RemoveFace(this);
	}
	if (m_p2 != nullptr)
	{
		m_p2->RemoveFace(this);
	} 

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

	if (m_Parent != nullptr)
	{
		m_p0->RemoveFace(m_Parent);
		m_p1->RemoveFace(m_Parent);
		m_p2->RemoveFace(m_Parent);
	}

	m_p0->AddFace(this);
	m_p1->AddFace(this);
	m_p2->AddFace(this);
}

float WaveletFace::GetArea()
{
	Bow::Core::Vector3<float> b = (m_p0->GetVertex() + m_p1->GetVertex()) * 0.5;
	Bow::Core::Vector3<float> h = m_p2->GetVertex() - b;
	return 0.5f * (b.Length() * h.Length());
}

Bow::Core::Vector3<float> WaveletFace::GetFaceNormal(float height)
{
	return (m_p1->GetVertex(height) - m_p0->GetVertex(height)).CrossP(m_p2->GetVertex(height) - m_p0->GetVertex(height));
}

void WaveletFace::CreateChildren(float wavelet1, float wavelet2, float wavelet3)
{ 
	if (!HasChildren())
	{
		unsigned int newOffset = m_Index * 4;

		m_T0 = new WaveletFace();
		m_T1 = new WaveletFace();
		m_T2 = new WaveletFace();
		m_T3 = new WaveletFace();

		m_T0->m_Index = newOffset + 0;
		m_T1->m_Index = newOffset + 1;
		m_T2->m_Index = newOffset + 2;
		m_T3->m_Index = newOffset + 3;

		m_T0->m_Value = m_Value;
		m_T1->m_Value = wavelet1;
		m_T2->m_Value = wavelet2;
		m_T3->m_Value = wavelet3;

		m_T0->m_Parent = m_T1->m_Parent = m_T2->m_Parent = m_T3->m_Parent = this;

		Synthesis();
	}
}

bool WaveletFace::HasChildren()
{
	return m_T0 != nullptr && m_T1 != nullptr && m_T2 != nullptr && m_T3 != nullptr;
}

void WaveletFace::Analysis()
{
	Bow::Core::Matrix4x4<float> mat = CalculateAnalysisMatrix();
	Bow::Core::Vector4<float> scalar(m_T0->m_Value, m_T1->m_Value, m_T2->m_Value, m_T3->m_Value);
	Bow::Core::Vector4<float> wavelet = mat * scalar;

	m_T0->m_Value = wavelet.a[0];
	m_T1->m_Value = wavelet.a[1];
	m_T2->m_Value = wavelet.a[2];
	m_T3->m_Value = wavelet.a[3];
}

void WaveletFace::Synthesis()
{
	std::shared_ptr<WaveletVertex<float>> v0 = m_e0->Split();
	std::shared_ptr<WaveletVertex<float>> v1 = m_e1->Split();
	std::shared_ptr<WaveletVertex<float>> v2 = m_e2->Split();

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
	Bow::Core::Vector4<float> wavelet(m_T0->m_Value, m_T1->m_Value, m_T2->m_Value, m_T3->m_Value);
	Bow::Core::Vector4<float> scalar = mat * wavelet;

	m_T0->m_Value = scalar[0];
	m_T1->m_Value = scalar[1];
	m_T2->m_Value = scalar[2];
	m_T3->m_Value = scalar[3];
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
