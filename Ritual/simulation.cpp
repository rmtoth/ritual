#include "Ritual.h"
#include "World.h"

vector<tower> g_towers;
vector<unit> g_units;
list<potential_field> g_potential_fields;

bool position_finder(const position &p, float t)
{
	return p.t <= t;
}

position_transition GetPositionTransition(unit &u, float t)
{
	auto lo = lower_bound(u.path.begin(), u.path.end(), t, position_finder);
	auto hi = lo + 1;
	position_transition pt = { lo->x, lo->y, hi->x, hi->y };
	pt.lerp = (t - lo->t) / (hi->t - lo->t);
	return pt;
}

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
	int initial_cell = g_world->mDest.x + g_world->mDest.y * pf.w;
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

	// Validate that we didn't trap any units
	for (auto &u : g_units)
	{
		if (!u.alive(t))
			continue;
		position_transition pt = GetPositionTransition(u, t);
		int cell = pt.x0 + pt.y0 * pf.w;
		if (next[cell] == -1)
			return false;
		cell = pt.x1 + pt.y1 * pf.w;
		if (next[cell] == -1 && cell != initial_cell)
			return false;
	}

	// TODO: Validate if spawn points are not trapped

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

void InitSim()
{
	g_potential_fields.resize(1);
	auto &pf = *g_potential_fields.begin();
	pf.alive = span();
	ComputePotentialField(0, pf);

	CreateUnit({ 1, 10, 10 });
}

void GetDrawables(float t, vector<drawable> &stuff)
{
	
}
