#pragma once

#include "include\SDL.h"
#include <vector>
#include <hash_map>
#include <hash_set>
#include <list>
#include <algorithm>
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
