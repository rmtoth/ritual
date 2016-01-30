#pragma once

#include "include\SDL.h"
#include "lodepng.h"
#include <vector>
#include <list>
#include <queue>
#include <algorithm>
#include <math.h>
#include <limits>
#include "Misc.h"

using namespace std;

static const float inf = numeric_limits<float>::infinity();

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
