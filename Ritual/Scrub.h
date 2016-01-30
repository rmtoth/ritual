#pragma once

struct Scrub {

	Scrub(SDL_Renderer *renderer);
	void Draw(SDL_Renderer *renderer);
	bool Event(SDL_Event &event);

	SDL_Rect mRect;

	SDL_Texture *mMarker;
	int mMarkerW, mMarkerH;

	float mTime;
	bool mClaimedMouse;

};

