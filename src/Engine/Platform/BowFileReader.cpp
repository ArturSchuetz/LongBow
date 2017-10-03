#include "BowFileReader.h"

#include <iostream>
#include <fstream>

namespace Bow
{
	namespace Platform
	{
		FileReader::FileReader() 
			: m_pFile(nullptr)
		{
			m_pFile = (void*)new std::ifstream();
		}

		FileReader::~FileReader()
		{ 
			Close();

			if (m_pFile != nullptr)
			{
				delete m_pFile;
				m_pFile = nullptr;
			}
		}

		bool FileReader::Open(const char* filePath)
		{
			((std::ifstream*)m_pFile)->open(filePath, std::ios::in | std::ios::binary);
			if (!((std::ifstream*)m_pFile)->is_open())
			{
				return false;
			}

			return true;
		}

		void FileReader::Close()
		{
			((std::ifstream*)m_pFile)->close();
		}

		size_t FileReader::GetSizeOfFile()
		{
			// get current position
			std::streampos curr = ((std::ifstream*)m_pFile)->tellg();

			std::streampos begin = ((std::ifstream*)m_pFile)->tellg();
			((std::ifstream*)m_pFile)->seekg(0, std::ios::end);
			std::streampos end = ((std::ifstream*)m_pFile)->tellg();

			// restore position
			((std::ifstream*)m_pFile)->seekg(curr, std::ios::beg);
			return (size_t)(end - begin);
		}

		unsigned long FileReader::Tell()
		{
			return (unsigned long)((std::ifstream*)m_pFile)->tellg();
		}

		void FileReader::Seek(size_t offset)
		{
			((std::ifstream*)m_pFile)->seekg(offset, std::ios::beg);
		}

		void FileReader::Skip(long count)
		{
			((std::ifstream*)m_pFile)->seekg(count, std::ios::cur);
		}

		size_t FileReader::Read(char* memory_block, size_t sizeInBytes)
		{
			((std::ifstream*)m_pFile)->read(memory_block, sizeInBytes);
			return (size_t)((std::ifstream*)m_pFile)->gcount();
		}

		bool FileReader::EndOfFile() const
		{
			return ((std::ifstream*)m_pFile)->eof();
		}
	}
}