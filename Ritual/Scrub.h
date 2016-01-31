#pragma once

struct Scrub {

	Scrub(SDL_Renderer *renderer);
	void Draw(SDL_Renderer *renderer, int selectedTower, float time);
	bool Event(SDL_Event &event);

	SDL_Rect mRect;

	SDL_Texture *mMarker;
	int mMarkerW, mMarkerH;

	float mSpentTime;
	float mTime;
	float mSlider;
	bool mClaimedMouse;
	const float mTotalTime = 240.0f;

	void SetSlider(float x);
	void AdvanceTime(float x);
};

