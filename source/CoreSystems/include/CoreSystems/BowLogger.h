#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "CoreSystems/CoreSystems_api.h"

#include <fstream>
#include <vector>

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
#define LOG_LEVEL LOG_LEVEL_TRACE
#else
#define LOG_LEVEL LOG_LEVEL_INFO
#endif
#endif

#if LOG_LEVEL <= LOG_LEVEL_TRACE
#define LOG_TRACE bow::EventLogger::GetInstance().LogTrace
#else
#define LOG_TRACE
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO bow::EventLogger::GetInstance().LogInfo
#else
#define LOG_INFO
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARNING
#define LOG_WARNING bow::EventLogger::GetInstance().LogWarning
#else
#define LOG_WARNING
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR bow::EventLogger::GetInstance().LogError
#else
#define LOG_ERROR
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define FN(var_1) bow::FunctionScope obj____unique_name(var_1)
#define LOG_UPDATE bow::EventLogger::GetInstance().Update
#else
#define FN(var_1)
#define LOG_UPDATE
#endif

#define LOG_FATAL(var_1) bow::EventLogger::GetInstance().LogAssert(false, __FILE__, __LINE__, var_1)
#define LOG_ASSERT(var_1, var_2) bow::EventLogger::GetInstance().LogAssert(var_1, __FILE__, __LINE__, var_2)

namespace bow
{

class CORESYSTEMS_API FunctionScope
{
  public:
    FunctionScope(const char *functionName);
    ~FunctionScope();

  private:
    const char *m_function;
};

/**
 * \~german
 * \brief	Zeichnet die Ereignisse innerhalb der Engine auf.
 *
 * Der EvnetLogger erm�glicht verschiedene Ereignisse aufzuzeichnen und auf
 *mehreren Ausgabefl�chen gleichzeitig auszugeben. Jeder Logeintrag wird sofort
 *in eine Datei gestreamt und alle Logs sind auch nach einem Absturz verf�gbar.
 **/
class CORESYSTEMS_API EventLogger
{
  public:
    ~EventLogger();

    /**
     * \~german
     * \brief	Diese Funktion sollte aufgerufen werden falls die geladene DLL
     *in die selbe Log-Datei schreiben soll. Wenn man mit Dynamic Link Libraries
     *arbeitet, dann funktionieren Singletons nicht innerhalb der DLL und es
     *werden in jeder DLL eigene instanzen erstellt. Um das zu verhindern muss
     *man die Instanz des Loggers aus dem Programm in die DLL einschleusen. Mit
     *dieser Funktion wird die Instanz des Singletons ver�ndert.
     *
     * \param newInstance	Die neu zu setzende Instanz der Singleton
     **/
    static void SetInstance(EventLogger &newInstance);

    static bool HasInstance();

    /**
     * \~german
     * \brief	Gibt dir die aktuelle instanz der Singleton oder erstellt eine
     *neue Instanz falls diese Funktion zum ersten mal aufgerufen wurde.
     *
     * \return	Die Instanz des Singletons
     **/
    static EventLogger &GetInstance();

    /**
     * \~german
     * \brief	Initialisiert den Logger und erstellt dabei eine Log-Datei.
     *
     * \param logName	Pfad zu der Logdatei in die geschrieben werden soll.
     *
     * \return	'true' wenn der Logger erfolgreich initialisiert wurde.
     **/
    bool Initialize();

    /**
     * \~german
     * \brief	Befreit die Logdatei.
     **/
    void Release();

    /**
     * \~german
     * \brief	Gibt auskunft dar�ber ob der Logger bereit ist.
     *
     * \return	'true' wenn die Logdatei initialisiert wurde.
     **/
    bool IsInitialized() { return m_initialized; }

    /**
     * \~german
     * \brief	Schreibt etwas in die Log Datei und in jeden verf�gbaren Output.
     *
     * \param text	Der Logtext der in die Die Datei und in die Konsole
     *ausgegeben werden soll.
     **/
    void LogTrace(const char *text, ...);

    /**
     * \~german
     * \brief	Schreibt etwas in die Log Datei und in jeden verf�gbaren Output.
     *
     * \param text	Der Logtext der in die Die Datei und in die Konsole
     *ausgegeben werden soll.
     **/
    void LogInfo(const char *text, ...);

    /**
     * \~german
     * \brief	Schreibt etwas in die Log Datei und in jeden verf�gbaren Output.
     *
     * \param text	Der Logtext der in die Die Datei und in die Konsole
     *ausgegeben werden soll.
     **/
    void LogWarning(const char *text, ...);

    /**
     * \~german
     * \brief	Schreibt etwas in die Log Datei und in jeden verf�gbaren Output
     *und zeigt zudem noch eine Fehler Messagebox an um den Nutzer �ber den
     *Fehler zu informieren.
     *
     * \param text	Der Logtext der in die Die Datei und in die Konsole
     *ausgegeben werden soll.
     **/
    void LogError(const char *text, ...);

    /**
     * \~german
     * \brief	Schreibt etwas in die Log Datei und in jeden verf�gbaren Output
     *und zeigt zudem noch eine Fehler Messagebox an um den Nutzer �ber den
     *Fehler zu informieren. Au�erdem wird ein Assert ausgel�st und der Code
     *stoppt an der Stelle.
     *
     * \param contidion	Bedingung unter der der Assert ausgel�st wurd: 'true' =
     *Assert wird ausgel�st.
     *
     * \param file	Dateiname der Quellcode-Datei indem der Assert ausgel�st
     *wurde
     *
     * \param line	Zeile des Quellcodes an dem der Assert ausgel�st wurde
     *
     * \param description	Beschreibung des Fehlers
     **/
    void LogAssert(bool contidion, const char *file, long line, const char *description);

    void PushFunction(const char *name);

    void PopFunction();

    void Update();

  protected:
    EventLogger();

  private:
    EventLogger(const EventLogger &); // You shall not direct
    EventLogger &operator=(const EventLogger &) { return *this; }

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
    void LogOutput(char *buffer);

#ifdef _DEBUG
    /**
     * \~german
     * \brief	Schreibt Text in den Debug-Output von Visual Studio.
     *
     * \param buffer	Text der ausgegeben wird.
     **/
    void DebugOutput(const char *buffer);
#endif

    void LogAssertAndShowWindow(const char *text, ...);

    //! Whether fatal errors must not open a modal dialog or trip an assert.
    /*!
        Set LONGBOW_NO_ERROR_DIALOG in the environment for unattended runs --
        tests, build servers, batch rendering. Fatal errors then go to stderr
        and the log like every other message instead of waiting for a click.
    */
    static bool InteractiveErrorsDisabled();

    std::vector<const char *> m_callStack;
    unsigned int m_previousStackLevel;
    bool m_initialized;        //!< 'true' if the logger is ready
    std::ofstream m_logStream; //!< Logfile stream on Harddisk
};

} // namespace bow