#include "BowLogger.h"

#include <stdarg.h>
#include <memory>
#include <assert.h>
#include <cstring>
#include <string>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace bow
{
	static std::shared_ptr<EventLogger> Instance;
	const int MAX_DEBUG_LINE_LEN = 2048;

	EventLogger::EventLogger()
	{
		m_initialized = false;
	}


	EventLogger::~EventLogger()
	{
		Release();
	}


	void EventLogger::SetInstance(EventLogger& logger)
	{
		Instance = std::shared_ptr<EventLogger>(&logger, [](EventLogger* ptr){});
	}


	EventLogger& EventLogger::GetInstance()
	{
		if (Instance.get() == nullptr)
		{
			Instance = std::shared_ptr<EventLogger>(new EventLogger());
			Instance->Initialize(std::string("Log.txt").c_str());
		}
		return *Instance.get();
	}

	void EventLogger::InitLogFile()
	{
		m_logStream.open("Log.txt", std::fstream::out);

		if (m_logStream.is_open())
		{
			m_logStream.clear();
		}
	}


	bool EventLogger::Initialize(const char* logName)
	{
		if (IsInitialized())
			return false;

		InitLogFile();

		m_initialized = true;

		return true;
	}


	void EventLogger::Release()
	{
		m_logStream.close();

		m_initialized = false;
	}

	void EventLogger::LogTrace(const char* text, ...)
	{
		if (!IsInitialized())
			return;

		char buffer[MAX_DEBUG_LINE_LEN];

		va_list args;
		va_start(args, text);

#if defined(WINVER) || defined(_XBOX)
		int buf = _vsnprintf_s(buffer, MAX_DEBUG_LINE_LEN, text, args);
#else
		int buf = vsnprintf(buffer, MAX_DEBUG_LINE_LEN, text, args);
#endif

		assert((buf >= 0) && (buf < MAX_DEBUG_LINE_LEN));
		va_end(args);

		// Log output to debug windows and/or disk depending on options
		char buffer2[MAX_DEBUG_LINE_LEN];
		strcpy_s(buffer2, MAX_DEBUG_LINE_LEN, "TRACE: ");
		strcpy_s(buffer2 + 7, MAX_DEBUG_LINE_LEN - 7, buffer);
		LogOutput(buffer2);
	}

	void EventLogger::LogInfo(const char* text, ...)
	{
		if (!IsInitialized())
			return;

		char buffer[MAX_DEBUG_LINE_LEN];

		va_list args;
		va_start(args, text);

#if defined(WINVER) || defined(_XBOX)
		int buf = _vsnprintf_s(buffer, MAX_DEBUG_LINE_LEN, text, args);
#else
		int buf = vsnprintf(buffer, MAX_DEBUG_LINE_LEN, text, args);
#endif

		assert((buf >= 0) && (buf < MAX_DEBUG_LINE_LEN));
		va_end(args);

		// Log output to debug windows and/or disk depending on options
		char buffer2[MAX_DEBUG_LINE_LEN];
		strcpy_s(buffer2, MAX_DEBUG_LINE_LEN, "INFO: ");
		strcpy_s(buffer2 + 6, MAX_DEBUG_LINE_LEN - 6, buffer);
		LogOutput(buffer2);
	}

	void EventLogger::LogWarning(const char* text, ...)
	{
		if (!IsInitialized())
			return;

		char buffer[MAX_DEBUG_LINE_LEN];
		va_list args;
		va_start(args, text);

#if defined(WINVER) || defined(_XBOX)
		int buf = _vsnprintf_s(buffer, MAX_DEBUG_LINE_LEN, text, args);
#else
		int buf = vsnprintf(buffer, MAX_DEBUG_LINE_LEN, text, args);
#endif

		assert((buf >= 0) && (buf < MAX_DEBUG_LINE_LEN));
		va_end(args);

		// Log output to debug windows and/or disk depending on options
		char buffer2[MAX_DEBUG_LINE_LEN];
		strcpy_s(buffer2, MAX_DEBUG_LINE_LEN, "WARNING: ");
		strcpy_s(buffer2 + 9, MAX_DEBUG_LINE_LEN - 9, buffer);
		LogOutput(buffer2);
	}


	void EventLogger::LogError(const char* text, ...)
	{
		if (!IsInitialized())
			return;

		char buffer[MAX_DEBUG_LINE_LEN];
		va_list args;
		va_start(args, text);

#if defined(WINVER) || defined(_XBOX)
		int buf = _vsnprintf_s(buffer, MAX_DEBUG_LINE_LEN, text, args);
#else
		int buf = vsnprintf(buffer, MAX_DEBUG_LINE_LEN, text, args);
#endif

		assert((buf >= 0) && (buf < MAX_DEBUG_LINE_LEN));
		va_end(args);

		// Log output to debug windows and/or disk depending on options
		char buffer2[MAX_DEBUG_LINE_LEN];
		strcpy_s(buffer2, MAX_DEBUG_LINE_LEN, "ERROR: ");
		strcpy_s(buffer2 + 7, MAX_DEBUG_LINE_LEN - 7, buffer);
		LogOutput(buffer2);
	}

	void EventLogger::LogAssert(bool contidion, const char* file, long line, const char* description)
	{
		if (!contidion)
		{
			LogAssertAndShowWindow(std::string(std::string(description) + std::string(" - in file %s at line %u.")).c_str(), file, line);

#ifdef _DEBUG
			assert(contidion);
#endif
		}
	}

	void EventLogger::LogAssertAndShowWindow(const char* text, ...)
	{
		if (!IsInitialized())
			return;

		char buffer[MAX_DEBUG_LINE_LEN];
		va_list args;
		va_start(args, text);

#if defined(WINVER) || defined(_XBOX)
		int buf = _vsnprintf_s(buffer, MAX_DEBUG_LINE_LEN, text, args);
#else
		int buf = vsnprintf(buffer, MAX_DEBUG_LINE_LEN, text, args);
#endif

		assert((buf >= 0) && (buf < MAX_DEBUG_LINE_LEN));
		va_end(args);

		// Log output to debug windows and/or disk depending on options
		char buffer2[MAX_DEBUG_LINE_LEN];
		strcpy_s(buffer2, MAX_DEBUG_LINE_LEN, "FATAL ERROR: ");
		strcpy_s(buffer2 + 7, MAX_DEBUG_LINE_LEN - 7, buffer);
		LogOutput(buffer2);

#if defined(_WIN32)
		MessageBoxA(NULL, buffer, "LongBow - FATAL ERROR", MB_OK | MB_ICONERROR);
#else
		std::cerr << "An exception has occured: " << buffer << std::endl;
#endif

	}

	void EventLogger::LogOutput(char* buffer)
	{
		// Strip any unnecessary newline characters at the end of the buffer
		size_t i = strlen(buffer);
		if (i == 0)
			return;
		if (buffer[i - 1] == '\n')
			buffer[i - 1] = 0;

		m_logStream << buffer << '\n';

#ifdef _DEBUG
		DebugOutput(buffer);
#else
		std::cout << buffer << std::endl;
#endif
	}


#ifdef _DEBUG
	void EventLogger::DebugOutput(const char* buffer)
	{
#ifdef _WIN32
		char buf[MAX_DEBUG_LINE_LEN];
		sprintf_s(buf, "%s\n", buffer);
		std::cout << buf;
		OutputDebugStringA(buf);
#endif
	}
#endif

}
