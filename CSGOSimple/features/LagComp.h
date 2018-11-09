#pragma once
#include "../valve_sdk/sdk.hpp"

struct backtrackData
{
	float simtime;
	Vector hitboxPos;
};

struct player_bt {

	matrix3x4_t bone[128];

};

class C_LagComp : public Singleton<C_LagComp>
{
public:
	void LegitBacktrack(CUserCmd* cmd);
private:

};

extern player_bt bt_data[64][12];
extern backtrackData headPositions[64][12];