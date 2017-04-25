#pragma once
#include "BowClipmap.h"

namespace Bow {
	namespace Core {

		Clipmap::Clipmap(unsigned int columns, unsigned int rows)
		{
			m_ImageArray = new Bitmap[columns * rows];
			m_columns = columns;
			m_rows = rows;
		}

		Clipmap::~Clipmap()
		{
			delete[] m_ImageArray;
		}

		bool Clipmap::LoadFile(unsigned int column, unsigned int row, const char* path)
		{
			unsigned int index = column + m_columns * row;
			m_ImageArray[index].LoadFile(path);
			return true;
		}

		float Clipmap::GetPixel(float _x, float _y) const
		{
			float absolute_x = _x * (float)m_columns;
			float absolute_y = _y * (float)m_rows;

			unsigned int column = ((unsigned int)absolute_x) % m_columns;
			unsigned int row	= ((unsigned int)absolute_y) % m_rows;

			unsigned int index = column + m_columns * row;

			float relative_x = absolute_x - (float)((int)absolute_x);
			float relative_y = absolute_y - (float)((int)absolute_y);

			unsigned char* heightMap = m_ImageArray[index].GetData();
			unsigned int mapHeight = m_ImageArray[index].GetHeight();
			unsigned int mapWidth = m_ImageArray[index].GetWidth();
			unsigned int mapSize = m_ImageArray[index].GetSizeInBytes();

			unsigned int x = (int)(relative_x * (double)mapWidth);
			unsigned int y = (int)(relative_y * (double)mapHeight);

			return (float)heightMap[(x + y * mapWidth) % mapSize] / (float)255;
		}
	}
}