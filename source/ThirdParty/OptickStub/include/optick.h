// Drop-in no-op replacement for the Optick profiler.
//
// The engine is instrumented with OPTICK_* macros in ~300 places. Rather than
// guarding every one of them, this header satisfies `#include <optick.h>` when
// LONGBOW_ENABLE_PROFILER is OFF and compiles all of them away. Turning the
// option ON puts the real Optick headers ahead of this one on the include path.

#pragma once

#define OPTICK_EVENT(...)      ((void)0)
#define OPTICK_CATEGORY(...)   ((void)0)
#define OPTICK_FRAME(...)      ((void)0)
#define OPTICK_APP(...)        ((void)0)
#define OPTICK_SHUTDOWN(...)   ((void)0)
