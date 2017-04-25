#pragma once
#include "BowMath.h"

class WaveletTriangle : public Bow::Core::Triangle<double>
{
public:
	WaveletTriangle();
	WaveletTriangle(const Bow::Core::Vector3<double> &v1, const Bow::Core::Vector3<double> &v2, const Bow::Core::Vector3<double> &v3);

	double GetArea();
	
	bool HasChildren();
	bool ChildrenAreWavelets();
	bool IsWavelet();

	double m_Value; // Indices into Value::Array

	// Parent
	WaveletTriangle	*m_Parent;

	// Children
	WaveletTriangle	*m_T0;
	WaveletTriangle	*m_T1;
	WaveletTriangle	*m_T2;
	WaveletTriangle	*m_T3;

	// Neighbors
	WaveletTriangle	*m_T4;
	WaveletTriangle	*m_T5;
	WaveletTriangle	*m_T6;

	void Analysis();
	void Synthesis();

private:
	Bow::Core::Matrix4x4<double> CalculateAnalysisMatrix();
	Bow::Core::Matrix4x4<double> CalculateSynthesisMatrix();

	bool isWavelet;
	bool isRoot;
};
