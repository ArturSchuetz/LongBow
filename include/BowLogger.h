/**
 * @file BowLogger.h
 * @brief Declarations for BowLogger.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include <fstream>

// Chose one of this LogLevels to define LOG_LEVEL
#define LOG_LEVEL_ALL 0
#define LOG_LEVEL_TRACE 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_OFF 5

// Low Loglevel => Less important logs will be displayed.
#ifndef LOG_LEVEL
#ifdef _DEBUG
#define LOG_LEVEL	LOG_LEVEL_TRACE
#else
#define LOG_LEVEL	LOG_LEVEL_INFO
#endif
#endif

#if LOG_LEVEL <= LOG_LEVEL_TRACE
#define LOG_TRACE	bow::EventLogger::GetInstance().LogTrace
#else
#define LOG_TRACE
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO	bow::EventLogger::GetInstance().LogInfo
#else
#define LOG_INFO
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARNING
#define LOG_WARNING	bow::EventLogger::GetInstance().LogWarning
#else
#define LOG_WARNING
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR	bow::EventLogger::GetInstance().LogError
#else
#define LOG_ERROR
#endif

#define LOG_FATAL(var_1)			bow::EventLogger::GetInstance().LogAssert(false, __FILE__, __LINE__, var_1)
#define LOG_ASSERT(var_1, var_2)	bow::EventLogger::GetInstance().LogAssert(var_1, __FILE__, __LINE__, var_2)

namespace bow
{
	/**
	* \~german
	* \brief	Zeichnet die Ereignisse innerhalb der Engine auf.
	*
	* Der EvnetLogger ermöglicht verschiedene Ereignisse aufzuzeichnen und auf mehreren Ausgabeflächen gleichzeitig auszugeben.
	* Jeder Logeintrag wird sofort in eine Datei gestreamt und alle Logs sind auch nach einem Absturz verfügbar.
	**/
	class EventLogger
	{
	public:
		~EventLogger();

		/**
		* \~german
		* \brief	Diese Funktion sollte aufgerufen werden falls die geladene DLL in die selbe Log-Datei schreiben soll.
		*			Wenn man mit Dynamic Link Libraries arbeitet, dann funktionieren Singletons nicht innerhalb der DLL und
		*			es werden in jeder DLL eigene instanzen erstellt. Um das zu verhindern muss man die Instanz des Loggers
		*			aus dem Programm in die DLL einschleusen. Mit dieser Funktion wird die Instanz des Singletons verändert.
		*
		* \param newInstance	Die neu zu setzende Instanz der Singleton
		**/
		static void SetInstance(EventLogger& newInstance);

		/**
		* \~german
		* \brief	Gibt dir die aktuelle instanz der Singleton oder erstellt eine neue Instanz falls diese Funktion zum ersten mal aufgerufen wurde.
		*
		* \return	Die Instanz des Singletons
		**/
		static EventLogger& GetInstance();

		/**
		* \~german
		* \brief	Initialisiert den Logger und erstellt dabei eine Log-Datei.
		*
		* \param logName	Pfad zu der Logdatei in die geschrieben werden soll.
		*
		* \return	'true' wenn der Logger erfolgreich initialisiert wurde.
		**/
		bool Initialize(const char* logName);

		/**
		* \~german
		* \brief	Befreit die Logdatei.
		**/
		void Release();

		/**
		* \~german
		* \brief	Gibt auskunft darüber ob der Logger bereit ist.
		*
		* \return	'true' wenn die Logdatei initialisiert wurde.
		**/
		bool IsInitialized() { return m_initialized; }

		/**
		* \~german
		* \brief	Schreibt etwas in die Log Datei und in jeden verfügbaren Output.
		*
		* \param text	Der Logtext der in die Die Datei und in die Konsole ausgegeben werden soll.
		**/
		void LogTrace(const char* text, ...);

		/**
		* \~german
		* \brief	Schreibt etwas in die Log Datei und in jeden verfügbaren Output.
		*
		* \param text	Der Logtext der in die Die Datei und in die Konsole ausgegeben werden soll.
		**/
		void LogInfo(const char* text, ...);

		/**
		* \~german
		* \brief	Schreibt etwas in die Log Datei und in jeden verfügbaren Output.
		*
		* \param text	Der Logtext der in die Die Datei und in die Konsole ausgegeben werden soll.
		**/
		void LogWarning(const char* text, ...);

		/**
		* \~german
		* \brief	Schreibt etwas in die Log Datei und in jeden verfügbaren Output und zeigt zudem noch eine Fehler Messagebox an um den Nutzer über den Fehler zu informieren.
		*
		* \param text	Der Logtext der in die Die Datei und in die Konsole ausgegeben werden soll.
		**/
		void LogError(const char* text, ...);

		/**
		* \~german
		* \brief	Schreibt etwas in die Log Datei und in jeden verfügbaren Output und zeigt zudem noch eine Fehler Messagebox an um den Nutzer über den Fehler zu informieren. 
		*			Außerdem wird ein Assert ausgelöst und der Code stoppt an der Stelle.
		*
		* \param contidion	Bedingung unter der der Assert ausgelöst wurd: 'true' = Assert wird ausgelöst.
		*
		* \param file	Dateiname der Quellcode-Datei indem der Assert ausgelöst wurde
		*
		* \param line	Zeile des Quellcodes an dem der Assert ausgelöst wurde
		*
		* \param description	Beschreibung des Fehlers
		**/
		void LogAssert(bool contidion, const char* file, long line, const char* description);

	protected:
		EventLogger();

	private:
		EventLogger(const EventLogger&){}; // You shall not direct
		EventLogger& operator=(const EventLogger&) { return *this; }

		/**
		* \~german
		* \brief Initialisiert die Log Datei in die geschrieben wird.
		*
		* \~english
		* \brief Initialize log file.
		**/
		void InitLogFile();

		/**
		* \~german
		* \brief	Schreibt Text in die Textdatei und in die Konsole.
		*
		* \param buffer	Text der ausgegeben wird.
		**/
		void LogOutput(char* buffer);

#ifdef _DEBUG
		/**
		* \~german
		* \brief	Schreibt Text in den Debug-Output von Visual Studio.
		*
		* \param buffer	Text der ausgegeben wird.
		**/
		void DebugOutput(const char* buffer);
#endif

		void LogAssertAndShowWindow(const char* text, ...);

		bool            m_initialized;	//!< 'true' if the logger is ready		
		std::ofstream   m_logStream;	//!< Logfile stream on Harddisk
	};
}
