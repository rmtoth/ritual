#pragma once
#include "Ritual.h"

struct World {

	float mCamX, mCamY;


	struct TileType {
		SDL_Texture *mTex;
		int mW, mH;
	};
	unordered_map<int, TileType *> mTileTypes;

	int *mTiles;
	float *mWalkCost;

	SDL_Texture *mTextures;

	unsigned int mWidth, mHeight;
	unordered_map<u32, int> mPaletteMap;


	World(SDL_Renderer *renderer, string filename);
	~World();

	void AddTile(SDL_Renderer *renderer, int i, string filename);

	void Draw(SDL_Renderer *renderer);

	bool Event(SDL_Event &event);
	bool MouseDown(SDL_Event &event);
	bool MouseMove(SDL_Event &event);
	bool MouseUp(SDL_Event &event);


};


