#pragma once
#include "Ritual.h"
#include "AudioManager.h"

struct World {


	float mCamX, mCamY;
	int mMouseX, mMouseY;
	
	SDL_Texture *mMarker;
	int mMarkerW, mMarkerH;
	
	SDL_Texture *mShadow;
	int mShadowW, mShadowH;

	struct TileType {
		SDL_Texture *mTex;
		int mW, mH;
	};

	/*
	struct ObjectRender
	{
		TileType* mType;
		SDL_Rect mRect;
	};
	*/
	vector<drawable> objectsToRender;
	vector<drawable> unitsToRender;
	vector<drawable> doodadToRender;

	unordered_map<int, TileType *> mTileTypes;
	unordered_map<int, vector<TileType *>> mObjectTypes;

	int *mTiles;
	int *mTileObjects;
	float *mWalkCost;

	i2 mDest;
	vector<i2> mSpawn;

	SDL_Texture *mTextures;

	unsigned int mWidth, mHeight;
	unordered_map<u32, int> mPaletteMap;

	World(SDL_Renderer *renderer, string filename);
	~World();

	void ProcessObjectColor(u32 col, u32 tileIndex);
	void AddTile(SDL_Renderer *renderer, int i, string filename);
	void AddObject(SDL_Renderer *renderer, int i, string filename);

	void Draw(SDL_Renderer *renderer);
	void DrawMarker(SDL_Renderer *renderer);

	bool Event(SDL_Event &event, int selectedTower);
	bool MouseDown(SDL_Event &event, int selectedTower);
	bool MouseMove(SDL_Event &event);
	bool MouseUp(SDL_Event &event);

	void ScreenToWorld(float &wx, float &wy, float sx, float sy);
	void WorldToScreen(float &sx, float &sy, float wx, float wy);

	void RenderIsoSprite(SDL_Renderer *renderer, SDL_Texture &tex, int x, int y, int w, int h);

	CAudioManager myAudioManager;
	int mScrubSoundID;
};


