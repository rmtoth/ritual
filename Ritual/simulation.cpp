#include "Ritual.h"

vector<tower> g_towers;
vector<unit> g_units;
list<potential_field> g_potential_fields;

potential_field* GetPotentialField(float t)
{
	for (auto &pf : g_potential_fields)
	{
		if (t < pf.t1)
			return &pf;
	}
	__debugbreak();
	return nullptr;
}

// Path vector of u is valid but incomplete.
// Continue from last node to the target position.
void FindRemainingPath(unit &u)
{
	float *speedfield = nullptr;
	while (true)
	{
		const position &initial = u.path.back();
		float t = initial.t;
		int x = initial.x;
		int y = initial.y;
		potential_field *pf = GetPotentialField(t);
		int w = pf->w;
		while (t < pf->t1)
		{
			int cell = x + y * w;
			int nextcell = pf->next[cell];
			if (nextcell == cell)
				return;
			int x2 = nextcell % w;
			int y2 = nextcell / w;
			float dist = 1;
			if ((x2 - x != 0) && (y2 - y != 0))
				dist = 1.4f;
			float t2 = t + speedfield[cell] * dist;
			u.path.push_back({ t2, x2, y2 });
			x = x2;
			y = y2;
			t = t2;
		}
	}
}

void CreateUnit(position p)
{
	unit u = {};
	u.spawntime = p.t;
	u.deathtime = inf;
	u.hp.push_back({ p.t, 100, nullptr });
	u.path.push_back(p);
	FindRemainingPath(u);
}

void SpawnTimeInterval(float t0, float t1)
{
	// Temp: create one unit per second.
	for (float t = ceil(t0); t < t1; t += 1.0f)
	{
		position p = { t, 100, 100 };
		CreateUnit(p);
	}
}

void TestSim()
{
	CreateUnit({ 1, 10, 10 });
}
