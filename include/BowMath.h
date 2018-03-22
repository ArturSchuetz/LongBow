#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector2.h"
#include "BowVector3.h"
#include "BowVector4.h"

#include "BowQuaternion.h"

#include "BowMatrix.h"
#include "BowMatrix2D.h"
#include "BowMatrix3D.h"
#include "BowMatrix3x3.h"
#include "BowMatrix4x4.h"

#include "BowSVD.h"
#include "BowAABB.h"
#include "BowPlane.h"
#include "BowRay.h"
#include "BowSphere.h"
#include "BowTriangle.h"
#include "BowFrustum.h"
#include "BowTransform.h"

namespace bow
{
	namespace math
	{
		//Taken from 'From Quaterninon to Matrix and Back', J.M.P. van Waveren, February 27th 2005
		static float Sqrt(float x) {
			long i;
			float y, r;
			y = x * 0.5f;
			i = *(long *)(&x);
			i = 0x5f3759df - (i >> 1);
			r = *(float *)(&i);
			r = r * (1.5f - r * r * y);
			return r;
		}

		static double Sqrt(double x) {
			long i;
			double y, r;
			y = x * 0.5f;
			i = *(long *)(&x);
			i = 0x5f3759df - (i >> 1);
			r = *(double *)(&i);
			r = r * (1.5f - r * r * y);
			return r;
		}
	}
}