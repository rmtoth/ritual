#include "Ritual.h"
#include "Scrub.h"


Scrub::Scrub(SDL_Renderer *renderer)
{
	mMarker = ImgToTex(renderer, "assets/marker.png", mMarkerW, mMarkerH);
	mTime = 0.0f;
	mSlider = 0.0f;

	mRect.x = 100;
	mRect.y = RES_Y - 100;
	mRect.w = RES_X - 200;
	mRect.h = 56;

	mClaimedMouse = false;
}

void Scrub::Draw(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MOD);
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, 128);
	SDL_RenderFillRect(renderer, &mRect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &mRect);

	SDL_Rect srcrect;
	SDL_Rect dstrect;

	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = RES_X;
	srcrect.h = RES_Y;

	dstrect.x = 100 + int(mSlider * mRect.w + 0.5f) - (mMarkerW >> 1);
	dstrect.y = RES_Y - 100 - 4;
	dstrect.w = mMarkerW;
	dstrect.h = mMarkerH;

	SDL_RenderCopy(renderer, mMarker, &srcrect, &dstrect);
}

bool Scrub::Event(SDL_Event &event)
{
	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == 1 &&
		event.button.x >= mRect.x && event.button.y >= mRect.y &&
		event.button.x <= mRect.x + mRect.w && event.button.y <= mRect.y + mRect.h)
	{
		mClaimedMouse = true;
	} else if (event.type == SDL_MOUSEBUTTONUP)
		mClaimedMouse = false;
	else if (mClaimedMouse && event.type == SDL_MOUSEMOTION) {
		SetSlider(float(event.motion.x - mRect.x) / float(mRect.w));
	}

	return mClaimedMouse;
}

void Scrub::SetSlider(float x)
{
	mSlider = min(1.f, max(0.f, x));
	mTime = mSlider * 60;
}

void Scrub::AdvanceTime(float x)
{
	mTime += x;
	mSlider = min(1.f, max(0.f, mTime / 60));
}


