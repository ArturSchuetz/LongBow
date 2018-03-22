#include "Application.h"

#include <iostream>
#include <chrono>
#include <thread>

double frameRate = 60.0;
double smoothing = 0.7;
double waitTimer = 0.0;

void waitForEndOfFrame(std::chrono::duration<double, std::milli> frameduration)
{
	// =======================================================
	// Calculate and display Framerime and Framerate

	frameRate = (frameRate * smoothing) + ((1000.0 / frameduration.count()) * (1.0 - smoothing));

	waitTimer += frameduration.count();
	if (waitTimer > 250.0)
	{
		// =======================================================
		// Calculate and display Framerime and Framerate

		char buff[5];
		snprintf(buff, sizeof(buff), "%f", frameduration.count());
		std::string frametimeStr = buff;

		std::cout << "Frametime: " << frametimeStr << " ms | Framerate: " << (int)frameRate << "\t\t\r" << std::flush;
		// ======================================================= 
		waitTimer = 0;
	}
}

int main()
{
	Application app;
	BasicTimer timer;
	if (app.Init())
	{
		while (app.IsRunning())
		{
			timer.Update();

			auto startFrame = std::chrono::high_resolution_clock::now(); // Take time before frame

			app.Update((float)timer.GetDelta());
			app.Render();

			auto endFrame = std::chrono::high_resolution_clock::now(); // Take time after frame

			waitForEndOfFrame(endFrame - startFrame);
		}
	}
	else
	{
		return -1;
	}
	return 0;
}
