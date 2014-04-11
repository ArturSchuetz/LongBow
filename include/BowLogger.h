#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include <fstream>

// Chose one of this LogLevels to define LOG_LEVEL
#define LOG_LEVEL_ALL 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_OFF 5

// Low Loglevel means that less important logs will be displayed
#ifndef LOG_LEVEL
#ifdef _DEBUG
#define LOG_LEVEL	LOG_LEVEL_DEBUG
#else
#define LOG_LEVEL	LOG_LEVEL_WARNING
#endif
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG	Bow::Core::EventLogger::GetInstance().LogDebug
#else
#define LOG_DEBUG
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARNING
#define LOG_WARNING	Bow::Core::EventLogger::GetInstance().LogWarning
#else
#define LOG_WARNING
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR	Bow::Core::EventLogger::GetInstance().LogError
#else
#define LOG_ERROR
#endif

#define LOG_FATAL(var_1)			Bow::Core::EventLogger::GetInstance().LogAssert(false, __FILE__, __LINE__, var_1)
#define LOG_ASSERT(var_1, var_2)	Bow::Core::EventLogger::GetInstance().LogAssert(var_1, __FILE__, __LINE__, var_2)

namespace Bow
{
	namespace Core
	{
		//! \brief EventLogger writes logs into the console and a textfile and handles assertions and error-MessageBoxes.
		class EventLogger
		{
		public:
			~EventLogger();

			//! \brief This avoids the creations of a new instance
			//! If you are using the logger inside a dll, each module will create his own instance of the logger.
			//! You have to give an instance of the logger into every dll.
			static void SetInstance(EventLogger&);

			//! \brief Singleton instance.
			//! \return returns an instance or creates a new one
			static EventLogger& GetInstance();

			//! \brief Initialize log file.
			void InitLogFile();

			//! \brief Load File to stream into.
			//! \param logName Path to new logfile which will be created
			//! \return true if initialization succeeded
			bool Init(const char* logName);

			//! \brief Release stream logfile.
			void Release();

			//! \brief Is the event logging system initialized.
			//! \return true if the logfile is initialized
			bool IsInitialized() { return m_initialized; }

			//! \brief writes debug log into console and into the Logfile.
			//! You should use debug log if you want to present informations for debugging purposes.
			//! \param text logtext
			void LogDebug(const char* text, ...);

			//! \brief Informations which are not lead into a crash but maybe result in semantic errors.
			//! \param text logtext
			void LogWarning(const char* text, ...);

			//! \brief An errormessage will be written into the log and into the console and also will be shown in a messagebox
			//! \param text logtext
			void LogError(const char* text, ...);

			//! \brief An error which will do the same actions as logError but also triggers an assertion in debug.
			//! \param contidion The assert will be triggered if the condition is false
			//! \param file Filename where the assert is triggered
			//! \param line	Line of code where the assert is triggered
			//! \param description Brief descriptopn of the error
			void LogAssert(bool contidion, const char* file, long line, const char* description);

		protected:
			EventLogger();

		private:
			EventLogger(const EventLogger&){}; // You shall not copy
			EventLogger& operator=(const EventLogger&) { return *this; }

			//! \brief Write an text into the console and into a logfile.
			//! \param buffer text to log
			//! \param flags flags
			void LogOutput(char* buffer);

			//! \brief Outputs a text into the output
			void DebugOutput(const char* buffer);

			bool            m_initialized;	//!< True if the logger is ready		
			std::ofstream   m_logStream;	//!< Logfile stream on Harddisk
		};
	}
}