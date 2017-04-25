#pragma once
#include "BowMath.h"

class WaveletFace;

template <typename T> class WaveletEdge
{
public:
	WaveletEdge(std::shared_ptr<Bow::Core::Vector3<T>> &p1, std::shared_ptr<Bow::Core::Vector3<T>> &p2)
	{
		m_p1 = p1;
		m_p2 = p2;

		m_child1 = nullptr;
		m_child2 = nullptr;
		m_parent = nullptr;

		m_T1 = nullptr;
		m_T2 = nullptr;

		m_center = nullptr;
	}

	std::shared_ptr<Bow::Core::Vector3<T>> Split(bool normalizeCenter = false)
	{
		if (m_center == nullptr)
		{
			m_center = std::shared_ptr<Bow::Core::Vector3<T>>(new Bow::Core::Vector3<T>((*m_p1 + *m_p2) * 0.5));
			m_child1 = std::shared_ptr<WaveletEdge>(new WaveletEdge(m_p1, m_center));
			m_child2 = std::shared_ptr<WaveletEdge>(new WaveletEdge(m_center, m_p2));

			m_child1->m_parent = m_child2->m_parent = this;
		}

		if (normalizeCenter)
		{
			m_center->Normalize();
		}

		return m_center;
	}

	void AddEdgeFace(WaveletFace *face)
	{
		if (face != m_T1 && face != m_T2)
		{
			if (m_T1 == nullptr)
			{
				m_T1 = face;
			}
			else if (m_T2 == nullptr)
			{
				m_T2 = face;
			}
			else
			{
				LOG_ASSERT(true, "No more faces allowed!");
			}
		}
	}

	void CorrectConcaveCurvature()
	{
		if (m_center != nullptr)
		{
			Bow::Core::Vector3<T> targetPosition = ((*m_p1 + *m_p2) * 0.5);
			if (m_center->LengthSquared() < targetPosition.LengthSquared())
			{
				*(m_center) = targetPosition;
			}
		}

		if (m_child1 != nullptr)
		{
			m_child1->CorrectConcaveCurvature();
		}

		if (m_child2 != nullptr)
		{
			m_child2->CorrectConcaveCurvature();
		}
	}

	std::shared_ptr<WaveletEdge> m_child1, m_child2;
	WaveletEdge *m_parent;
	WaveletFace *m_T1, *m_T2;
	std::shared_ptr<Bow::Core::Vector3<T>> m_p1, m_p2, m_center;
};

class WaveletFace
{
public:
	WaveletFace();
	WaveletFace(std::shared_ptr<WaveletEdge<float>> &e0, std::shared_ptr<WaveletEdge<float>> &e1, std::shared_ptr<WaveletEdge<float>> &e2);
	~WaveletFace();

	bool HasChildren();
	bool ChildrenAreWavelets();
	bool IsWavelet();
	bool IsNormalized();

	float GetArea();

	void Analysis();
	void Synthesis(bool normalize = false);

	std::shared_ptr<Bow::Core::Vector3<float>> m_p0, m_p1, m_p2;
	std::shared_ptr<WaveletEdge<float>> m_e0, m_e1, m_e2;
	float *m_Value;

	// Parent
	WaveletFace	*m_Parent;

	// Children
	WaveletFace	*m_T0;
	WaveletFace	*m_T1;
	WaveletFace	*m_T2;
	WaveletFace	*m_T3;

private:
	void SplitNeignbor(WaveletEdge<float> *edge);
	void CorrectConcaveCurvature();
	void SetEdges(std::shared_ptr<WaveletEdge<float>> &e0, std::shared_ptr<WaveletEdge<float>> &e1, std::shared_ptr<WaveletEdge<float>> &e2);

	Bow::Core::Matrix4x4<float> CalculateAnalysisMatrix();
	Bow::Core::Matrix4x4<float> CalculateSynthesisMatrix();
};
