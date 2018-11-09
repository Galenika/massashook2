#include "LagComp.h"
#include "../options.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../hooks.hpp"
#include "../helpers/math.hpp"

backtrackData headPositions[64][12];
player_bt bt_data[64][12];
#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

inline float distance_point_to_line(Vector Point, Vector LineOrigin, Vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

inline Vector angle_vector(QAngle meme)
{
	auto sy = sin(meme.yaw / 180.f * static_cast<float>(3.14159265358979323846f));
	auto cy = cos(meme.yaw / 180.f * static_cast<float>(3.14159265358979323846f));

	auto sp = sin(meme.pitch / 180.f * static_cast<float>(3.14159265358979323846f));
	auto cp = cos(meme.pitch / 180.f* static_cast<float>(3.14159265358979323846f));

	return Vector(cp*cy, cp*sy, -sp);
}

void C_LagComp::LegitBacktrack(CUserCmd* cmd) 
{
	int bestTargetIndex = -1;
	float bestFov = FLT_MAX;
	player_info_t info;
	
	auto local_id = g_EngineClient->GetLocalPlayer();
	if (!local_id) return;

	C_BasePlayer* local = (C_BasePlayer*)g_EntityList->GetClientEntity(local_id);
	if (!local) return;

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!entity) continue;
		if (entity == local) continue;
		if (!g_EngineClient->GetPlayerInfo(i, &info)) continue;
		if (entity->IsDormant()) continue;
		if (entity->m_iTeamNum() == local->m_iTeamNum()) continue;
		if (!entity->IsAlive()) continue;

		float simtime = entity->m_flSimulationTime();
		Vector hitboxPos = entity->GetHitboxPos( 0 );
		matrix3x4_t bones[128];
		entity->SetupBones( bones, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, g_GlobalVars->curtime );

		for ( int idx = 0; idx <= 128; ++idx )
			bt_data[i][cmd->command_number % 13].bone[idx] = bones[idx];

		headPositions[i][cmd->command_number % 13] = backtrackData{ simtime, hitboxPos };
		Vector ViewDir = angle_vector(cmd->viewangles + (local->m_aimPunchAngle() * 2.f));
		float FOVDistance = distance_point_to_line(hitboxPos, local->GetEyePos(), ViewDir);

		if (bestFov > FOVDistance)
		{
			bestFov = FOVDistance;
			bestTargetIndex = i;
		}
	}

	float bestTargetSimTime = 0;
	if (bestTargetIndex != -1)
	{
		float tempFloat = FLT_MAX;
		Vector ViewDir = angle_vector(cmd->viewangles + (local->m_aimPunchAngle() * 2.f));
		for (int t = 0; t < g_Options.legit_backtrack; ++t)
		{
			float tempFOVDistance = distance_point_to_line(headPositions[bestTargetIndex][t].hitboxPos, local->GetEyePos(), ViewDir);
			if (tempFloat > tempFOVDistance && headPositions[bestTargetIndex][t].simtime > local->m_flSimulationTime() - 1)
			{
				tempFloat = tempFOVDistance;
				bestTargetSimTime = headPositions[bestTargetIndex][t].simtime;
			}
		}
	}


	cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);
}
