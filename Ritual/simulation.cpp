#include "Ritual.h"
#include "World.h"

vector<tower> g_towers;
vector<unit> g_units;
list<potential_field> g_potential_fields;

static const struct {
	int dx, dy;
	float dist;
} FieldNeighbors[8] = {
	{ -1, 0, 1.0f },
	{ +1, 0, 1.0f },
	{ 0, -1, 1.0f },
	{ 0, +1, 1.0f },
	{ -1, -1, 1.4f },
	{ -1, +1, 1.4f },
	{ +1, -1, 1.4f },
	{ +1, +1, 1.4f },
};

bool ComputePotentialField(float t, potential_field &pf)
{
	pf.w = g_world->mWidth;
	pf.h = g_world->mHeight;
	pf.next.resize(0);
	pf.next.resize(pf.w * pf.h, -1);
	vector<bool> blocked(pf.w*pf.h, false);
	for (auto &tower : g_towers)
	{
		if (tower.alive(t))
		{
			int cell = tower.x + tower.y * pf.w;
			blocked[cell] = true;
		}
	}
	int *next = &pf.next[0];
	struct node {
		float cost;
		int cell;
		int from;
		bool operator<(const node &rhs) const {
			return rhs.cost < cost; // deliberately flipped
		}
	};
	priority_queue<node> Q;
	int initial_cell = 500; // TODO
	Q.push({ 0, initial_cell, -1 });
	unordered_set<int> visited;
	while (!Q.empty())
	{
		node n = Q.top();
		Q.pop();
		if (!visited.insert(n.cell).second)
			continue;
		next[n.cell] = n.from;
		for (auto &fn : FieldNeighbors)
		{
			int nextcell = n.cell + fn.dx + fn.dy * pf.w;
			if (visited.find(nextcell) != visited.end())
				continue;
			Q.push({ n.cost + fn.dist, nextcell, n.cell });
		}
	}

	// TODO: Validate if this was OK

	return true;
}

potential_field* GetPotentialField(float t)
{
	for (auto &pf : g_potential_fields)
	{
		if (t < pf.alive.t1)
			return &pf;
	}
	__debugbreak();
	return nullptr;
}

// Path vector of u is valid but incomplete.
// Continue from last node to the target position.
void FindRemainingPath(unit &u)
{
	while (true)
	{
		const position &initial = u.path.back();
		float t = initial.t;
		int x = initial.x;
		int y = initial.y;
		potential_field *pf = GetPotentialField(t);
		int w = pf->w;
		int *next = &pf->next[0];
		while (t < pf->alive.t1)
		{
			int cell = x + y * w;
			int nextcell = next[cell];
			if (nextcell == -1)
				return;
			int x2 = nextcell % w;
			int y2 = nextcell / w;
			float dist = 1;
			if ((x2 - x != 0) && (y2 - y != 0))
				dist = 1.4f;
			float t2 = t + g_world->mWalkCost[cell] * dist;
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
	u.alive = span(p.t);
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
