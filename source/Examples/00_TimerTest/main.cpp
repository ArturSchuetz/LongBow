#include "CoreSystems/BowBasicTimer.h"

#include <CoreSystems/BowLogger.h>

#include <optick.h>

#include <iostream>
#include <string>

int main(int /*argc*/, char * /*argv[]*/)
{
    FN("main");

    OPTICK_APP("Timer Test Sample");

    bow::BasicTimer timer;

    std::cout << "I will now count to ten:" << std::endl;

    // wait 10 seconds
    while (timer.GetTotal() < 10)
    {
        OPTICK_FRAME("MainThread");

        // Update timer to get new total time and new delta time
        timer.Update();

        std::cout << std::to_string(timer.GetTotal()) << "\r";
        LOG_UPDATE();
    }

    std::cout << std::endl;
    std::cout << "I am done!" << std::endl;

    OPTICK_SHUTDOWN();

    return 0;
}
