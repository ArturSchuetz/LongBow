#include "BowFileWriter.h"

#include <iostream>
#include <fstream>

namespace bow
{

	FileWriter::FileWriter()
		: m_pFile(nullptr)
	{
		m_pFile = (void*)new std::ofstream();
	}

	FileWriter::~FileWriter()
	{ 
		Close();

		if (m_pFile != nullptr)
		{
			delete m_pFile;
			m_pFile = nullptr;
		}
	}

	bool FileWriter::Open(const char* filePath)
	{
		((std::ofstream*)m_pFile)->open(filePath, std::ios::in | std::ios::binary);
		if (!((std::ofstream*)m_pFile)->is_open())
		{
			return false;
		}

		((std::ofstream*)m_pFile)->seekp(0, std::ios::beg);

		return true;
	}

	void FileWriter::Close()
	{
		((std::ofstream*)m_pFile)->close();
	}

	size_t FileWriter::GetSizeOfFile()
	{
		// get current position
		std::streampos curr = ((std::ofstream*)m_pFile)->tellp();

		std::streampos begin = ((std::ofstream*)m_pFile)->tellp();
		((std::ofstream*)m_pFile)->seekp(0, std::ios::end);
		std::streampos end = ((std::ofstream*)m_pFile)->tellp();

		// restore position
		((std::ofstream*)m_pFile)->seekp(curr, std::ios::beg);
		return (size_t)(end - begin);
	}

	unsigned long FileWriter::Tell()
	{
		return (unsigned long)((std::ofstream*)m_pFile)->tellp();
	}

	void FileWriter::Seek(size_t offset)
	{
		((std::ofstream*)m_pFile)->seekp(offset, std::ios::beg);
	}

	void FileWriter::Skip(long count)
	{
		((std::ofstream*)m_pFile)->seekp(count, std::ios::cur);
	}

	size_t FileWriter::Write(char* memory_block, size_t sizeInBytes)
	{
		((std::ofstream*)m_pFile)->write(memory_block, sizeInBytes);
		return sizeInBytes;
	}

	bool FileWriter::EndOfFile() const
	{
		return ((std::ofstream*)m_pFile)->eof();
	}
}
