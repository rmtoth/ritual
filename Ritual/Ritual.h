#pragma once

#include "include\SDL.h"
#include "lodepng.h"
#include <vector>
#include <list>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include <limits>

using namespace std;

#define RES_X		1024
#define RES_Y		768
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

struct unit;
struct tower;

struct interaction {
	float t;
	unit *unit;
	tower *tower;
};

struct position {
	float t;
	int x, y;
};

struct health {
	float t;
	float hp;
	interaction *i; // What caused the change
};

struct unit {
	float spawntime;
	float deathtime;
	vector<position> path;
	vector<health> hp;
};

struct tower {
	float spawntime;
	int x, y;
	vector<interaction*> shots;
};

struct potential_field {
	float t0, t1;
	int w, h;
	int *next;
};

struct drawable {
	int sprite;
	float x, y;
};

void GetDrawables(float t, vector<drawable>&);
void TestSim();

extern vector<tower> g_towers;
extern vector<unit> g_units;
