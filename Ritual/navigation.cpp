#include "Ritual.h"
#include "World.h"

list<potential_field> g_potential_fields;

static bool position_finder(const position &p, float t)
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
	vector<int> blocked_arr(pf.w*pf.h, 0);
	int *blocked = &blocked_arr[0];
	for (int x = 0; x < pf.w; ++x)
		blocked[x] = blocked[x + (pf.h - 1) * pf.w] = 1;
	for (int y = 0; y < pf.h; ++y)
		blocked[y*pf.w] = blocked[(y + 1) * pf.w - 1] = 1;
	for (auto &tower : g_towers)
	{
		if (tower.alive(t))
		{
			int cell = tower.x + tower.y * pf.w;
			blocked[cell] = 1;
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
			if (blocked[nextcell])
				continue;
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

list<potential_field>::iterator GetPotentialField(float t)
{
	for (auto &it = g_potential_fields.begin(); it != g_potential_fields.end(); ++it)
	{
		if (t < it->alive.t1)
			return it;
	}
	__debugbreak();
	return g_potential_fields.end();
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
		potential_field &pf = *GetPotentialField(t);
		int w = pf.w;
		int *next = &pf.next[0];
		while (t < pf.alive.t1)
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

void RecomputePath(unit &u, float t)
{
	// Find first node after t
	auto lo = lower_bound(u.path.begin(), u.path.end(), t, position_finder);
	auto hi = lo + 1;
	// Remove excessive nodes, leaving spawn point alone
	if (lo != u.path.begin())
		u.path.resize(lo - u.path.begin());
	FindRemainingPath(u);
}

bool CreatePotentialField(float t)
{
	// Generate new potential field, make sure it doesn't complain
	potential_field pf;
	if (!ComputePotentialField(t, pf))
		return false;
	// Locate current potential field, split it at t
	auto it = GetPotentialField(t);
	pf.alive = span(t, it->alive.t1);
	it->alive.t1 = t;
	g_potential_fields.insert(++it, pf);
	return true;
}

void InitNav()
{
	g_potential_fields.resize(1);
	potential_field &pf = *g_potential_fields.begin();
	ComputePotentialField(0, pf);
}
