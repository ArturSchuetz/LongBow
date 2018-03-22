#pragma once
#include "BowMath.h"

template <typename T> class WaveletEdge;
class WaveletFace;

template <typename T> class WaveletVertex
{
public:
	WaveletVertex(const bow::Vector3<T>& vec)
	{
		SetVertex(vec);
		m_isDirty = true;
	}

	WaveletVertex(const T& p0, const T& p1, const T& p2)
	{
		SetVertex(bow::Vector3<T>(p0, p1, p2));
		m_isDirty = true;
	}

	void AddFace(WaveletFace *face)
	{
		auto it = find(m_faces.begin(), m_faces.end(), face);
		if (it == m_faces.end())
		{
			m_faces.push_back(face);
		}

		if (m_faces.size() > 6)
		{
			LOG_ERROR("To many Faces!");
		}
		m_isDirty = true;
	}

	void RemoveFace(WaveletFace *face)
	{
		auto it = find(m_faces.begin(), m_faces.end(), face);
		if (it != m_faces.end())
		{
			m_faces.erase(it);
		}

		if (m_faces.size() > 6)
		{
			LOG_ERROR("To many Faces!");
		}
		m_isDirty = true;
	}

	bow::Vector3<T> GetVertex(float height = -1.0f)
	{
		if (height < 0.0f)
		{
			return m_Vector.Normalized();
		}
		else
		{
			return m_Vector + (m_Vector * (GetHeight() * height));
		}
	}

	void SetVertex(const bow::Vector3<T>& vec)
	{
		m_Vector = vec;
	}

	T GetHeight()
	{
		if (m_isDirty)
		{
			if (m_faces.size() < 6)
			{
				return 0.0;
			}

			float value = 0.0f;
			unsigned int count = 0;
			for (auto it = m_faces.begin(); it != m_faces.end(); it++)
			{
				value += (*it)->m_Value;
				count++;
			}

			m_isDirty = false;
			m_Height = value / (float)count;
			return m_Height;
		}
		else
		{
			return m_Height;
		}
	}

	void SetHeight(float height = -1.0f)
	{
		if (height < -0.5f)
		{
			m_Height = 0.5f;
			m_isDirty = true;
		}
		else
		{
			m_Height = height;
			m_isDirty = false;
		}
	}

private:
	bow::Vector3<T> m_Vector;
	std::vector<WaveletFace*> m_faces;
	T m_Height;
	bool m_isDirty;
};

template <typename T> class WaveletEdge
{
public:
	WaveletEdge(std::shared_ptr<WaveletVertex<T>> &p1, std::shared_ptr<WaveletVertex<T>> &p2)
	{
		m_p1 = p1;
		m_p2 = p2;

		m_child1 = nullptr;
		m_child2 = nullptr;
		m_parent = nullptr;

		m_T1 = nullptr;
		m_T2 = nullptr;

		m_center = nullptr;
		m_isDirty = true;
	}

	~WaveletEdge()
	{
	}

	std::shared_ptr<WaveletVertex<T>> Split()
	{
		if (m_center == nullptr)
		{
			m_center = std::shared_ptr<WaveletVertex<T>>(new WaveletVertex<T>((m_p1->GetVertex() + m_p2->GetVertex()) * 0.5));
			m_center->SetVertex(m_center->GetVertex().Normalized());

			m_child1 = std::shared_ptr<WaveletEdge>(new WaveletEdge(m_p1, m_center));
			m_child2 = std::shared_ptr<WaveletEdge>(new WaveletEdge(m_center, m_p2));

			m_child1->m_parent = m_child2->m_parent = this;
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
				LOG_ERROR("No more faces allowed!");
			}
		}

		if (m_parent != nullptr)
		{
			m_parent->m_isDirty = true;
		}
	}

	void RemoveEdgeFace(WaveletFace *face)
	{
		if (m_T1 == face)
		{
			m_T1 = nullptr;
		}
		else if (m_T2 == face)
		{
			m_T2 = nullptr;
		}
		else
		{
			LOG_ERROR("There is no Face!");
		}

		if (m_parent != nullptr)
		{
			m_parent->m_isDirty = true;
		}

		if (m_T1 == nullptr && m_T2 == nullptr)
		{
			m_child1 = nullptr;
			m_child2 = nullptr;
			m_center = nullptr;
		}
	}

	void CorrectConcaveCurvature(float height = -1.0f)
	{
		if (m_isDirty && m_center != nullptr)
		{
			if (m_center->GetHeight() == 0.0f)
			{
				bow::Vector3<T> targetPosition = ((m_p1->GetVertex(height) + m_p2->GetVertex(height)) * 0.5);
				m_center->SetVertex(targetPosition);
				m_center->SetHeight(0.0f);
			}
			else
			{
				m_center->SetVertex(m_center->GetVertex().Normalized());
				m_center->SetHeight();
			}
		}
	}

	std::shared_ptr<WaveletEdge> m_child1, m_child2;
	std::shared_ptr<WaveletVertex<T>> m_p1, m_p2, m_center;

	WaveletEdge *m_parent;
	WaveletFace *m_T1, *m_T2;
	bool m_isDirty;
};
