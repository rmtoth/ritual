#pragma once

struct Scrub {

	Scrub(SDL_Renderer *renderer, World* world);
	void Draw(SDL_Renderer *renderer, int selectedTower, float time);
	bool Event(SDL_Event &event);

	SDL_Rect mRect;

	SDL_Texture *mMarker;
	int mMarkerW, mMarkerH;

	SDL_Texture *mEndMarker;
	int mEndMarkerW, mEndMarkerH;

	float mSpentTime;
	float mTime;
	float mSlider;
	bool mClaimedMouse;
	const float mTotalTime = 240.0f;

	void SetSlider(float x);
	void AdvanceTime(float x);
	World* mWorld;
};

