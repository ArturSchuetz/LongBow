#pragma once
#include "BowMath.h"

namespace bow { namespace math { static float Sqrt(float); static double Sqrt(double); } }

#include <algorithm>

namespace bow {

	template <typename T>
	class Quaternion
	{
	public:

		//! Default constructor of a Quaternion.
		//! Creates a simple unit Quaternion.
		Quaternion() : _q(bow::Vector4<T>((T)0, (T)0, (T)0, (T)1)) { }
		//
		Quaternion(T x, T y, T z, T w) : _q(bow::Vector4<T>(x, y, z, w)) { }

		//! Constructor for a Quaternion from a 3D (rotation) matrix.
		//! Matrices here are [row][column], row major.
		//! Taken from 'From Quaterninon to Matrix and Back', J.M.P. van Waveren, February 27th 2005
		/*!
			\param m Rotation matrix, of which an corresponding quaternion is constructed.
			\warning The representation of a rotation as a quaternion is not unique! 
		*/
		Quaternion(const bow::Matrix3D<T> &m)
		{
			if (m._11 + m._22 + m._33 > 0.0)
			{
				T t = +m._11 + m._22 + m._33 + (T)1.0;
				T s = bow::math::Sqrt(t) * (T)0.5;

				_q.w = s * t;
				_q.z = (m._12 - m._21) * s;
				_q.y = (m._31 - m._13) * s;
				_q.x = (m._23 - m._32) * s;
			}
			else if (m._11 > m._22 && m._11 > m._33)
			{
				T t = +m._11 - m._22 - m._33 + (T)1.0;
				T s = bow::math::Sqrt(t) * (T)0.5;
				_q.x = s * t;
				_q.y = (m._12 + m._21) * s;
				_q.z = (m._31 + m._13) * s;
				_q.w = (m._23 - m._32) * s;
			}
			else if (m._22 > m._33)
			{
				T t = -m._11 + m._22 - m._33 + (T)1.0;
				T s = bow::math::Sqrt(t) * (T)0.5;
				_q.y = s * t;
				_q.x = (m._12 + m._21) * s;
				_q.w = (m._31 - m._13) * s;
				_q.z = (m._23 + m._32) * s;
			}
			else
			{
				T t = -m._11 - m._22 + m._33 + (T)1.0;
				T s = bow::math::Sqrt(t) * (T)0.5;
				_q.z = s * t;
				_q.w = (m._12 - m._21) * s;
				_q.x = (m._31 + m._13) * s;
				_q.y = (m._23 + m._32) * s;
			}
		}

		//! Elementwise (as it is defined) addition of two quaternions.
		/*!
			\param rhs The other quaternion to be added to this quaternion.
			\return The plus equaled Quaternion.
		*/
		Quaternion &operator+=(const Quaternion& rhs)
		{
			_q.x += rhs._q.x;
			_q.y += rhs._q.y;
			_q.z += rhs._q.z;
			_q.w += rhs._q.w;
			return *this;
		}
		//! Elementwise (as it is defined) addition of two quaternions.
		/*!
			\param lhs The first quaternion for elementwise quaternion addition.
			\param rhs The other quaternion to be added elementwise with this quaternion.
			\return The new quaternion, as a result of the addition.
		*/
		friend Quaternion operator+(Quaternion& lhs, const Quaternion& rhs)
		{
			Quaternion q;
			q._q.x = lhs._q.x + rhs._q.x;
			q._q.y = lhs._q.y + rhs._q.y;
			q._q.z = lhs._q.z + rhs._q.z;
			q._q.w = lhs._q.w + rhs._q.w;
			return q;
		}

		//! Elementwise (as it is defined) subtraction of two quaternions.
		/*!
			\param rhs The other quaternion to be subtracted to this quaternion.
			\return The subtraction equaled Quaternion.
		*/
		Quaternion &operator-=(const Quaternion& rhs)
		{
			_q.x -= rhs._q.x;
			_q.y -= rhs._q.y;
			_q.z -= rhs._q.z;
			_q.w -= rhs._q.w;
			return *this;
		}
		//! Elementwise (as it is defined) subtraction of two quaternions.
		/*!
			\param lhs The first quaternion for elementwise quaternion subtraction.
			\param rhs The other quaternion to be subtracted with this quaternion.
			\return The new quaternion, as a result of the subtraction.
		*/
		friend Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs)
		{
			Quaternion q;
			q._q.x = lhs._q.x - rhs._q.x;
			q._q.y = lhs._q.y - rhs._q.y;
			q._q.z = lhs._q.z - rhs._q.z;
			q._q.w = lhs._q.w - rhs._q.w;
			return q;
		}

		//!  Elementwise multiplication of the quaternion with a scalar from the right hand side and replacing it 
		/*!
			\param rhs Righthand side scalar for the multiplication.
			\return A new multiplication equaled (*=) quatenion as a result of the scalr multiplication.
		*/
		Quaternion &operator*=(const T& rhs)
		{
			_q.x *= rhs;
			_q.y *= rhs;
			_q.z *= rhs;
			_q.w *= rhs;
			return *this;
		}
		//! Elementwise multiplication of the quaternion with a scalar from the left hand side and returning a new resulting quaternion.
		/*!
			\param lhs Left hand side scalr for elementwise multiplication.
			\param rhs Right hand side quaternion for elementwise multiplication
			\return Resulting quaternion of scalar with quaternion multiplication.
		*/
		friend Quaternion operator*(const T& lhs, const Quaternion& rhs)
		{
			Quaternion q;
			q._q.x = lhs * rhs._q.x;
			q._q.y = lhs * rhs._q.y;
			q._q.z = lhs * rhs._q.z;
			q._q.w = lhs * rhs._q.w;
			return q;
		}

		//! Quaternion multiplication operator, with another quaternion and replacing it.
		/*!
			\param q2 Right hand side quaternion for quaternion multiplication.
			\return A new multiplication equaled (*=) quatenion as a result of the quaternion multiplication.
			
		* The way quaternion multiplication works:
		*
			\f$
			(qq') = (q_w + q_xi + q_yj + q_zk) + (q'_w + q'_xi + q'_yj + q'_zk)
			\f$
		*
		* Collecting components and using identities of components listed above results in:
		*
			\f$
			(qq')_{xyz} = {\times{q_{xyz}}{q'_{xyz}}} + q_w q'_{xyz} + q'_w q_{xyz}
			\f$
		*
			\f$
			(qq')_{w} = {\times{q_{w}}{q'_{w}}} + q_w q'_{xyz} \cdot q'_w q_{xyz}
			\f$
		*
		* Taken from 'https://www.gamasutra.com/view/feature/131686/rotating_objects_using_quaternions.php?page=2'.
		* Smart rearrangement of quaternion element equations, so parts of an element of the equation are reused as much as possible, 
		* whilst keeping arithmetic operations at a minimum.
		*/
		Quaternion& operator *= (const Quaternion& q2)
		{
			Quaternion result;

			T A, B, C, D, E, F, G, H;
			A = (this->_q.w + this->_q.x)*(q2._q.w + q2._q.x);
			B = (this->_q.z - this->_q.y)*(q2._q.y - q2._q.z);
			C = (this->_q.w - this->_q.x)*(q2._q.y + q2._q.z);
			D = (this->_q.y + this->_q.z)*(q2._q.w - q2._q.x);
			E = (this->_q.x + this->_q.z)*(q2._q.x + q2._q.y);
			F = (this->_q.x - this->_q.z)*(q2._q.x - q2._q.y);
			G = (this->_q.w + this->_q.y)*(q2._q.w - q2._q.z);
			H = (this->_q.w - this->_q.y)*(q2._q.w + q2._q.z);
			this->_q.w = B + (-E - F + G + H) / (T)2;
			this->_q.x = A - (E + F + G + H) / (T)2;
			this->_q.y = C + (E - F + G - H) / (T)2;
			this->_q.z = D + (E - F - G + H) / (T)2;

			return *this;
		}

		//! Quaternion multiplication operator, with another quaternion resulting in a new quaternion.
		/*!
			\param q1 Left hand side quaternion for elementwise multiplication.
			\param q2 Right hand side quaternion for elementwise multiplication.
			\return A new quatenion as a result of the quaternion multiplication.

		* The way quaternion multiplication works:
		*
			\f$
			(qq') = (q_w + q_xi + q_yj + q_zk) + (q'_w + q'_xi + q'_yj + q'_zk)
			\f$
		*
		* Collecting components and using identities of components listed above results in:
		*
			\f$
			(qq')_{xyz} = {\times{q_{xyz}}{q'_{xyz}}} + q_w q'_{xyz} + q'_w q_{xyz}
			\f$
		*
			\f$
			(qq')_{w} = {\times{q_{w}}{q'_{w}}} + q_w q'_{xyz} \cdot q'_w q_{xyz}
			\f$
		*
		* Taken from 'https://www.gamasutra.com/view/feature/131686/rotating_objects_using_quaternions.php?page=2'.
		* Smart rearrangement of quaternion element equations, so parts of an element of the equation are reused as much as possible,
		* whilst keeping arithmetic operations at a minimum.
		*/
		friend Quaternion operator * (const Quaternion& q1, const Quaternion& q2)
		{
			Quaternion result;

			float A, B, C, D, E, F, G, H;
			A = (q1._q.w + q1._q.x)*(q2._q.w + q2._q.x);
			B = (q1._q.z - q1._q.y)*(q2._q.y - q2._q.z);
			C = (q1._q.w - q1._q.x)*(q2._q.y + q2._q.z);
			D = (q1._q.y + q1._q.z)*(q2._q.w - q2._q.x);
			E = (q1._q.x + q1._q.z)*(q2._q.x + q2._q.y);
			F = (q1._q.x - q1._q.z)*(q2._q.x - q2._q.y);
			G = (q1._q.w + q1._q.y)*(q2._q.w - q2._q.z);
			H = (q1._q.w - q1._q.y)*(q2._q.w + q2._q.z);
			result._q.w = B + (-E - F + G + H) / (T)2;
			result._q.x = A - (E + F + G + H) / (T)2;
			result._q.y = C + (E - F + G - H) / (T)2;
			result._q.z = D + (E - F - G + H) / (T)2;

			return result;
		}

		//! Quaternion dot product, same as eucledean vector dot product.
		/*!
			\param lhs Left hand side quaternion for dot product computation.
			\param rhs Right hand side quaternion for dot product computation.
			\return Dot product of two quaternions.
		*/
		inline T Dot(const Quaternion &lhs, const Quaternion &rhs)
		{
			//float dotPResult = lhs._imaginary.DotP(rhs._imaginary);
			//float dotPRealResult = lhs._real * rhs._real;
			//return lhs._imaginary.DotP(rhs._imaginary) + lhs._real * rhs._real;
			return lhs._q.x*rhs._q.x + lhs._q.y*rhs._q.y + lhs._q.z*rhs._q.z + lhs._q.w * rhs._q.w;
		}

		// Normalize a quaternion, same as with vectors, to the length of one.
		/*!
			\param q Quaternion to normalize.
			\return Normalized quaternion to the length of one. 
		*/
		inline Quaternion Normalize(const Quaternion& q)
		{
			double length = std::sqrt(Dot(q, q));
			return (1.0 / std::sqrt(Dot(q, q))) * q;
		}

		//! Matrices here are [row][column], row major.
		/*!
			\return Corresponding 3D rotation matrix in xyz space.
			\warning The representation of a rotation as a quaternion is not unique! 
		* Taken from 'From Quaterninon to Matrix and Back', J.M.P. van Waveren, February 27th 2005
		*/
		bow::Matrix3D<T> To3DMatrix() const
		{
			bow::Matrix3D<T> m;

			T x2 = _q.x + _q.x;
			T y2 = _q.y + _q.y;
			T z2 = _q.z + _q.z;
			{
				// diagonal values
				T xx2 = _q.x * x2;
				T yy2 = _q.y * y2;
				T zz2 = _q.z * z2;
				m._11 = 1.0f - yy2 - zz2;
				m._22 = 1.0f - xx2 - zz2;
				m._33 = 1.0f - xx2 - yy2;
			}
			{
				T yz2 = _q.y * z2;
				T wx2 = _q.w * x2;
				m._32 = yz2 - wx2;
				m._23 = yz2 + wx2;
			}
			{
				//
				T xy2 = _q.x * y2;
				T wz2 = _q.w * z2;
				m._21 = xy2 - wz2;
				m._12 = xy2 + wz2;
			}
			{
				//
				T xz2 = _q.x * z2;
				T wy2 = _q.w * y2;
				m._13 = xz2 - wy2;
				m._31 = xz2 + wy2;
			}
			return m;
		}

		//! Spherical interpolation between two quaternions, with parameter t.
		/*!
			\param t Interpolation paramter. Should range from zero to one. 
			\param q1 Start quaternion of the spherical interpolation.
			\param q2 Target quaternion of the spherical interpolation.
			\return Spherical interpolated quaternion.
			\warning The representation of a rotation as a quaternion is not unique! 
		*/
		Quaternion Slerp(float t, const Quaternion &q1, const Quaternion &q2)
		{
			float cosTheta = Dot(q1, q2);
			// If target and destination quaternion distance is too small use direct interpolation.
			if (cosTheta > (T)0.9995)
			{
				return (1 - t) * q1 + t * q2;
			}
			// Else use spherical interpolation.
			else
			{
				float theta = std::acos(std::max<float>(std::min<float>(cosTheta, (T)1.0), (T)-1.0));
				float thetap = t * theta;
				Quaternion qperp = Normalize(q2 - cosTheta * q1);
				return std::cos(thetap) * q1 + std::sin(thetap) * qperp;
			}
		}

		//bow::Vector4<float> _q;

		T X() { return _q.x; }
		T Y() { return _q.y; }
		T Z() { return _q.z; }
		T W() { return _q.w; }

	private:
		bow::Vector4<T> _q;	/// Four element vector holding x,y,z and w of the quaternion.
		//bow::Vector3<float> _imaginary;
		//float _real;
	};
}