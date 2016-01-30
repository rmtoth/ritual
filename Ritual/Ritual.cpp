// Ritual.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Ritual.h"
#include "World.h"
#include "Scrub.h"

World *g_world;
Scrub *g_scrub;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("Ritual", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RES_X, RES_Y, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	bool quit = false;

	g_world = new World(renderer, "assets/map");
	g_scrub = new Scrub(renderer);
	InitSim();

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
			if (g_scrub->Event(event)) continue;
			if (g_world->Event(event)) continue;
		}
		if (quit)
			break;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		g_world->Draw(renderer);
		g_scrub->Draw(renderer);

		SDL_RenderPresent(renderer);
	}

    return 0;
}

SDL_Texture *ImgToTex(SDL_Renderer *renderer, string filename, int &w, int &h)
{
	const u32 rmask = 0x000000ff;
	const u32 gmask = 0x0000ff00;
	const u32 bmask = 0x00ff0000;
	const u32 amask = 0xff000000;

	u8 *img;
	u32 uw, uh;
	lodepng_decode32_file(&img, &uw, &uh, filename.c_str());

	w = int(uw);
	h = int(uh);

	SDL_Surface *image = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
	SDL_LockSurface(image);
	memcpy(image->pixels, img, 4 * w * h);
	SDL_UnlockSurface(image);
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, image);
	free(img);

	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

	return tex;
}

