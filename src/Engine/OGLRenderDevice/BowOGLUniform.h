#pragma once
#include "BowPrerequisites.h"

#include "IBowCleanableObserver.h"

namespace Bow {
	namespace Renderer {

		using namespace Core;

		enum class UniformType : char
		{
			INVALID,
			Int,
			UnsignedInt,
			Float,
			FloatVector2,
			FloatVector3,
			FloatVector4,
			IntVector2,
			IntVector3,
			IntVector4,
			UnsignedIntVector2,
			UnsignedIntVector3,
			UnsignedIntVector4,
			Bool,
			BoolVector2,
			BoolVector3,
			BoolVector4,
			FloatMatrix22,
			FloatMatrix33,
			FloatMatrix44,
			FloatMatrix23,
			FloatMatrix24,
			FloatMatrix32,
			FloatMatrix34,
			FloatMatrix42,
			FloatMatrix43,
			Sampler1D,
			Sampler2D,
			Sampler2DRectangle,
			Sampler2DRectangleShadow,
			Sampler3D,
			SamplerCube,
			Sampler1DShadow,
			Sampler2DShadow,
			Sampler1DArray,
			Sampler2DArray,
			Sampler1DArrayShadow,
			Sampler2DArrayShadow,
			SamplerCubeShadow,
			IntSampler1D,
			IntSampler2D,
			IntSampler2DRectangle,
			IntSampler3D,
			IntSamplerCube,
			IntSampler1DArray,
			IntSampler2DArray,
			UnsignedIntSampler1D,
			UnsignedIntSampler2D,
			UnsignedIntSampler2DRectangle,
			UnsignedIntSampler3D,
			UnsignedIntSamplerCube,
			UnsignedIntSampler1DArray,
			UnsignedIntSampler2DArray
		};


		class IOGLUniform : public ICleanable
		{
		public:
			// Update method
			IOGLUniform(int location, UniformType datatype, int count, ICleanableObserver* observer) :
				m_dirty(false),
				Location(location),
				Datatype(datatype),
				m_count(count),
				m_observer(observer)
			{
			}

			// implemented by derivative
			virtual void		Clean() = 0;

			const int			Location;
			const UniformType	Datatype;

		protected:
			const int m_count;

			bool m_dirty;
			ICleanableObserver*	const m_observer;
		};

		typedef std::shared_ptr<IOGLUniform> OGLUniformPtr;
		typedef std::hash_map<std::string, OGLUniformPtr> UniformMap;

		template<class T>
		class OGLUniform : public IOGLUniform
		{
		public:
			OGLUniform(int location, UniformType datatype, int count, ICleanableObserver* observer) :
				IOGLUniform(location, datatype, count, observer)
			{
			}

			~OGLUniform()
			{
				delete[] m_val;
			}

			// implemented by derivative
			virtual void Clean() = 0;

			template <class A>
			void SetValue(A* val, int arrayLength)
			{
				if (m_arrayLength < arrayLength)
				{
					arrayLength = m_arrayLength;
					LOG_ERROR("Uniformsize überschritten!");
				}
				else if (m_arrayLength > arrayLength)
				{
					LOG_ERROR("Übergebene Uniformsize ist als der reservierte Speicher. Der Rest wird mit Nullen aufgefüllt!");
				}

				if (!m_dirty)
				{
					m_dirty = true;
					m_observer->NotifyDirty(this);
				}

				memset(m_val, 0, arrayLength*sizeof(T));
				for (int i = 0; i < arrayLength; i++)
				{
					m_val[i] = static_cast<T>(val[i]);
				}
			}

		protected:
			int m_arrayLength;
			T*	m_val;
		};

		class OGLUniformI : public OGLUniform<int>
		{
		public:
			OGLUniformI(int location, UniformType datatype, int count, ICleanableObserver* observer);

			void Clean();
		};

		class OGLUniformF : public OGLUniform<float>
		{
		public:
			OGLUniformF(int location, UniformType datatype, int count, ICleanableObserver* observer);

			void Clean();
		};

		class OGLUniformUI : public OGLUniform<unsigned int>
		{
		public:
			OGLUniformUI(int location, UniformType datatype, int count, ICleanableObserver* observer);

			void Clean();
		};

	}
}