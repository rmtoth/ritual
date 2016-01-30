#include "World.h"

static const int tileWidth = 64;
static const int tileHeight = 32;

World::World(SDL_Renderer *renderer, string filename)
{

	mCamX = 0.0f;
	mCamY = 0.0f;

	AddTile(renderer, 0, "assets/tile_concrete.png");
	AddTile(renderer, 1, "assets/tile_grass.png");
	AddTile(renderer, 2, "assets/tile_grass_flat.png");
	AddTile(renderer, 3, "assets/tile_gravel.png");

	u32 w, h;
	u8 *img;
	lodepng_decode32_file(&img, &w, &h, "assets/palette.png");
	for (u32 i = 0; i < w * h; i++) {
		u32 col = *((u32 *) &img[i * 4]) & 0x00FFFFFF;
		mPaletteMap[col] = i;
	}
	free(img);

	lodepng_decode32_file(&img, &mWidth, &mHeight, filename.c_str());
	mTiles = new int[mWidth * mHeight];

	for (u32 i = 0; i < mWidth * mHeight; i++) {
		u32 col = *((u32 *)&img[i * 4]) & 0x00FFFFFF;
		auto c = mPaletteMap.find(col);
		if (c == mPaletteMap.end()) {
			printf("Level color mismatch! (Color not in palette)");
			exit(1);
		}
		mTiles[i] = c->second;
	}
	free(img);
}

World::~World()
{

}

void World::AddTile(SDL_Renderer *renderer, int i, string filename)
{
	const u32 rmask = 0x000000ff;
	const u32 gmask = 0x0000ff00;
	const u32 bmask = 0x00ff0000;
	const u32 amask = 0xff000000;

	u8 *img;
	u32 w, h;
	lodepng_decode32_file(&img, &w, &h, filename.c_str());

	SDL_Surface *image = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
	SDL_LockSurface(image);
	memcpy(image->pixels, img, 4 * w * h);
	SDL_UnlockSurface(image);
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, image);
	free(img);

	TileType *tt = new TileType();
	tt->mTex = tex;
	tt->mW = int(w);
	tt->mH = int(h);

	mTileTypes[i] = tt;

	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
}



void World::Draw(SDL_Renderer *renderer)
{

	SDL_Rect srcrect;
	SDL_Rect dstrect;

	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = RES_X;
	srcrect.h = RES_Y;

	//for (int y = mHeight - 1; y >= 0; y--) {
	//for (int y = 3; y >= 0; y--) {

	int camX = int(mCamX + 0.5f);
	int camY = int(mCamY + 0.5f);

	for (u32 y = 0; y < mHeight; y++) {

		for (u32 x = 0; x < mWidth; x++) {

			int i = y * mWidth + x;
			int t = mTiles[i];
			TileType *tt = mTileTypes.find(t)->second;

			dstrect.x = camX + x * (tileWidth >> 1) - y * (tileWidth >> 1) + (tileWidth >> 1);
			dstrect.y = camY + x * (tileHeight >> 1) + y * (tileHeight >> 1) + (tileHeight >> 1);
			dstrect.w = tt->mW;
			dstrect.h = tt->mH;

			if (dstrect.x < -200 || dstrect.y < -200 || dstrect.x > RES_X + 200 || dstrect.y > RES_Y + 200)
				continue;

			SDL_RenderCopy(renderer, tt->mTex, &srcrect, &dstrect);
		}
	}
}

bool World::Event(SDL_Event &event)
{
	switch (event.type) {
	case SDL_MOUSEBUTTONDOWN: return MouseDown(event);
	case SDL_MOUSEBUTTONUP: return MouseUp(event);
	case SDL_MOUSEMOTION: return MouseMove(event);
	}
	return false;
}



bool World::MouseDown(SDL_Event &event)
{
	return false;
}

bool World::MouseMove(SDL_Event &event)
{
	if (event.button.button == 4) {
		mCamX += event.motion.xrel;
		mCamY += event.motion.yrel;
	}
	return true;
}

bool World::MouseUp(SDL_Event &event)
{
	return false;
}

