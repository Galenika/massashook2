#pragma once
#include <iostream>
#include "options.hpp"
#include "valve_sdk/sdk.hpp"
#include "hooks.hpp"
#include "features/visuals.hpp"
#include "helpers/Config.h"
#include "features/Ragebot.h"
#include "Resolver.h"
#pragma comment(lib, "winmm.lib")

void ConColorMsg(Color const &color, const char* buf, ...);
void ConMsg(const char* msg, ...);

class C_EventListener : public IGameEventListener2, public Singleton<C_EventListener>
{
public:
	C_EventListener( )
	{
		g_GameEvents->AddListener( this , "player_hurt" , false );
		g_GameEvents->AddListener( this , "player_death" , false );
		g_GameEvents->AddListener( this , "item_purchase" , false );
		g_GameEvents->AddListener( this , "round_freeze_end" , false );
		g_GameEvents->AddListener( this , "round_end" , false );
		g_GameEvents->AddListener( this , "weapon_fire" , false );
		g_GameEvents->AddListener( this , "item_equip" , false );
	}
	~C_EventListener()
	{
		g_GameEvents->RemoveListener(this);
	}

	virtual void FireGameEvent(IGameEvent* event)
	{
		if (!g_GameEvents)
			return;

		int local_id = g_EngineClient->GetLocalPlayer();
		if (!local_id) return;

		C_BasePlayer* local = (C_BasePlayer*)g_EntityList->GetClientEntity(local_id);
		if (!local) return;

		player_info_t local_info; 
		if (!g_EngineClient->GetPlayerInfo(local->EntIndex(), &local_info))
			return;

		C_Resolver::Get().on_events(event);

		static ConVar* right_hand = nullptr;

		if ( !strcmp( event->GetName( ) , "item_equip" ) )
		{
			int uid = event->GetInt( "userid" );
			int uid_index = g_EngineClient->GetPlayerForUserID( uid );
			short weptype = event->GetInt( "weptype" );

			if ( g_Options.misc_knife_switch && uid_index == g_LocalPlayer->EntIndex( ) )
			{
				static bool was_knife = false;
				right_hand = g_CVar->FindVar( "cl_righthand" );

				if ( weptype == WEAPONTYPE_KNIFE )
				{
					right_hand->SetValue( !right_hand->GetBool( ) );
					was_knife = true;
				}
				else
				{
					if ( was_knife )
					{
						right_hand->SetValue( !right_hand->GetBool( ) );
						was_knife = false;
					}
				}

			}
		}

		if (!(strcmp(event->GetName(), "player_hurt")))
		{
			int attacker_id = event->GetInt("attacker");
			int victim_id = event->GetInt("userid");
			int damage = event->GetInt("dmg_health");

			int attacker_index = g_EngineClient->GetPlayerForUserID(attacker_id);
			int victim_index = g_EngineClient->GetPlayerForUserID(victim_id);


			player_info_t victim_info;
			g_EngineClient->GetPlayerInfo(victim_index, &victim_info);

			if (attacker_index == local->EntIndex() && g_Options.misc_hitmark)
			{
				if (attacker_id == victim_id) return;
				g_Options.Hitmark_alpha = 1.f;
			}

			if (g_Options.misc_event_log)
			{
				if (attacker_index == local->EntIndex())
				{
					std::string buf = std::string("[MassasHook] -" + std::to_string(damage) + " to " + victim_info.szName + "\n");
					ConColorMsg(Color(250, 250, 250, 255), buf.c_str());
				}
			}

		}


		if (!(strcmp(event->GetName(), "round_freeze_end")))
		{
			g_Options.CanAutoPistol = true;
			C_Ragebot::Get( ).can_aa = true;
		}

		if (!(strcmp(event->GetName(), "round_end")))
		{
			g_Options.CanAutoPistol = false;
			C_Ragebot::Get( ).can_aa = false;
		}

		if (!(strcmp(event->GetName(), "item_purchase")))
		{
			if (g_Options.misc_event_log)
			{

				if (!local->IsAlive()) return;

				auto buyer = event->GetInt("userid");
				std::string gun = event->GetString("weapon");

				if (strstr(gun.c_str(), "molotov")
					|| strstr(gun.c_str(), "nade")
					|| strstr(gun.c_str(), "kevlar")
					|| strstr(gun.c_str(), "decoy")
					|| strstr(gun.c_str(), "suit")
					|| strstr(gun.c_str(), "flash")
					|| strstr(gun.c_str(), "vest")
					|| strstr(gun.c_str(), "cutter")
					|| strstr(gun.c_str(), "defuse")
					)  return;

				auto player_index = g_EngineClient->GetPlayerForUserID(buyer);
				C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(player_index);

				player_info_t pinfo;

				if (player && local && g_EngineClient->GetPlayerInfo(player_index, &pinfo))
				{
					gun.erase(gun.find("weapon_"), 7);
					if (local->m_iTeamNum() != player->m_iTeamNum())
					{
						std::string buf = std::string("[MassasHook] " + std::string(pinfo.szName) + " -> " + gun + std::string("\n"));
						ConColorMsg(Color(40, 120, 250), buf.c_str());
					}
				}
			}
		}

		if (!(strcmp(event->GetName(), "player_death")))
		{
			int victim_id = event->GetInt("userid");
			int attacker_id = event->GetInt("attacker");
			auto weapon = event->GetString("weapon");

			int victim = g_EngineClient->GetPlayerForUserID(victim_id);
			int attacker = g_EngineClient->GetPlayerForUserID(attacker_id);

			player_info_t victom_info;
			player_info_t attacker_info;

			if (!g_EngineClient->GetPlayerInfo(victim, &victom_info)) return;
			if (!g_EngineClient->GetPlayerInfo(attacker, &attacker_info)) return;

			std::string buf = std::string(std::string("[MassasHook] ") + attacker_info.szName + std::string(" killed ") + victom_info.szName + "\n");
			g_CVar->ConsoleDPrintf(buf.c_str());


			if (g_Options.Players[victim].bShittalk)
			{
				if (attacker == g_LocalPlayer->EntIndex())
					g_EngineClient->ClientCmd("say so fucking easy...");
				else
					g_EngineClient->ClientCmd("say lmao you're so fucking bad");
			}

			if (g_Options.Players[attacker].bShittalk)
			{
				if (victim == g_LocalPlayer->EntIndex())
					g_EngineClient->ClientCmd("say Only with noob weapon...");
			}

		}
	}

	int GetEventDebugID() override
	{
		return 42;
	}
};

