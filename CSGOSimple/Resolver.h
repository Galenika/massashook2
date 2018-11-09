#pragma once
#include "valve_sdk/sdk.hpp"

struct ResolverData_t
{
	int missed_shots = 0;

	float last_moving_lby = 0;
	float last_hit_angle = 0;
	float last_not_braking_lby = 0;
	std::string mode;
};

class C_Resolver : public Singleton<C_Resolver>
{
public:
	void on_events(IGameEvent* event);
	void resolve ( C_BasePlayer* player );
private:
	bool is_moving(C_BasePlayer* player);
	bool is_onground(C_BasePlayer* player);
	bool is_breaking_lby(C_BasePlayer* player);
};

extern ResolverData_t resolve_data[64];