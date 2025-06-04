/**
 * @file BowMatrix.h
 * @brief Declarations for BowMatrix.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowVector2.h"
#include "BowVector3.h"

#include <assert.h>

namespace bow {

	template<typename T>
	class Matrix
	{
		friend class Matrix_trans<T>;

	public:
		Matrix() : m_data(nullptr), m_NumRows(0), m_NumColumns(0)
		{
		}

		Matrix(unsigned int rows, unsigned columns) : m_data(nullptr), m_NumRows(0), m_NumColumns(0)
		{
			Resize(rows, columns);

			memset(m_data, 0, sizeof(T) * NumElements());
		}

		Matrix(const Matrix& ref) : m_data(nullptr), m_NumRows(0), m_NumColumns(0)
		{
			Resize(ref.NumRows(), ref.NumColumns());

			for (unsigned int i = 0; i < NumElements(); i++)
			{
				this->m_data[i] = ref.m_data[i];
			}
		}

		Matrix(const Matrix_trans<T>& ref) : m_data(nullptr), m_NumRows(0), m_NumColumns(0)
		{
			Resize(ref.NumRows(), ref.NumColumns());

			m_NumRows = ref.NumRows();
			m_NumColumns = ref.NumColumns();

			for (unsigned int row = 0; row < m_NumRows; row++)
			{
				for (unsigned int col = 0; col < m_NumColumns; col++)
				{
					(*this)(row, col) = ref(row, col);
				}
			}
		}

		~Matrix()
		{
			Release();
		}

		void Resize(unsigned int rows, unsigned columns)
		{
			// Check for same size. Includes transposed tag check.
			assert(rows > 0 && columns > 0);

			Release();

			m_data = new T[rows * columns];
			m_NumRows = rows;
			m_NumColumns = columns;
		}

		void Release()
		{
			if (m_data != nullptr)
			{
				delete[] m_data;
				m_data = nullptr;
			}

			m_NumRows = 0;
			m_NumColumns = 0;
		}

		inline Matrix_trans<T> Transposed()
		{
			return Matrix_trans<T>(this);
		}

		inline unsigned int NumElements() const 
		{
			return m_NumRows * m_NumColumns;
		}

		inline unsigned int NumRows() const 
		{
			return m_NumRows;
		}

		inline unsigned int NumColumns() const
		{
			return m_NumColumns;
		}

		inline T operator () (unsigned int row, unsigned int column) const
		{
			assert(row < NumRows() && column < NumColumns());
			assert(m_data != nullptr);

			return m_data[column + row * m_NumColumns];
		}

		inline T& operator () (unsigned int row, unsigned int column)
		{
			assert(row < NumRows() && column < NumColumns());
			assert(m_data != nullptr);

			return m_data[column + row * m_NumColumns];
		}

		inline Matrix operator + (const Matrix& rhs) const
		{
			// Check for same size. Includes transposed tag check.
			assert(NumColumns() == rhs.NumColumns() && NumRows() == rhs.NumRows());
			assert(m_data != nullptr);

			Matrix result(m_NumRows, m_NumColumns);

			unsigned int numElements = NumElements();
			for (unsigned int i = 0; i < numElements; ++i)
			{
				result.m_data[i] = m_data[i] + (T)rhs.m_data[i];
			}

			return result;
		}

		inline Matrix operator - (const Matrix& rhs) const
		{
			// Check for same size. Includes transposed tag check.
			assert(NumColumns() == rhs.NumColumns() && NumRows() == rhs.NumRows());
			assert(m_data != nullptr);

			Matrix result(m_NumRows, m_NumColumns);

			unsigned int numElements = NumElements();
			for (unsigned int i = 0; i < numElements; ++i)
			{
				result.m_data[i] = m_data[i] - (T)rhs.m_data[i];
			}

			return result;
		}

		inline Matrix operator * (const Matrix& rhs) const
		{
			// Check whether operation is allowed. Includes transposition.
			assert((*this).NumColumns() == rhs.NumRows());
			assert(m_data != nullptr);

			Matrix result = Matrix((*this).NumRows(), rhs.NumColumns());
			T* columnB = new T[rhs.NumRows()];

			// For each row of the resulting matrix
			for (unsigned int col = 0; col < result.NumColumns(); col++)
			{
				for (unsigned int i = 0; i < rhs.NumRows(); i++)
				{
					columnB[i] = (T)rhs(i, col);
				}

				// For each col of the resulting matrix
				for (unsigned int row = 0; row < result.NumRows(); row++)
				{
					// Calculate the dot product of A's row and B's col to get the result for the current element
					T dotProduct = 0;
					for (unsigned int innerRow = 0; innerRow < rhs.NumRows(); ++innerRow) // B Transposed!
					{
						dotProduct += (*this)(row, innerRow) * columnB[innerRow];
					}
					result(row, col) = dotProduct;
				}
			}

			delete[] columnB;
			return result;
		}

		inline Matrix operator * (T rhs) const
		{
			assert(m_data != nullptr);

			Matrix result(m_NumRows, m_NumColumns);

			unsigned int numElements = NumElements();
			for (unsigned int i = 0; i < numElements; ++i)
			{
				result.m_data[i] = m_data[i] * rhs;
			}

			return result;
		}

		inline Matrix operator / (T rhs) const
		{
			assert(m_data != nullptr);

			Matrix result(m_NumRows, m_NumColumns);

			unsigned int numElements = NumElements();
			for (unsigned int i = 0; i < numElements; ++i)
			{
				result.m_data[i] = m_data[i] / rhs;
			}

			return result;
		}

		/*-----------------------------------------------------------------------------------------*/

		friend inline Matrix operator * (T x, const Matrix<T>& m) { return m * x; }

	private:
		T* m_data;
		unsigned int m_NumRows;
		unsigned int m_NumColumns;
	};

	template<typename C>
	inline std::ostream& operator << (std::ostream& str, const Matrix<C>& m)
	{
		unsigned int nRow = m.NumRows();
		unsigned int nCol = m.NumColumns();

		str << '[';
		for (unsigned int row = 0; row < nRow; ++row)
		{
			for (unsigned int col = 0; col < nCol; ++col)
			{
				str << m(row, col);

				if (col != nCol - 1)
					str << ", ";
			}

			if(row != nRow - 1)
				str << ";" << std::endl << " ";
		}
		str << "]";

		return str;
	}

	/*----------------------------------------------------------------*/

	template<typename T>
	class Matrix_trans
	{
		friend class Matrix<T>;

	public:
		~Matrix_trans() {}

		inline unsigned int NumRows() const { return m_mat.NumColumns(); }
		inline unsigned int NumColumns() const { return m_mat.NumRows(); }

		inline float operator () (unsigned int row, unsigned int column) const
		{
			assert(m_mat.m_data != nullptr);

			return m_mat.m_data[row + column * m_mat.NumColumns()];
		}

		inline Matrix<T> Transposed()
		{
			return m_mat;
		}

		inline Matrix<T> operator + (const Matrix<T>& rhs) const
		{
			// Check for same size. Includes transposed tag check.
			assert(NumColumns() == rhs.NumColumns() && NumRows() == rhs.NumRows());

			Matrix result(NumRows(), NumColumns());

			for (unsigned int row = 0; row < NumRows(); ++row)
			{
				for (unsigned int col = 0; col < NumColumns(); ++col)
				{
					result(row, col) = (*this)(row, col) + rhs(row, col);
				}
			}

			return result;
		}

		inline Matrix<T> operator + (const Matrix_trans& rhs) const
		{
			// Check for same size. Includes transposed tag check.
			assert(NumColumns() == rhs.NumColumns() && NumRows() == rhs.NumRows());

			Matrix result(NumRows(), NumColumns());

			for (unsigned int row = 0; row < NumRows(); ++row)
			{
				for (unsigned int col = 0; col < NumColumns(); ++col)
				{
					result(row, col) = (*this)(row, col) + (T)rhs(row, col);
				}
			}
			return result;
		}

		inline Matrix<T> operator - (const Matrix<T>& rhs) const
		{
			// Check for same size. Includes transposed tag check.
			assert(NumRows() == rhs.NumRows() && NumColumns() == rhs.NumColumns());

			Matrix result(NumRows(), NumColumns());

			for (unsigned int row = 0; row < NumRows(); ++row)
			{
				for (unsigned int col = 0; col < NumColumns(); ++col)
				{
					result(row, col) = (*this)(row, col) - rhs(row, col);
				}
			}

			return result;
		}

		inline Matrix<T> operator - (const Matrix_trans& rhs) const
		{
			assert(m_mat.NumColumns() == rhs.m_mat.NumColumns() && m_mat.NumRows() == rhs.m_mat.NumRows());

			Matrix result(NumRows(), NumColumns());

			for (unsigned int row = 0; row < NumRows(); ++row)
			{
				for (unsigned int col = 0; col < NumColumns(); ++col)
				{
					result(row, col) = (*this)(row, col) - rhs(row, col);
				}
			}
			return result;
		}

		inline Matrix<T> operator * (const Matrix<T>& rhs) const
		{
			// Check whether operation is allowed. Includes transposition.
			assert(NumColumns() == rhs.NumRows());

			Matrix prod = Matrix((*this).NumRows(), rhs.NumColumns());

			// Distinguish 4 cases: AB, A'B, AB' and A'B'.
			// Default to (,) operator for now.

			// For each col of the resulting matrix
			for (unsigned int row = 0; row < prod.NumRows(); row++)
			{
				for (unsigned int col = 0; col < prod.NumColumns(); col++)
				{
					// Calculate the dot product of A's row and B's col to get the result for the current element
					for (unsigned int innerRow = 0; innerRow < rhs.NumRows(); ++innerRow) // B Transposed!
					{
						prod(row, col) += (*this)(row, innerRow) * rhs(innerRow, col);
					}
				}
			}

			return prod;
		}

		inline Matrix<T> operator * (const Matrix_trans& rhs) const
		{
			// Check whether operation is allowed. Includes transposition.
			assert(NumColumns() == rhs.NumRows());

			Matrix prod = Matrix((*this).NumRows(), rhs.NumColumns());

			// Distinguish 4 cases: AB, A'B, AB' and A'B'.
			// Default to (,) operator for now.
			for (unsigned int row = 0; row < prod.NumRows(); row++)
			{
				// For each col of the resulting matrix
				for (unsigned int col = 0; col < prod.NumColumns(); col++)
				{
					// Calculate the dot product of A's row and B's col to get the result for the current element
					for (unsigned int innerRow = 0; innerRow < rhs.NumRows(); ++innerRow) // B Transposed!
					{
						prod(row, col) += (*this)(row, innerRow) * rhs(innerRow, col);
					}
				}
			}

			return prod;
		}

		inline Matrix<T> operator * (T rhs) const
		{
			Matrix result(NumRows(), NumColumns());

			for (unsigned int row = 0; row < NumRows(); ++row)
			{
				for (unsigned int col = 0; col < NumColumns(); ++col)
				{
					result(row, col) = (*this)(row, col) * rhs;
				}
			}

			return result;
		}

		inline Matrix<T> operator / (T rhs) const
		{
			Matrix result(NumRows(), NumColumns());

			for (unsigned int row = 0; row < NumRows(); ++row)
			{
				for (unsigned int col = 0; col < NumColumns(); ++col)
				{
					result(row, col) = (*this)(row, col) / rhs;
				}
			}

			return result;
		}

		/*-----------------------------------------------------------------------------------------*/

		friend inline Matrix_trans operator * (T x, const Matrix_trans& m) { return m * x; }

	protected:
		Matrix_trans() : m_mat() {}
		Matrix_trans(const Matrix<T>& m) : m_mat(m) {}
		const Matrix<T>& m_mat;
	};

	template<typename C>
	inline std::ostream& operator << (std::ostream& str, const Matrix_trans<C>& m)
	{
		unsigned int nRow = m.NumRows();
		unsigned int nCol = m.NumColumns();

		str << '[';
		for (unsigned int row = 0; row < nRow; ++row)
		{
			for (unsigned int col = 0; col < nCol; ++col)
			{
				str << m(row, col);

				if (col != nCol - 1)
					str << ", ";
			}

			if (row != nRow - 1)
				str << ";" << std::endl << " ";
		}
		str << "]";

		return str;
	}

	/*-----------------------------------------------------------------------------------------*/
}
