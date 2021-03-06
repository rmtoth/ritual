#include "Ritual.h"
#include "World.h"
#include "Scrub.h"
#include <random>

vector<tower> g_towers;
vector<unit> g_units;
vector<particle> g_preparticles;
vector<particle> g_postparticles;
float g_simulation_time = 0; // Kinda inclusive/exclusive, be cautious
float g_game_over_time = inf;

static bool health_finder(const health &h, float t)
{
	return h.t < t;
}

static bool shot_finder(const shot &s, float t)
{
	return s.t < t;
}

static bool preparticle_finder(const particle &s, float t)
{
	return s.alive.t1 < t;
}

static bool postparticle_finder(const particle &s, float t)
{
	return s.alive.t0 < t;
}

void InterpolateParticle(const particle &p, float t, float &x, float &y, int *idx)
{
	float s = (t - p.alive.t0) / (p.alive.t1 - p.alive.t0);
	x = p.x0 + s * (p.x1 - p.x0);
	y = p.y0 + s * (p.y1 - p.y0);
	if (idx)
		*idx = int(s * 5.99f);
}

// TODO: Unit types with corresponding health and speed
void CreateUnit(position p, int type)
{
	g_units.push_back({});
	unit &u = g_units.back();
	u.alive = span(p.t);
	u.type = type;
	u.hp.push_back({ p.t, unit_types[type].health });
	u.path.push_back(p);
	RecomputePath(u, 0);
}

// TODO: Implement this one!
void SpawnTimeInterval(float t0, float t1)
{
	// Temp: create one unit per 10 seconds.
	//for (float tt = ceil(t0/10+.5); tt < t1/10; tt += 1)
	//{
	//	float t = tt * 10 + 5;
	//	position p = { t, 20, 20 };
	//	CreateUnit(p);
	//}
}

// TODO: Do we need anything here?
// TODO: Remove test unit
void InitSim()
{
	srand(0xBABEFACE);

	float incr[3] = { 1.35f, 1.2f, 1.55f };
	float nUnits[3] = { 1.0f, 3.0f, 3.5f };

	int nWaves = 10;

	for (int wave = 0; wave < nWaves; wave++) {
		float t0 = 1.0f + 0.75f * (wave / float(nWaves)) * g_scrub->mTotalTime;
		float t1 = 1.0f + 0.75f * ((wave + 1) / float(nWaves)) * g_scrub->mTotalTime;
		t1 = t0 + (t1 - t0) * 0.1f;

		for (int a = 0; a < 3; a++) {

			for (auto it : g_world->mSpawn) {
				int u = int(1 + (nUnits[a] / g_world->mSpawn.size()));
				for (int k = 0; k < u; k++) {
					float t = t0 + (t1 - t0) * (float(rand()) / float(RAND_MAX));
					CreateUnit({ t, it.x, it.y }, a);
				}

			}
			nUnits[a] *= incr[a];
		}

	}

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

float GetHealth(unit &u, float t)
{
	auto hi = lower_bound(u.hp.begin(), u.hp.end(), t, health_finder);
	if (hi == u.hp.begin())
		return u.hp.front().hp;
	return (hi - 1)->hp;
}

void UnsimulateFrom(float t)
{
	for (auto &u : g_units)
		TruncateHealth(u, t);
	for (auto &u : g_towers)
		TruncateShots(u, t);
	{
		auto rm = lower_bound(g_preparticles.begin(), g_preparticles.end(), t, preparticle_finder);
		g_preparticles.resize(rm - g_preparticles.begin());
		rm = lower_bound(g_postparticles.begin(), g_postparticles.end(), t, postparticle_finder);
		g_postparticles.resize(rm - g_postparticles.begin());
	}
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

void Interpolate(position_transition &pt, float &x, float &y)
{
	x = pt.x0 + (pt.x1 - pt.x0) * pt.lerp;
	y = pt.y0 + (pt.y1 - pt.y0) * pt.lerp;
}

float Distance2(float t, unit &u, float x0, float y0)
{
	float ux, uy;
	auto pt = GetPositionTransition(u, t);
	Interpolate(pt, ux, uy);
	float dx = ux - x0;
	float dy = uy - y0;
	return dx * dx + dy * dy;
}

float FindClosest(float t, int x, int y, int *ui)
{
	float d2best = inf;
	int ubest = -1;
	float x0 = float(x);
	float y0 = float(y);
	for (auto &u : g_units)
	{
		if (!u.alive(t))
			continue;
		float d2 = Distance2(t, u, x0, y0);
		if (d2best > d2)
		{
			d2best = d2;
			ubest = int(&u - &g_units[0]);
		}
	}
	*ui = ubest;
	return d2best;
}

void ComputeDieTime()
{
	g_game_over_time = inf;
	for (auto &u : g_units)
	{
		float tdie = u.path.back().t;
		if (u.alive(tdie))
		{
			if (tdie < g_game_over_time)
				g_game_over_time = tdie;
		}
	}
}

// TODO: End condition
void SimulateUntil(float tend)
{
	if (tend <= g_simulation_time)
		return;

	// Create new units, will also compute their full path
	SpawnTimeInterval(g_simulation_time, tend);

	if (g_towers.empty())
	{
		g_simulation_time = tend;
		ComputeDieTime();
		return;
	}

	// Create priority queue sorted by next shot time
	struct node {
		float t;
		tower *tower;
		bool operator<(const node &rhs) const { return t > rhs.t; } // deliberate!
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
		int ty = n.tower->type;
		auto &tt = tower_types[ty];
		int target = -1;
		float d2 = FindClosest(n.t, n.tower->x, n.tower->y, &target);
		if ((target != -1) && (d2 <= tt.range2))
		{
			n.tower->shots.push_back({ n.t, target });
			auto pt = GetPositionTransition(g_units[target], n.t);
			float x, y;
			Interpolate(pt, x, y);
			if (ty == 3)
			{
				for (auto &u : g_units)
				{
					if (u.alive(n.t))
					{
						float dmg = 1 - Distance2(n.t, u, x, y)/4;
						if (dmg > 0)
							Damage(n.t, int(&u - &g_units[0]), tt.damage * dmg);
					}
				}
				particle part;
				part.alive = span(n.t, n.t + .3f);
				part.x0 = x;
				part.y0 = y;
				part.type = 100;
				part.x1 = x;
				part.y1 = y;
				g_postparticles.push_back(part);
			}
			else
			{
				Damage(n.t, target, tt.damage);
			}
			particle part;
			part.alive = span(n.t - .1f, n.t);
			part.x0 = float(n.tower->x);
			part.y0 = float(n.tower->y);
			part.x1 = x;
			part.y1 = y;
			part.type = 90 + ty;
			g_preparticles.push_back(part);
		}
		n.t += tower_types[n.tower->type].period;
		Q.push(n);
	}
	ComputeDieTime();
}

float GetGameOverTime()
{
	SimulateUntil(g_scrub->mTotalTime);
	return g_game_over_time;
}

// TODO: Remove future overlapping towers instead of bail
bool BuildTower(float t, int x, int y, int type)
{
	if (t < g_scrub->mSpentTime)
	{
		ShowErrorMessage(errNotEnoughTime);
		return false;
	}

	// Loop over towers, don't double-build
	for (auto &u : g_towers)
	{
		if ((u.x == x) && (u.y == y) && (u.alive.t1 > t))
		{
			ShowErrorMessage(errFutureTowerBlocking);
			return false;
		}
	}
	// Loop over units, make sure we don't stom anyone
	for (auto &u : g_units)
	{
		if (!u.alive(t))
			continue;
		auto pt = GetPositionTransition(u, t);
		if (((pt.x0 == x) && (pt.y0 == y)) || ((pt.x1 == x) && (pt.y1 == y)))
		{
			ShowErrorMessage(errDemonBlocking);
			return false;
		}
	}

	// We must create tower before CreatePotentialField for it to be blocking
	tower tw;
	tw.alive = span(t);
	tw.x = x;
	tw.y = y;
	tw.type = type;
	g_towers.push_back(tw);

	// Try to compute an updated potential field. Will complain if we do something bad
	if (!CreatePotentialField(t))
	{
		g_towers.pop_back();
		RepairPotentialField(t);
		return false;
	}

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
	SimulateUntil(g_scrub->mTotalTime);
	for (auto &u : g_units)
	{
		if (u.alive(t))
		{
			position_transition pt = GetPositionTransition(u, t);
			drawable d;
			d.sprite = 80 + u.type;
			Interpolate(pt, d.x, d.y);
			float hp = GetHealth(u, t);
			d.health = hp / unit_types[u.type].health;
			int dx = pt.x1 - pt.x0;
			int dy = pt.y1 - pt.y0;
			static const int dirtable[3][3] = {
				5, 6, 7,
				4, 0, 0,
				3, 2, 1,
			};
			int dir = dirtable[dx + 1][dy + 1];
			int frame = int(unit_types[u.type].animspeed * (t - u.alive.t0)) % 4;
			static const int frametable[4] = { 0, 8, 0, 16 };
			d.variation = frametable[frame] + dir;
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
			d.health = 0.0f;
			stuff.push_back(d);
		}
	}
	{
		auto it = lower_bound(g_preparticles.begin(), g_preparticles.end(), t, preparticle_finder);
		while (it != g_preparticles.end())
		{
			if (!it->alive(t))
				break;
			drawable d;
			InterpolateParticle(*it, t, d.x, d.y, nullptr);
			d.sprite = it->type;
			d.health = 0;
			stuff.push_back(d);
			++it;
		}
	}
	{
		auto it = lower_bound(g_postparticles.begin(), g_postparticles.end(), t, preparticle_finder);
		while (it != g_postparticles.end())
		{
			if (!it->alive(t))
				break;
			drawable d;
			int idx;
			InterpolateParticle(*it, t, d.x, d.y, &idx);
			d.sprite = it->type + idx;
			d.health = 0;
			stuff.push_back(d);
			++it;
		}
	}
}

void PlaySimulationSounds(float t0, float t1)
{
	{
		auto it = lower_bound(g_preparticles.begin(), g_preparticles.end(), t0, postparticle_finder);
		// it->t0 > t0.
		while (it != g_preparticles.end())
		{
			// it->t0 < t1 ?
			if (it->alive.t0 >= t1)
				break;
			PlaySimSound(it->type);
			++it;
		}
	}
	{
		auto it = lower_bound(g_postparticles.begin(), g_postparticles.end(), t0, postparticle_finder);
		// it->t0 > t0.
		while (it != g_postparticles.end())
		{
			// it->t0 < t1 ?
			if (it->alive.t0 >= t1)
				break;
			PlaySimSound(it->type);
			++it;
		}
	}
	for (auto &u : g_units)
	{
		if ((u.alive.t1 >= t0) && (u.alive.t1 < t1))
			PlaySimSound(u.type);
	}
}

