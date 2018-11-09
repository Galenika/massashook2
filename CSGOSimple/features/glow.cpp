#include "glow.hpp"

#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"

Glow::Glow()
{
}

Glow::~Glow()
{
    // We cannot call shutdown here unfortunately.
    // Reason is not very straightforward but anyways:
    // - This destructor will be called when the dll unloads
    //   but it cannot distinguish between manual unload 
    //   (pressing the Unload button or calling FreeLibrary)
    //   or unload due to game exit.
    //   What that means is that this destructor will be called
    //   when the game exits.
    // - When the game is exiting, other dlls might already 
    //   have been unloaded before us, so it is not safe to 
    //   access intermodular variables or functions.
    //   
    //   Trying to call Shutdown here will crash CSGO when it is
    //   exiting (because we try to access g_GlowObjManager).
    //
}

void Glow::Shutdown()
{
    // Remove glow from all entities
    for(auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++) {
        auto& glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
        auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

        if(glowObject.IsUnused())
            continue;

        if(!entity || entity->IsDormant())
            continue;

        glowObject.m_flAlpha = 0.0f;
    }
}

void Glow::Run()
{
    for(auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++) {
        auto& glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
        auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

        if(glowObject.IsUnused())
            continue;

        if(!entity || entity->IsDormant())
            continue;

        auto class_id = entity->GetClientClass()->m_ClassID;
        auto color = Color{};

        switch(class_id) {
            case ClassId_CCSPlayer:
            {
                auto is_enemy = entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();

                if(!g_Options.glow_enabled || !entity->IsAlive())
                    continue;

                if(!is_enemy && g_Options.esp_enemies_only)
                    continue;

				if (g_Options.esp_visible_only && !g_LocalPlayer->CanSeePlayer(entity, HITBOX_CHEST))
					continue;

				if (g_Options.esp_dead_only && g_LocalPlayer->IsAlive())
					continue;
	
                color = is_enemy ? g_Options.color_glow_enemy : g_Options.color_glow_ally;

				if (!g_Options.Players[entity->EntIndex()].is_forced_esp && g_Options.esp_playerlist)
					continue;

				if (g_Options.Players[entity->EntIndex()].is_friendly)
					color = Color(30, 255, 100);

                break;
            }
            case ClassId_CBaseAnimating:
                if(!g_Options.esp_defuse_kit)
                    continue;
                color = g_Options.color_esp_defuse;
                break;
            case ClassId_CPlantedC4:
                if(!g_Options.esp_planted_c4)
                    continue;
                color = g_Options.color_esp_c4;
                break;
            default:
            {
                if(entity->IsWeapon()) {
                    if(!g_Options.esp_dropped_weapons)
                        continue;
                    color = g_Options.color_esp_weapons;
                }
            }
        }

        glowObject.m_flRed = color.r() / 255.0f;
        glowObject.m_flGreen = color.g() / 255.0f;
        glowObject.m_flBlue = color.b() / 255.0f;
        glowObject.m_flAlpha = g_Options.glow_alpha;
        glowObject.m_bRenderWhenOccluded = true;
        glowObject.m_bRenderWhenUnoccluded = false;
    }
}
