#pragma once
#pragma once
#include "BowMath.h"
#include <algorithm>


namespace Bow {
	namespace Core {
		template <typename T>
		class Quaternion
		{
		public:

			Quaternion() : _q(Bow::Core::Vector4<T>(0, 0, 0, 1)) { }
			Quaternion(T x, T y, T z, T w) : _q(Bow::Core::Vector4<T>(x, y, z, w)) { }

			// Construct Quaternion from 3D (rotation) matrix.
			// Matrices here are [row][column].
			// Taken from 'From Quaterninon to Matrix and Back', J.M.P. van Waveren, February 27th 2005
			Quaternion(const Bow::Core::Matrix3D<T> &m)
			{
				if (m._11 + m._22 + m._33 > 0.0)
				{
					T t = +m._11 + m._22 + m._33 + 1.0;
					T s = Bow::Math::Sqrt(t) * 0.5;

					_q.w = s * t;
					_q.z = (m._12 - m._21) * s;
					_q.y = (m._31 - m._13) * s;
					_q.x = (m._23 - m._32) * s;
				}
				else if (m._11 > m._22 && m._11 > m._33)
				{
					T t = +m._11 - m._22 - m._33 + 1.0;
					T s = Bow::Math::Sqrt(t) * 0.5;
					_q.x = s * t;
					_q.y = (m._12 + m._21) * s;
					_q.z = (m._31 + m._13) * s;
					_q.w = (m._23 - m._32) * s;
				}
				else if (m._22 > m._33)
				{
					T t = -m._11 + m._22 - m._33 + 1.0;
					T s = Bow::Math::Sqrt(t) * 0.5;
					_q.y = s * t;
					_q.x = (m._12 + m._21) * s;
					_q.w = (m._31 - m._13) * s;
					_q.z = (m._23 + m._32) * s;
				}
				else
				{
					T t = -m._11 - m._22 + m._33 + 1.0;
					T s = Bow::Math::Sqrt(t) * 0.5;
					_q.z = s * t;
					_q.w = (m._12 - m._21) * s;
					_q.x = (m._31 + m._13) * s;
					_q.y = (m._23 + m._32) * s;
				}
			}

			Quaternion &operator+=(const Quaternion& rhs)
			{
				_q.x += rhs._q.x;
				_q.y += rhs._q.y;
				_q.z += rhs._q.z;
				_q.w += rhs._q.w;
				return *this;
			}
			friend Quaternion operator+(Quaternion& lhs, const Quaternion& rhs)
			{
				Quaternion q;
				q._q.x = lhs._q.x + rhs._q.x;
				q._q.y = lhs._q.y + rhs._q.y;
				q._q.z = lhs._q.z + rhs._q.z;
				q._q.w = lhs._q.w + rhs._q.w;
				return q;
			}

			Quaternion &operator-=(const Quaternion& rhs)
			{
				_q.x -= rhs._q.x;
				_q.y -= rhs._q.y;
				_q.z -= rhs._q.z;
				_q.w -= rhs._q.w;
				return *this;
			}
			friend Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs)
			{
				Quaternion q;
				q._q.x = lhs._q.x - rhs._q.x;
				q._q.y = lhs._q.y - rhs._q.y;
				q._q.z = lhs._q.z - rhs._q.z;
				q._q.w = lhs._q.w - rhs._q.w;
				return q;
			}

			Quaternion &operator*=(const T& rhs)
			{
				_q.x *= rhs;
				_q.y *= rhs;
				_q.z *= rhs;
				_q.w *= rhs;
				return *this;
			}
			friend Quaternion operator*(const T& lhs, const Quaternion& rhs)
			{
				Quaternion q;
				q._q.x = lhs * rhs._q.x;
				q._q.y = lhs * rhs._q.y;
				q._q.z = lhs * rhs._q.z;
				q._q.w = lhs * rhs._q.w;
				return q;
			}

			// Quaternion multiplication operator
			// Taken from 'https://www.gamasutra.com/view/feature/131686/rotating_objects_using_quaternions.php?page=2'
			// Smart rearrangement of quaternion element equations, so parts of an element equation are reused as much as possible, 
			// whilst keeping arithmetic operations at a minimum.
			Quaternion &operator*=(const Quaternion& q2)
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
				this->_q.w = B + (-E - F + G + H) / 2;
				this->_q.x = A - (E + F + G + H) / 2;
				this->_q.y = C + (E - F + G - H) / 2;
				this->_q.z = D + (E - F - G + H) / 2;

				return *this;
			}

			// Quaternion multiplication operator
			// Taken from 'https://www.gamasutra.com/view/feature/131686/rotating_objects_using_quaternions.php?page=2'
			// Smart rearrangement of quaternion element equations, so parts of an element equation are reused as much as possible.
			friend Quaternion &operator*(const Quaternion& q1, const Quaternion& q2)
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
				result._q.w = B + (-E - F + G + H) / 2;
				result._q.x = A - (E + F + G + H) / 2;
				result._q.y = C + (E - F + G - H) / 2;
				result._q.z = D + (E - F - G + H) / 2;

				return result;
			}

			inline T Dot(const Quaternion &lhs, const Quaternion &rhs)
			{
				//float dotPResult = lhs._imaginary.DotP(rhs._imaginary);
				//float dotPRealResult = lhs._real * rhs._real;
				//return lhs._imaginary.DotP(rhs._imaginary) + lhs._real * rhs._real;
				return lhs._q.x*rhs._q.x + lhs._q.y*rhs._q.y + lhs._q.z*rhs._q.z + lhs._q.w * rhs._q.w;
			}

			inline Quaternion Normalize(const Quaternion& q)
			{
				double length = std::sqrt(Dot(q, q));
				return (1.0 / std::sqrt(Dot(q, q))) * q;
			}

			// Matrices here are [row][column].
			//Taken from 'From Quaterninon to Matrix and Back', J.M.P. van Waveren, February 27th 2005
			Bow::Core::Matrix3D<T> To3DMatrix() const
			{
				Bow::Core::Matrix3D<T> m;

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

			Quaternion Slerp(float t, const Quaternion &q1, const Quaternion &q2)
			{
				float cosTheta = Dot(q1, q2);
				if (cosTheta > 0.9995)
				{
					return (1 - t) * q1 + t * q2;
				}
				else
				{
					float theta = std::acos(std::max<float>(std::min<float>(cosTheta, 1.0), -1.0));
					float thetap = t * theta;
					Quaternion qperp = Normalize(q2 - cosTheta * q1);
					return std::cos(thetap) * q1 + std::sin(thetap) * qperp;
				}
			}

			//Bow::Core::Vector4<float> _q;

			T X() { return _q.x; }
			T Y() { return _q.y; }
			T Z() { return _q.z; }
			T W() { return _q.w; }

		private:
			Bow::Core::Vector4<T> _q;
			//Bow::Core::Vector3<float> _imaginary;
			//float _real;
		};
	}
}