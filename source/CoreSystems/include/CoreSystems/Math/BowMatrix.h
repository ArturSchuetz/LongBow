#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include "CoreSystems/Math/BowVector2.h"
#include "CoreSystems/Math/BowVector3.h"

#include <assert.h>

namespace bow
{

template <typename T> class Matrix
{
    friend class Matrix_trans<T>;

  public:
    Matrix() : m_data(nullptr), m_NumRows(0), m_NumColumns(0) { FN("Matrix::Matrix"); }

    Matrix(uint32_t rows, unsigned columns) : m_data(nullptr), m_NumRows(0), m_NumColumns(0)
    {
        FN("Matrix::Matrix");

        Resize(rows, columns);

        memset(m_data, 0, sizeof(T) * NumElements());
    }

    Matrix(const Matrix &ref) : m_data(nullptr), m_NumRows(0), m_NumColumns(0)
    {
        FN("Matrix::Matrix");

        Resize(ref.NumRows(), ref.NumColumns());

        for (size_t i = 0; i < NumElements(); i++)
        {
            this->m_data[i] = ref.m_data[i];
        }
    }

    Matrix(const Matrix_trans<T> &ref) : m_data(nullptr), m_NumRows(0), m_NumColumns(0)
    {
        FN("Matrix::Matrix");

        Resize(ref.NumRows(), ref.NumColumns());

        m_NumRows = ref.NumRows();
        m_NumColumns = ref.NumColumns();

        for (size_t row = 0; row < m_NumRows; row++)
        {
            for (size_t col = 0; col < m_NumColumns; col++)
            {
                (*this)(row, col) = ref(row, col);
            }
        }
    }

    ~Matrix()
    {
        FN("Matrix::~Matrix");

        Release();
    }

    void Resize(uint32_t rows, unsigned columns)
    {
        FN("Matrix::Resize");

        // Check for same size. Includes transposed tag check.
        assert(rows > 0 && columns > 0);

        Release();

        m_data = new T[rows * columns];
        m_NumRows = rows;
        m_NumColumns = columns;
    }

    void Release()
    {
        FN("Matrix::Release");

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
        FN("Matrix::Transposed");

        return Matrix_trans<T>(this);
    }

    inline uint32_t NumElements() const
    {
        FN("Matrix::NumElements");

        return m_NumRows * m_NumColumns;
    }

    inline uint32_t NumRows() const
    {
        FN("Matrix::NumRows");

        return m_NumRows;
    }

    inline uint32_t NumColumns() const
    {
        FN("Matrix::NumColumns");

        return m_NumColumns;
    }

    inline T operator()(uint32_t row, uint32_t column) const
    {
        FN("Matrix::operator()");

        assert(row < NumRows() && column < NumColumns());
        assert(m_data != nullptr);

        return m_data[column + row * m_NumColumns];
    }

    inline T &operator()(uint32_t row, uint32_t column)
    {
        FN("Matrix::operator()");

        assert(row < NumRows() && column < NumColumns());
        assert(m_data != nullptr);

        return m_data[column + row * m_NumColumns];
    }

    inline Matrix operator+(const Matrix &rhs) const
    {
        FN("Matrix::operator+");

        // Check for same size. Includes transposed tag check.
        assert(NumColumns() == rhs.NumColumns() && NumRows() == rhs.NumRows());
        assert(m_data != nullptr);

        Matrix result(m_NumRows, m_NumColumns);

        uint32_t numElements = NumElements();
        for (size_t i = 0; i < numElements; ++i)
        {
            result.m_data[i] = m_data[i] + (T)rhs.m_data[i];
        }

        return result;
    }

    inline Matrix operator-(const Matrix &rhs) const
    {
        FN("Matrix::operator-");

        // Check for same size. Includes transposed tag check.
        assert(NumColumns() == rhs.NumColumns() && NumRows() == rhs.NumRows());
        assert(m_data != nullptr);

        Matrix result(m_NumRows, m_NumColumns);

        uint32_t numElements = NumElements();
        for (size_t i = 0; i < numElements; ++i)
        {
            result.m_data[i] = m_data[i] - (T)rhs.m_data[i];
        }

        return result;
    }

    inline Matrix operator*(const Matrix &rhs) const
    {
        FN("Matrix::operator*");

        // Check whether operation is allowed. Includes transposition.
        assert((*this).NumColumns() == rhs.NumRows());
        assert(m_data != nullptr);

        Matrix result = Matrix((*this).NumRows(), rhs.NumColumns());
        T *columnB = new T[rhs.NumRows()];

        // For each row of the resulting matrix
        for (size_t col = 0; col < result.NumColumns(); col++)
        {
            for (size_t i = 0; i < rhs.NumRows(); i++)
            {
                columnB[i] = (T)rhs(i, col);
            }

            // For each col of the resulting matrix
            for (size_t row = 0; row < result.NumRows(); row++)
            {
                // Calculate the dot product of A's row and B's col to get the
                // result for the current element
                T dotProduct = 0;
                for (size_t innerRow = 0; innerRow < rhs.NumRows(); ++innerRow) // B Transposed!
                {
                    dotProduct += (*this)(row, innerRow) * columnB[innerRow];
                }
                result(row, col) = dotProduct;
            }
        }

        delete[] columnB;
        return result;
    }

    inline Matrix operator*(T rhs) const
    {
        FN("Matrix::operator*");

        assert(m_data != nullptr);

        Matrix result(m_NumRows, m_NumColumns);

        uint32_t numElements = NumElements();
        for (size_t i = 0; i < numElements; ++i)
        {
            result.m_data[i] = m_data[i] * rhs;
        }

        return result;
    }

    inline Matrix operator/(T rhs) const
    {
        FN("Matrix::operator/");

        assert(m_data != nullptr);

        Matrix result(m_NumRows, m_NumColumns);

        uint32_t numElements = NumElements();
        for (size_t i = 0; i < numElements; ++i)
        {
            result.m_data[i] = m_data[i] / rhs;
        }

        return result;
    }

    /*-----------------------------------------------------------------------------------------*/

    friend inline Matrix operator*(T x, const Matrix<T> &m)
    {
        FN("Matrix::operator*");

        return m * x;
    }

  private:
    T *m_data;
    uint32_t m_NumRows;
    uint32_t m_NumColumns;
};

template <typename C> inline std::ostream &operator<<(std::ostream &str, const Matrix<C> &m)
{
    FN("Matrix::operator<<");

    uint32_t nRow = m.NumRows();
    uint32_t nCol = m.NumColumns();

    str << '[';
    for (size_t row = 0; row < nRow; ++row)
    {
        for (size_t col = 0; col < nCol; ++col)
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

/*----------------------------------------------------------------*/

template <typename T> class Matrix_trans
{
    friend class Matrix<T>;

  public:
    ~Matrix_trans() { FN("Matrix_trans::Matrix_trans<<"); }

    inline uint32_t NumRows() const
    {
        FN("Matrix_trans::NumRows");

        return m_mat.NumColumns();
    }
    inline uint32_t NumColumns() const
    {
        FN("Matrix_trans::NumColumns");

        return m_mat.NumRows();
    }

    inline float operator()(uint32_t row, uint32_t column) const
    {
        FN("Matrix_trans::operator()");

        assert(m_mat.m_data != nullptr);

        return m_mat.m_data[row + column * m_mat.NumColumns()];
    }

    inline Matrix<T> Transposed()
    {
        FN("Matrix_trans::Transposed");

        return m_mat;
    }

    inline Matrix<T> operator+(const Matrix<T> &rhs) const
    {
        FN("Matrix_trans::operator+");

        // Check for same size. Includes transposed tag check.
        assert(NumColumns() == rhs.NumColumns() && NumRows() == rhs.NumRows());

        Matrix<T> result(NumRows(), NumColumns());

        for (size_t row = 0; row < NumRows(); ++row)
        {
            for (size_t col = 0; col < NumColumns(); ++col)
            {
                result(row, col) = (*this)(row, col) + rhs(row, col);
            }
        }

        return result;
    }

    inline Matrix<T> operator+(const Matrix_trans &rhs) const
    {
        FN("Matrix_trans::operator+");

        // Check for same size. Includes transposed tag check.
        assert(NumColumns() == rhs.NumColumns() && NumRows() == rhs.NumRows());

        Matrix<T> result(NumRows(), NumColumns());

        for (size_t row = 0; row < NumRows(); ++row)
        {
            for (size_t col = 0; col < NumColumns(); ++col)
            {
                result(row, col) = (*this)(row, col) + (T)rhs(row, col);
            }
        }
        return result;
    }

    inline Matrix<T> operator-(const Matrix<T> &rhs) const
    {
        FN("Matrix_trans::operator-");

        // Check for same size. Includes transposed tag check.
        assert(NumRows() == rhs.NumRows() && NumColumns() == rhs.NumColumns());

        Matrix<T> result(NumRows(), NumColumns());

        for (size_t row = 0; row < NumRows(); ++row)
        {
            for (size_t col = 0; col < NumColumns(); ++col)
            {
                result(row, col) = (*this)(row, col) - rhs(row, col);
            }
        }

        return result;
    }

    inline Matrix<T> operator-(const Matrix_trans &rhs) const
    {
        FN("Matrix_trans::operator-");

        assert(m_mat.NumColumns() == rhs.m_mat.NumColumns() && m_mat.NumRows() == rhs.m_mat.NumRows());

        Matrix<T> result(NumRows(), NumColumns());

        for (size_t row = 0; row < NumRows(); ++row)
        {
            for (size_t col = 0; col < NumColumns(); ++col)
            {
                result(row, col) = (*this)(row, col) - rhs(row, col);
            }
        }
        return result;
    }

    inline Matrix<T> operator*(const Matrix<T> &rhs) const
    {
        FN("Matrix_trans::operator*");

        // Check whether operation is allowed. Includes transposition.
        assert(NumColumns() == rhs.NumRows());

        Matrix<T> prod = Matrix<T>((*this).NumRows(), rhs.NumColumns());

        // Distinguish 4 cases: AB, A'B, AB' and A'B'.
        // Default to (,) operator for now.

        // For each col of the resulting matrix
        for (size_t row = 0; row < prod.NumRows(); row++)
        {
            for (size_t col = 0; col < prod.NumColumns(); col++)
            {
                // Calculate the dot product of A's row and B's col to get the
                // result for the current element
                for (size_t innerRow = 0; innerRow < rhs.NumRows(); ++innerRow) // B Transposed!
                {
                    prod(row, col) += (*this)(row, innerRow) * rhs(innerRow, col);
                }
            }
        }

        return prod;
    }

    inline Matrix<T> operator*(const Matrix_trans &rhs) const
    {
        FN("Matrix_trans::operator*");

        // Check whether operation is allowed. Includes transposition.
        assert(NumColumns() == rhs.NumRows());

        Matrix<T> prod = Matrix<T>((*this).NumRows(), rhs.NumColumns());

        // Distinguish 4 cases: AB, A'B, AB' and A'B'.
        // Default to (,) operator for now.
        for (size_t row = 0; row < prod.NumRows(); row++)
        {
            // For each col of the resulting matrix
            for (size_t col = 0; col < prod.NumColumns(); col++)
            {
                // Calculate the dot product of A's row and B's col to get the
                // result for the current element
                for (size_t innerRow = 0; innerRow < rhs.NumRows(); ++innerRow) // B Transposed!
                {
                    prod(row, col) += (*this)(row, innerRow) * rhs(innerRow, col);
                }
            }
        }

        return prod;
    }

    inline Matrix<T> operator*(T rhs) const
    {
        FN("Matrix_trans::operator*");

        Matrix<T> result(NumRows(), NumColumns());

        for (size_t row = 0; row < NumRows(); ++row)
        {
            for (size_t col = 0; col < NumColumns(); ++col)
            {
                result(row, col) = (*this)(row, col) * rhs;
            }
        }

        return result;
    }

    inline Matrix<T> operator/(T rhs) const
    {
        FN("Matrix_trans::operator/");

        Matrix<T> result(NumRows(), NumColumns());

        for (size_t row = 0; row < NumRows(); ++row)
        {
            for (size_t col = 0; col < NumColumns(); ++col)
            {
                result(row, col) = (*this)(row, col) / rhs;
            }
        }

        return result;
    }

    /*-----------------------------------------------------------------------------------------*/

    friend inline Matrix_trans operator*(T x, const Matrix_trans &m)
    {
        FN("Matrix_trans::operator*");

        return m * x;
    }

  protected:
    Matrix_trans() : m_mat() { FN("Matrix_trans::Matrix_trans"); }
    Matrix_trans(const Matrix<T> &m) : m_mat(m) { FN("Matrix_trans::Matrix_trans"); }
    const Matrix<T> &m_mat;
};

template <typename C> inline std::ostream &operator<<(std::ostream &str, const Matrix_trans<C> &m)
{
    FN("Matrix_trans::operator<<");

    uint32_t nRow = m.NumRows();
    uint32_t nCol = m.NumColumns();

    str << '[';
    for (size_t row = 0; row < nRow; ++row)
    {
        for (size_t col = 0; col < nCol; ++col)
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
} // namespace bow
