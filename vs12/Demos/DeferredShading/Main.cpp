#include "GameShell.h"

int main()
{
	DWORD dt, start_time = 0;
	float FrameRate, FrameTime = 0.0f;

	GameShell game;
	if (game.Init())
	{
		start_time = GetTickCount();
		while (game.IsRunning())
		{
			dt = GetTickCount() - start_time;
			start_time = GetTickCount();

			game.Update(dt);
			game.Render();

			FrameRate = 1000.0f / dt;
			FrameTime = (0.001f*dt) / 10;
		}
	}
	else
	{
		return -1;
	}
	return 0;
}
