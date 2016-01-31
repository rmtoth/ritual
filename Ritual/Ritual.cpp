// Ritual.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Ritual.h"
#include "World.h"
#include "Scrub.h"
#include "Button.h"

World *g_world;
Scrub *g_scrub;

const int nButtons = 4;
Button *g_buttons[nButtons];
Button *g_playpause[2];
int selectedButton = 0;
bool isPlaying = false;

double nowTime;

int errMsg = errNone;
double errEndTime;

SDL_Texture* errorTextures[numErrorMessages];

void ShowErrorMessage(ErrorMsg e)
{
	errMsg = e;
	errEndTime = nowTime + 2.0f;
}

void DrawErrorMessage(SDL_Renderer *r)
{
	if (errEndTime <= nowTime) errMsg = errNone;
	if (errMsg == errNone) return;
	double alpha = min(1.0, errEndTime - nowTime);

	SDL_Rect rc;
	SDL_Texture* tex = errorTextures[errMsg];
	SDL_QueryTexture(tex, NULL, NULL, &rc.w, &rc.h);
	rc.w /= 2;
	rc.h /= 2;

	rc.x = 800 - rc.w / 2;
	rc.y = 450 - rc.h / 2;

	SDL_RenderCopy(r, tex, nullptr, &rc);

	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_ADD);
	SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
	SDL_RenderDrawRect(r, &rc);

}

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("Ritual", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RES_X, RES_Y, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	int ww, hh;
	errorTextures[0] = ImgToTex(renderer, "assets/error1.png", ww, hh);
	errorTextures[1] = ImgToTex(renderer, "assets/error1.png", ww, hh);
	errorTextures[2] = ImgToTex(renderer, "assets/error2.png", ww, hh);
	errorTextures[3] = ImgToTex(renderer, "assets/error3.png", ww, hh);
	errorTextures[4] = ImgToTex(renderer, "assets/error4.png", ww, hh);
	errorTextures[5] = ImgToTex(renderer, "assets/error5.png", ww, hh);
	errorTextures[6] = ImgToTex(renderer, "assets/error6.png", ww, hh);
	errorTextures[7] = ImgToTex(renderer, "assets/error7.png", ww, hh);
	errorTextures[8] = ImgToTex(renderer, "assets/error8.png", ww, hh);

	bool quit = false;

	g_world = new World(renderer, "assets/map");
	g_scrub = new Scrub(renderer);
	//for (int i = 0; i < 5; i++)
	g_buttons[0] = new Button(renderer, "assets/tower3.png", 100, 100 + 0 * 110, 100, 100);
	g_buttons[1] = new Button(renderer, "assets/tower2.png", 100, 100 + 1 * 110, 100, 100);
	g_buttons[2] = new Button(renderer, "assets/tower1.png", 100, 100 + 2 * 110, 100, 100);
	g_buttons[3] = new Button(renderer, "assets/tower4.png", 100, 100 + 3 * 110, 100, 100);

	g_playpause[0] = new Button(renderer, "assets/play.png", RES_X - 280, RES_Y - 200, 80, 80);
	g_playpause[1] = new Button(renderer, "assets/pause.png", RES_X - 180, RES_Y - 200, 80, 80);

	InitNav();
	InitSim();

	unsigned long long perfCnt = SDL_GetPerformanceCounter();
	unsigned long long perfFreq = SDL_GetPerformanceFrequency();
	nowTime = double(perfCnt) / double(perfFreq);

	SDL_Surface *winSurf = SDL_GetWindowSurface(window);

	bool win = false;

	for (;;) {

		double lastTime = nowTime;
		perfCnt = SDL_GetPerformanceCounter();
		perfFreq = SDL_GetPerformanceFrequency();
		nowTime = double(perfCnt) / double(perfFreq);
		double ddeltatime = nowTime - lastTime;
		ddeltatime = ddeltatime > 0.1 ? 0.1 : ddeltatime;

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = 1;
				break;
			}
			if (g_scrub->Event(event)) continue;
			bool eatenByButton = false;
			for (int i = 0; i < nButtons; i++) {
				bool hit = g_buttons[i]->Event(event);
				if (hit)
					selectedButton = i;
				eatenByButton |= hit;
			}
			if (eatenByButton)
				continue;
			if (g_playpause[0]->Event(event)) {
				isPlaying = true;
				continue;
			}
			if (g_playpause[1]->Event(event)) {
				isPlaying = false;
				continue;
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
				isPlaying = !isPlaying;
			}
			if (g_scrub->Event(event)) continue;
			if (!win) {
				if (g_world->Event(event, selectedButton)) continue;
			}
		}
		if (quit)
			break;

		if (isPlaying) {
			g_scrub->AdvanceTime(float(ddeltatime));
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		g_world->Draw(renderer);
		if (!g_scrub->mClaimedMouse)
			g_world->DrawMarker(renderer);
		g_scrub->Draw(renderer, selectedButton, float(nowTime));
		for (int i = 0; i < nButtons; i++)
			g_buttons[i]->Draw(renderer, i == selectedButton);
		g_playpause[0]->Draw(renderer, isPlaying);
		g_playpause[1]->Draw(renderer, !isPlaying);
		SimDebugDraw(renderer, g_scrub->mTime);

		if (win) {
			ShowErrorMessage(errWin);
		}
		else {
			if (GetGameOverTime() / g_scrub->mTotalTime >= 1.0f)
				win = true;
		}

		DrawErrorMessage(renderer);

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
