// Ritual.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Ritual.h"
#include "include\SDL_main.h"

#define RES_X		640
#define RES_Y		480


int main(int argc, const char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("Ritual", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RES_X, RES_Y, 0);

	bool quit = false;

	unsigned long long perfCnt = SDL_GetPerformanceCounter();
	unsigned long long perfFreq = SDL_GetPerformanceFrequency();
	double nowTime = double(perfCnt) / double(perfFreq);

	while (!quit) {

		double lastTime = nowTime;
		perfCnt = SDL_GetPerformanceCounter();
		perfFreq = SDL_GetPerformanceFrequency();
		nowTime = double(perfCnt) / double(perfFreq);
		double ddeltatime = nowTime - lastTime;

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = 1;
				break;
			}
		}
		if (quit)
			break;
	}

    return 0;
}



