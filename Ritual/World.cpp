#include "Ritual.h"
#include "World.h"
#include "Scrub.h"
#include <algorithm>

static const int tileWidth = 64;
static const int tileHeight = 32;

static const int spawnPointColor = 32;

SDL_Point attackCircle[10][65];

World::TileType demon[3][3][8];

World::World(SDL_Renderer *renderer, string filename)
{
	mCamX = 0.0f;
	mCamY = 0.0f;

	myAudioManager.Init();

	AddTile(renderer, 0, "assets/tile_gravel2.png");
	AddTile(renderer, 1, "assets/tile_grass.png");
	AddTile(renderer, 2, "assets/tile_grass_flat2.png");
	AddTile(renderer, 3, "assets/tile_concrete.png");
	AddTile(renderer, 4, "assets/tile_grass.png");
	AddTile(renderer, 6, "assets/tile_grass_flat_wall.png");
	AddTile(renderer, 7, "assets/tile_grass_flat_wall_2.png");
	AddTile(renderer, 8, "assets/tile_grass_flat_wall_3.png");
	AddTile(renderer, 32, "assets/tile_spawn.png");

	AddObject(renderer, 5, "assets/tile_tree1.png");
	AddObject(renderer, 5, "assets/tile_tree2.png");
	AddObject(renderer, 5, "assets/tile_tree3.png");
	AddObject(renderer, 5, "assets/tile_tree4.png");
	AddObject(renderer, 32, "assets/tile_spawn.png");
	AddObject(renderer, 50, "assets/tower1.png");
	AddObject(renderer, 80, "assets/tower1.png");

	u32 w, h;
	u8 *img;
	lodepng_decode32_file(&img, &w, &h, "assets/palette.png");
	for (u32 i = 0; i < w * h; i++) {
		u32 col = *((u32 *) &img[i * 4]) & 0x00FFFFFF;
		mPaletteMap[col] = i;
	}
	free(img);

	string fn = filename + ".png";

	lodepng_decode32_file(&img, &mWidth, &mHeight, fn.c_str());
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

	fn = filename + "_weights.png";
	lodepng_decode32_file(&img, &w, &h, fn.c_str());
	mWalkCost = new float[mWidth * mHeight];
	for (u32 i = 0; i < mWidth * mHeight; i++) {
		u32 col = *((u32 *)&img[i * 4]) & 0x000000FF;
		mWalkCost[i] = float(col) / 255.0f;
	}
	free(img);

	// Objects, like trees
	fn = filename + "_objects.png";
	lodepng_decode32_file(&img, &w, &h, fn.c_str());
	mTileObjects = new int[mWidth * mHeight];
	for (u32 i = 0; i < mWidth * mHeight; i++)
	{
		u32 col = *((u32 *)&img[i * 4]) & 0x00FFFFFF;
		ProcessObjectColor(col, i);

		auto c = mPaletteMap.find(col);
		if (c == mPaletteMap.end()) {
			printf("Level color mismatch! (Color not in palette)");
			exit(1);
		}
		if (c->second < 120) {
			drawable d;
			d.sprite = c->second;
			d.x = float(i % mWidth);
			d.y = float(i / mWidth);
			d.variation = rand() % mObjectTypes[d.sprite].size();
			d.health = 0.0f;
			mWalkCost[i] = d.sprite == spawnPointColor ? mWalkCost[i] : 0;
			objectsToRender.push_back(d);
		}

	}
	free(img);

	mMarker = ImgToTex(renderer, "assets/tile_marker.png", mMarkerW, mMarkerH);
	mShadow = ImgToTex(renderer, "assets/shadow.png", mShadowW, mShadowH);

	for (int i = 0; i < 1; i++) {
		float r = sqrtf(tower_types[i].range2);
		for (int k = 0; k < 65; k++) {
			float f = 2.0f * float(M_PI) * (k / 64.0f);
			float fx = cosf(f) * r;
			float fy = sinf(f) * r;
			float ax, ay;
			WorldToScreen(ax, ay, fx, fy);
			attackCircle[i][k].x = int(ax + 0.5f);
			attackCircle[i][k].y = int(ay + 0.5f);
		}
	}

	for (int sz = 0; sz < 3; sz++) {
		for (int frame = 0; frame < 3; frame++) {
			for (int rot = 0; rot < 8; rot++) {
				char buf[1024];
				sprintf(buf, "assets/demon%d_w%d_000%d.png", sz, frame, rot);

				SDL_Texture *tex = ImgToTex(renderer, buf, demon[sz][frame][rot].mW, demon[sz][frame][rot].mH);
				demon[sz][frame][rot].mTex = tex;
			}

		}

	}


	mDest = { 32, 32 };

	//myAudioManager.PlaySound("assets/audio/music.mp3");
}

World::~World()
{
	delete [] mTiles;
	delete[] mTileObjects;
}

void World::ProcessObjectColor(u32 col, u32 tileIndex)
{
	bool isSpawnPoint = (spawnPointColor == mPaletteMap[col]);
	if (isSpawnPoint)
	{
		int x = tileIndex % mWidth;
		int y = tileIndex / mWidth;

		mSpawn.push_back({ x, y });
	}

}

void World::AddTile(SDL_Renderer *renderer, int i, string filename)
{
	int w, h;
	SDL_Texture *tex = ImgToTex(renderer, filename, w, h);

	TileType *tt = new TileType();
	tt->mTex = tex;
	tt->mW = w;
	tt->mH = h;

	mTileTypes[i] = tt;

	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
}

void World::AddObject(SDL_Renderer *renderer, int i, string filename)
{
	int w, h;
	SDL_Texture *tex = ImgToTex(renderer, filename, w, h);

	TileType *tt = new TileType();
	tt->mTex = tex;
	tt->mW = w;
	tt->mH = h;

	mObjectTypes[i].push_back(tt);

	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
}


void World::Draw(SDL_Renderer *renderer)
{

	int camX = int(mCamX + 0.5f);
	int camY = int(mCamY + 0.5f);

	float fx, fy;

	for (u32 y = 0; y < mHeight; y++) {
		for (u32 x = 0; x < mWidth; x++) {
			int i = y * mWidth + x;
			int t = mTiles[i];
			TileType *tt = mTileTypes.find(t)->second;

			WorldToScreen(fx, fy, float(x), float(y));

			if (fx < -200 || fy < -200 || fx > RES_X + 200 || fy > RES_Y + 200)
				continue;

			RenderIsoSprite(renderer, *tt->mTex, int(fx), int(fy), tt->mW, tt->mH);
		}
	}


	unitsToRender.clear();
	GetDrawables(g_scrub->mTime, unitsToRender);

	doodadToRender.clear();

	doodadToRender.insert(doodadToRender.end(), unitsToRender.begin(), unitsToRender.end());
	doodadToRender.insert(doodadToRender.end(), objectsToRender.begin(), objectsToRender.end());
	
	struct sorter_ {
		bool operator() (drawable &i, drawable &j) {
			return (i.y<j.y);
		}
	} sorter;
	sort(doodadToRender.begin(), doodadToRender.end(), sorter);

	for (drawable& d : doodadToRender)
	{
		WorldToScreen(fx, fy, float(d.x), float(d.y));

		TileType *tt = mObjectTypes[d.sprite][d.variation];

		RenderIsoSprite(renderer, *mShadow, int(fx), int(fy), mShadowW, mShadowH);



		RenderIsoSprite(renderer, *tt->mTex, int(fx), int(fy), tt->mW, tt->mH);
	}
	
	for (drawable& d : doodadToRender)
	{
		if (d.health == 0.0f)
			continue;
		WorldToScreen(fx, fy, float(d.x), float(d.y));
		SDL_Rect rect;
		rect.x = int(fx) - 16;
		rect.y = int(fy) + 10;
		rect.w = 32;
		rect.h = 8;
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MOD);
		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 128);
		SDL_RenderFillRect(renderer, &rect);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(renderer, &rect);
		rect.x += 2;
		rect.y += 2;
		rect.w = int(32.0f * d.health + 0.5f);
		rect.w -= 4;
		rect.h -= 4;
		SDL_SetRenderDrawColor(renderer, 20, 180, 20, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
}

void World::DrawMarker(SDL_Renderer *renderer)
{
	float wx, wy;
	ScreenToWorld(wx, wy, float(mMouseX), float(mMouseY));

	if (wx < 0 || wy < 0 || wx >= mWidth || wy >= mHeight)
		return;

	float sx, sy;
	WorldToScreen(sx, sy, floor(wx + 0.5f), floor(wy + 0.5f));

	RenderIsoSprite(renderer, *mMarker, int(sx), int(sy), mMarkerW, mMarkerH);

	int ix = int(wx + 0.5f);
	int iy = int(wy + 0.5f);

	unitsToRender.clear();
	GetDrawables(g_scrub->mTime, unitsToRender);

	for (auto it : unitsToRender) {
		int ax = int(it.x);
		int ay = int(it.y);
		if (ax == ix && ay == iy) {
			if (it.sprite >= 50 && it.sprite <= 55) {
				int i = it.sprite - 50;
				SDL_Point ac[65];
				memcpy(ac, attackCircle[i], sizeof(SDL_Point) * 65);
				for (int k = 0; k < 65; k++) {
					ac[k].x += int(sx) + (tileWidth >> 1);
					ac[k].y += int(sy) + (tileHeight >> 1);
				}
				SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderDrawLines(renderer, ac, 65);
				break;
			}
		}
	}

}


bool World::Event(SDL_Event &event, int selectedTower)
{
	switch (event.type) {
	case SDL_MOUSEBUTTONDOWN: return MouseDown(event, selectedTower);
	case SDL_MOUSEBUTTONUP: return MouseUp(event);
	case SDL_MOUSEMOTION: return MouseMove(event);
	}
	return false;
}



bool World::MouseDown(SDL_Event &event, int selectedTower)
{
	float wx, wy;
	ScreenToWorld(wx, wy, float(mMouseX), float(mMouseY));

	if (wx < 0 || wy < 0 || wx >= mWidth || wy >= mHeight)
		return false;

	int buildX = int(wx + 0.5f);
	int buildY = int(wy + 0.5f);
	for (auto &it : objectsToRender) {
		if (int(it.x) == buildX && int(it.y) == buildY)
			return false;
	}

	if (event.button.button == 1) {
		if (tower_types[selectedTower].cost > g_scrub->mTotalTime - g_scrub->mSpentTime)
			return true;

 		bool didBuild = BuildTower(g_scrub->mTime, buildX, buildY, selectedTower);
		if (didBuild) {
			g_scrub->mSpentTime += tower_types[selectedTower].cost;
		}
		return true;
	}

	return false;
}

bool World::MouseMove(SDL_Event &event)
{
	if (event.button.button == 4) {
		mCamX += event.motion.xrel;
		mCamY += event.motion.yrel;
	}
	mMouseX = event.motion.x;
	mMouseY = event.motion.y;

	return true;
}

bool World::MouseUp(SDL_Event &event)
{
	return false;
}

void World::ScreenToWorld(float &wx, float &wy, float sx, float sy)
{
	sx -= mCamX;
	sy -= mCamY;
	float tw = float(tileWidth >> 1);
	float th = float(tileHeight >> 1);
	wx = 0.5f * (sy / th + sx / tw) + 1.0f;
	wy = 0.5f * (sy / th - sx / tw);
}

void World::WorldToScreen(float &sx, float &sy, float wx, float wy)
{
	float tw = float(tileWidth >> 1);
	float th = float(tileHeight >> 1);
	sx = wx * tw - wy * tw - tw + mCamX;
	sy = wx * th + wy * th - th + mCamY;
	//sx = wx * tw - wy * tw + tw;
	//sy = wx * th + wy * th + th;
}


void World::RenderIsoSprite(SDL_Renderer *renderer, SDL_Texture &tex, int x, int y, int w, int h)
{
	SDL_Rect srcrect;
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = RES_X;
	srcrect.h = RES_Y;

	SDL_Rect dstrect;
	dstrect.x = x - (w >> 1);
	dstrect.y = y - (h >> 1) - (tileHeight >> 1);
	dstrect.w = w;
	dstrect.h = h;

	SDL_RenderCopy(renderer, &tex, &srcrect, &dstrect);
}
