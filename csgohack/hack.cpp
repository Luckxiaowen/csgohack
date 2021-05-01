#include "cheat.h"


int main(int argc, char* argv[])
{
	bool isaim = false;
	inItGame();
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThred, NULL, NULL, NULL);
	while (1)
	{
		if (GetAsyncKeyState(VK_F1) & 1)
		{
			isaim = !isaim;
			printf("自瞄:%d\n", isaim);
		}

		glow();
		
		//aimbot();
		if (isaim)
		{
			aimBot();
			Sleep(10);
		}
		
		
	}
	return 0;
}