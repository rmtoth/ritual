// Ritual.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Ritual.h"
#include "World.h"

World *world;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("Ritual", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RES_X, RES_Y, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	bool quit = false;

	world = new World(renderer, "assets/map.png");

	unsigned long long perfCnt = SDL_GetPerformanceCounter();
	unsigned long long perfFreq = SDL_GetPerformanceFrequency();
	double nowTime = double(perfCnt) / double(perfFreq);

	SDL_Surface *winSurf = SDL_GetWindowSurface(window);

	for (;;) {

		double lastTime = nowTime;
		perfCnt = SDL_GetPerformanceCounter();
		perfFreq = SDL_GetPerformanceFrequency();
		nowTime = double(perfCnt) / double(perfFreq);
		double ddeltatime = nowTime - lastTime;

		//printf("%f\n", ddeltatime);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = 1;
				break;
			}
			if (world->Event(event)) continue;
		}
		if (quit)
			break;

		SDL_RenderClear(renderer);

		world->Draw(renderer);

		SDL_RenderPresent(renderer);
	}

    return 0;
}



