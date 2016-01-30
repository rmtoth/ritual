#pragma once

struct Button {

	Button(SDL_Renderer *renderer, string icon, int x, int y, int w, int h);
	void Draw(SDL_Renderer *renderer, bool selected);
	bool Event(SDL_Event &event);

	SDL_Rect mRect;

	SDL_Texture *mIcon;
	int mIconW, mIconH;
};

