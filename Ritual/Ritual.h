#pragma once

#include "include\SDL.h"
#include "lodepng.h"
#include <vector>
#include <list>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <math.h>
#include <limits>

using namespace std;

#define RES_X		1600
#define RES_Y		900
static const float inf = numeric_limits<float>::infinity();

typedef float					f32;
typedef	double					f64;
typedef signed		char		i8;
typedef unsigned	char		u8;
typedef signed		short		i16;
typedef unsigned	short		u16;
typedef signed		int			i32;
typedef unsigned	int			u32;
typedef signed		__int64		i64;
typedef unsigned	__int64		u64;
typedef signed		long long	i64;
typedef unsigned	long long	u64;

struct i2 {
	int x, y;
};

struct f2 {
	float x, y;
};

struct unit;
struct tower;

struct position {
	float t;
	int x, y;
};

struct position_transition {
	int x0, y0;
	int x1, y1;
	float lerp;
};

struct health {
	float t;
	float hp;
};

struct span {
	float t0;
	float t1;
	bool operator()(float t) { return (t >= t0) && (t < t1); }
	span(float _t0 = 0, float _t1 = inf) : t0(_t0), t1(_t1) {}
};

struct unit {
	span alive;
	int type;
	vector<position> path;
	vector<health> hp;
};

struct shot {
	float t;
	int target;
};

struct tower {
	span alive;
	int x, y;
	int type;
	vector<shot> shots;
};

struct potential_field {
	span alive;
	int w, h;
	vector<int> next;
};

struct drawable {
	int sprite;
	float x, y;
	int variation = 0;
};

//== Sim API ==
void InitSim();
void GetDrawables(float t, vector<drawable>&);
bool BuildTower(float t, int x, int y, int type);
// TODO: bool SellTower(float t, int x, int y);
// TODO: bool EraseTower(float t, int x, int y);
float GetGameOverTime();
void SimDebugDraw(SDL_Renderer*, float t);
//==============

//== Nav API ===
void InitNav();
position_transition GetPositionTransition(unit &u, float t);
bool CreatePotentialField(float t);
void RecomputePath(unit &u, float t);
//==============

extern vector<tower> g_towers;
extern vector<unit> g_units;
extern struct World *g_world;
extern struct Scrub *g_scrub;

SDL_Texture *ImgToTex(SDL_Renderer *renderer, string filename, int &w, int &h);

