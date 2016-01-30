#include "Ritual.h"
#include "Button.h"


Button::Button(SDL_Renderer *renderer, string icon, int x, int y, int w, int h)
{
	mIcon = nullptr;
	mIcon = ImgToTex(renderer, icon, mIconW, mIconH);
	mRect.x = x;
	mRect.y = y;
	mRect.w = w;
	mRect.h = h;
}

void Button::Draw(SDL_Renderer *renderer, bool selected)
{
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MOD);
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, 128);
	SDL_RenderFillRect(renderer, &mRect);
	if (selected) {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 64);
		SDL_RenderFillRect(renderer, &mRect);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(renderer, &mRect);
	}
	else {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
		SDL_RenderDrawRect(renderer, &mRect);
	}

	SDL_Rect srcrect;
	SDL_Rect dstrect;

	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = RES_X;
	srcrect.h = RES_Y;

	dstrect.x = mRect.x + ((mRect.w - mIconW) >> 1);
	dstrect.y = mRect.y + ((mRect.h - mIconH) >> 1);
	dstrect.w = mIconW;
	dstrect.h = mIconH;

	SDL_RenderCopy(renderer, mIcon, &srcrect, &dstrect);
}

bool Button::Event(SDL_Event &event)
{
	if (event.button.x >= mRect.x && event.button.y >= mRect.y &&
		event.button.x <= mRect.x + mRect.w && event.button.y <= mRect.y + mRect.h)
	{
		if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == 1) {
			return true;
		}
	}

	return false;
}


