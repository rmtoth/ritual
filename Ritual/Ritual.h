#pragma once

#include "include\SDL.h"
#include "lodepng.h"
#include <vector>
#include <list>
#include <algorithm>
#include "Misc.h"
using namespace std;

struct unit;
struct tower;

struct interaction {
	float t;
	unit *unit;
	tower *tower;
};

struct position {
	float t;
	float x, y;
};

struct health {
	float t;
	float hp;
	interaction *i; // What caused the change
};

struct unit {
	float spawntime;
	vector<position> path;
	vector<health> hp;
};

struct tower {
	float spawntime;
	int x, y;
	vector<interaction*> shots;
};

struct drawable {
	int sprite;
	float x, y;
};

void GetDrawables(float t, vector<drawable>&);

extern vector<tower> g_towers;
extern vector<unit> g_units;
