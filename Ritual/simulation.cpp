#include "Ritual.h"

vector<tower> g_towers;
vector<unit> g_units;
float g_simulation_time = 0; // Kinda inclusive/exclusive, be cautious

static struct {
	float period;
	float range2;
	float damage;
} tower_types[] = {
	{ 2.0f, 9.0f, 5.0f },
};

static bool health_finder(const health &h, float t)
{
	return h.t < t;
}

static bool shot_finder(const shot &s, float t)
{
	return s.t < t;
}

// TODO: Unit types with corresponding health and speed
void CreateUnit(position p)
{
	unit u = {};
	u.alive = span(p.t);
	u.hp.push_back({ p.t, 100 });
	u.path.push_back(p);
	RecomputePath(u, 0);
}

// TODO: Implement this one!
void SpawnTimeInterval(float t0, float t1)
{
	// Temp: create one unit per 10 seconds.
	for (float t = ceil(t0); t < t1; t += 10.0f)
	{
		position p = { t, 20, 20 };
		CreateUnit(p);
	}
}

// TODO: Do we need anything here?
// TODO: Remove test unit
void InitSim()
{
	CreateUnit({ 1, 10, 10 });
}

void TruncateHealth(unit &u, float t)
{
	if (!u.alive(t)) return;
	auto rm = lower_bound(u.hp.begin(), u.hp.end(), t, health_finder);
	u.hp.resize(rm - u.hp.begin());
	u.alive.t1 = inf;
}

void TruncateShots(tower &u, float t)
{
	if (u.alive.t1 <= t) return;
	auto rm = lower_bound(u.shots.begin(), u.shots.end(), t, shot_finder);
	u.shots.resize(rm - u.shots.begin());
}

void UnsimulateFrom(float t)
{
	for (auto &u : g_units)
		TruncateHealth(u, t);
	for (auto &u : g_towers)
		TruncateShots(u, t);
	if (g_simulation_time > t)
		g_simulation_time = t;
}

void Damage(float t, int ui, float amount)
{
	unit &u = g_units[ui];
	auto &last = u.hp.back();
	u.hp.push_back({ t, last.hp - amount });
	if (u.hp.back().hp <= 0)
		u.alive.t1 = t;
}

// TODO: implement this
float FindClosest(float t, int x, int y, int *ui)
{
	return inf;
}

// TODO: End condition
void SimulateUntil(float tend)
{
	// Create new units, will also compute their full path
	SpawnTimeInterval(g_simulation_time, tend);

	// Create priority queue sorted by next shot time
	struct node {
		float t;
		tower *tower;
		bool operator<(const node &rhs) const { return t < rhs.t; }
	};
	priority_queue<node> Q;
	// Add each tower to queue
	for (auto &u : g_towers)
	{
		float t = u.alive.t0;
		if (!u.shots.empty())
			t = u.shots.back().t;
		t += tower_types[u.type].period;
		Q.push({ t, &u });
	}
	// Dequeue one shot at a time until we've simulated enough
	while (g_simulation_time < tend)
	{
		node n = Q.top();
		Q.pop();
		g_simulation_time = n.t;
		auto &tt = tower_types[n.tower->type];
		int target = -1;
		float d2 = FindClosest(n.t, n.tower->x, n.tower->y, &target);
		if ((target == -1) || (d2 > tt.range2))
		{
			n.t += tower_types[n.tower->type].period;
			Q.push(n);
			continue;
		}
		n.tower->shots.push_back({ n.t, target });
		Damage(n.t, target, tt.damage);
	}
}

// TODO: Remove future overlapping towers instead of bail
bool BuildTower(float t, int x, int y, int type)
{
	// Loop over towers, don't double-build
	for (auto &u : g_towers)
	{
		if (u.alive.t1 > t)
			return false;
	}
	// Loop over units, make sure we don't stom anyone
	for (auto &u : g_units)
	{
		auto pt = GetPositionTransition(u, t);
		if ((pt.x0 == x) && (pt.y0 == y)) return false;
		if ((pt.x1 == x) && (pt.y1 == y)) return false;
	}
	// Try to compute an updated potential field. Will complain if we do something bad
	if (!CreatePotentialField(t))
		return false;

	tower tw;
	tw.alive = span(t);
	tw.x = x;
	tw.y = y;
	tw.type = type;
	g_towers.push_back(tw);
	// For each unit, clear path from t and recompute it
	for (auto &u : g_units)
		RecomputePath(u, t);

	UnsimulateFrom(t);
	return true;
}

// TODO: Proper sprite index based on rotation & animation
// TODO: Deal with after-game-over times
// TODO: Projectiles!
void GetDrawables(float t, vector<drawable> &stuff)
{
	if (t >= g_simulation_time)
		SimulateUntil(t + 10);
	for (auto &u : g_units)
	{
		if (u.alive(t))
		{
			position_transition pt = GetPositionTransition(u, t);
			drawable d;
			d.sprite = 80 + u.type;
			d.x = pt.x0 + (pt.x1 - pt.x0) * pt.lerp;
			d.y = pt.y0 + (pt.y1 - pt.y0) * pt.lerp;
			stuff.push_back(d);
		}
	}
	for (auto &u : g_towers)
	{
		if (u.alive(t))
		{
			drawable d;
			d.sprite = 50 + u.type;
			d.x = (float)u.x;
			d.y = (float)u.y;
			stuff.push_back(d);
		}
	}
}

