// Ritual.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Misc.h"
#include "include\SDL.h"
#include "include\SDL_main.h"
#include "include\SDL_surface.h"
#include "lodepng.h"

#define RES_X		640
#define RES_Y		480

//u32 rmask = 0xff000000;
//u32 gmask = 0x00ff0000;
//u32 bmask = 0x0000ff00;
//u32 amask = 0x000000ff;
u32 rmask = 0x000000ff;
u32 gmask = 0x0000ff00;
u32 bmask = 0x00ff0000;
u32 amask = 0xff000000;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("Ritual", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RES_X, RES_Y, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	bool quit = false;

	unsigned long long perfCnt = SDL_GetPerformanceCounter();
	unsigned long long perfFreq = SDL_GetPerformanceFrequency();
	double nowTime = double(perfCnt) / double(perfFreq);

	SDL_Surface *winSurf = SDL_GetWindowSurface(window);
	u8 *img;
	u32 imgw, imgh;
	lodepng_decode32_file(&img, &imgw, &imgh, "assets/tile.png");

	SDL_Surface *image = SDL_CreateRGBSurface(0, imgw, imgh, 32, rmask, gmask, bmask, amask);
	SDL_LockSurface(image);
	memcpy(image->pixels, img, 4 * imgw * imgh);
	SDL_UnlockSurface(image);
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, image);

	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

	SDL_Rect srcrect;
	SDL_Rect dstrect;

	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = RES_X;
	srcrect.h = RES_Y;

	dstrect.x = 0;
	dstrect.y = 0;
	dstrect.w = imgw;
	dstrect.h = imgh;


	for (;;) {

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

		SDL_RenderCopy(renderer, tex, &srcrect, &dstrect);

		SDL_RenderPresent(renderer);
	}

    return 0;
}



