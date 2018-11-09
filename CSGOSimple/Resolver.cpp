#include "Resolver.h"
#include "hooks.hpp"
#include "features/Ragebot.h"
#include "options.hpp"

ResolverData_t resolve_data[64];

bool C_Resolver::is_moving(C_BasePlayer* player)
{
	return player->m_vecVelocity().Length() >= 70;
}

bool C_Resolver::is_onground(C_BasePlayer* player)
{
	return player->m_fFlags() & FL_ONGROUND;
}

bool C_Resolver::is_breaking_lby(C_BasePlayer* player)
{
	return player->m_flLowerBodyYawTarget() - player->m_angEyeAngles().yaw > 35;
}

void C_Resolver::on_events(IGameEvent* event)
{
	if (!(strcmp(event->GetName(), "weapon_fire")))
	{
		int uid = event->GetInt("userid");

		int uid_index = g_EngineClient->GetPlayerForUserID(uid);

		if (uid_index == g_LocalPlayer->EntIndex() && g_Options.rage_aimbot_resolver)
			resolve_data[C_Ragebot::Get().Get_Target()].missed_shots++;
	}

	if (!(strcmp(event->GetName(), "player_hurt")))
	{
		int victim_id = event->GetInt("userid");

		int victim_index = g_EngineClient->GetPlayerForUserID(victim_id);

		C_BasePlayer* ent = (C_BasePlayer*)g_EntityList->GetClientEntity(victim_index);

		if (g_Options.rage_aimbot_resolver)
		{
			resolve_data[victim_index].last_hit_angle = ent->m_angEyeAngles().yaw - ent->m_flLowerBodyYawTarget();
			resolve_data[victim_index].missed_shots--;
		}
	}
}

void C_Resolver::resolve( C_BasePlayer* player )
{

	int index = player->EntIndex( );



	if ( resolve_data[index].missed_shots <= -1 )
		resolve_data[index].missed_shots = 0;

	resolve_data[index].mode = "";

	if ( is_moving( player ) && is_onground( player ) )
	{
		player->m_angEyeAngles( ).yaw = player->m_flLowerBodyYawTarget( );
		resolve_data[index].last_moving_lby = player->m_flLowerBodyYawTarget( );
		resolve_data[index].mode = "moving target";
		resolve_data[index].missed_shots = 0;
	}
	else
	{
		if ( !is_onground( player ) )
		{
			resolve_data[index].mode = "in air";

			switch ( resolve_data[index].missed_shots % 3 )
			{
			case 0:
				resolve_data[index].mode = "last moving lby";
				player->m_angEyeAngles( ).yaw = resolve_data[index].last_moving_lby ? resolve_data[index].last_moving_lby : player->m_flLowerBodyYawTarget( );
				break;
			case 1:
				resolve_data[index].mode = "last hit angle";
				player->m_angEyeAngles( ).yaw = player->m_flLowerBodyYawTarget( ) + resolve_data[index].last_hit_angle ? resolve_data[index].last_hit_angle : 90;
				break;
			case 2:
				player->m_angEyeAngles( ).yaw = player->m_flLowerBodyYawTarget( ) - 180.f;
				break;
			case 3:
				player->m_angEyeAngles( ).yaw = player->m_flLowerBodyYawTarget( ) - 90.f;
				break;
			}

		}
		else
		{
			switch ( resolve_data[index].missed_shots % 3 )
			{
			case 0:
				resolve_data[index].mode = "last moving lby";
				player->m_angEyeAngles( ).yaw = resolve_data[index].last_moving_lby ? resolve_data[index].last_moving_lby : player->m_flLowerBodyYawTarget( );
				break;
			case 1:
				resolve_data[index].mode = "last hit angle";
				player->m_angEyeAngles( ).yaw = player->m_flLowerBodyYawTarget( ) + resolve_data[index].last_hit_angle ? resolve_data[index].last_hit_angle : 90;
				break;
			case 2:
				player->m_angEyeAngles( ).yaw = player->m_flLowerBodyYawTarget( ) - 180.f;
				break;
			case 3:
				player->m_angEyeAngles( ).yaw = player->m_flLowerBodyYawTarget( ) - 90.f;
				break;
			}
		}
	}
}
