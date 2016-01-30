#include "Ritual.h"
#include "Scrub.h"


Scrub::Scrub(SDL_Renderer *renderer)
{
	mMarker = ImgToTex(renderer, "assets/marker.png", mMarkerW, mMarkerH);
	mTime = 0.0f;

	mRect.x = 100;
	mRect.y = RES_Y - 100;
	mRect.w = RES_X - 200;
	mRect.h = 56;
}

void Scrub::Draw(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, 128);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MOD);
	SDL_RenderFillRect(renderer, &mRect);

	SDL_Rect srcrect;
	SDL_Rect dstrect;

	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = RES_X;
	srcrect.h = RES_Y;

	dstrect.x = 100 + int(mTime * mRect.w + 0.5f) - (mMarkerW >> 1);
	dstrect.y = RES_Y - 100 - 4;
	dstrect.w = mMarkerW;
	dstrect.h = mMarkerH;

	SDL_RenderCopy(renderer, mMarker, &srcrect, &dstrect);
}

bool Scrub::Event(SDL_Event &event)
{
	static bool claimedMouse = false;

	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == 1 &&
		event.button.x >= mRect.x && event.button.y >= mRect.y &&
		event.button.x <= mRect.x + mRect.w && event.button.y <= mRect.y + mRect.h)
	{
		claimedMouse = true;
	} else if (event.type == SDL_MOUSEBUTTONUP)
		claimedMouse = false;
	else if (claimedMouse && event.type == SDL_MOUSEMOTION) {
		mTime = float(event.motion.x - mRect.x) / float(mRect.w);
		mTime = max(mTime, 0.0f);
		mTime = min(mTime, 1.0f);
	}


	return claimedMouse;
}


