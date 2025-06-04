/**
 * @file BowClipmap.h
 * @brief Declarations for BowClipmap.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"
#include "BowBitmap.h"

namespace bow {
	namespace Core {

		class Clipmap
		{
		public:
			Clipmap(unsigned int columns, unsigned int rows);
			~Clipmap();

			bool LoadFile(unsigned int column, unsigned int row, const char* path);
			float GetPixel(float x, float y) const;

		private:
			Clipmap(const Clipmap& other){} // do not copy

			unsigned int m_columns;
			unsigned int m_rows;

			Bitmap *m_ImageArray;
		};

	}
}