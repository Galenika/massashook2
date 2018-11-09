#include "chams.hpp"
#include <fstream>

#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "../hooks.hpp"
#include "../helpers/input.hpp"
#include "LagComp.h"

Chams::Chams()
{
    std::ofstream("csgo\\materials\\simple_regular.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
    std::ofstream("csgo\\materials\\simple_ignorez.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
    std::ofstream("csgo\\materials\\simple_flat.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
    std::ofstream("csgo\\materials\\simple_flat_ignorez.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

	std::ofstream( "csgo\\materials\\simple_metallic.vmt" ) << R"#("VertexLitGeneric" {
      "$basetexture" "vgui/white_additive"
      "$ignorez" "0"
      "$envmap" "env_cubemap"
      "$normalmapalphaenvmapmask" "1"
      "$envmapcontrast"  "1"
      "$nofog" "1"
      "$model" "1"
      "$nocull" "0"
      "$selfillum" "1"
      "$halflambert" "1"
      "$znearer" "0"
      "$flat" "1" 
}
)#";

    materialRegular = g_MatSystem->FindMaterial("simple_regular", TEXTURE_GROUP_MODEL);
    materialRegularIgnoreZ = g_MatSystem->FindMaterial("simple_ignorez", TEXTURE_GROUP_MODEL);
    materialFlatIgnoreZ = g_MatSystem->FindMaterial("simple_flat_ignorez", TEXTURE_GROUP_MODEL);
    materialFlat = g_MatSystem->FindMaterial("simple_flat", TEXTURE_GROUP_MODEL);
	materialMetalic = g_MatSystem->FindMaterial( "simple_metallic", TEXTURE_GROUP_MODEL );
}

Chams::~Chams()
{
    std::remove("csgo\\materials\\simple_regular.vmt");
    std::remove("csgo\\materials\\simple_ignorez.vmt");
    std::remove("csgo\\materials\\simple_flat.vmt");
    std::remove("csgo\\materials\\simple_flat_ignorez.vmt");
	std::remove( "csgo\\materials\\simple_metallic.vmt" );
}

void Chams::OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, const Color& rgba, bool transparent)
{
    IMaterial* material = nullptr;

    if(flat) {
        if(ignoreZ)
            material = materialFlatIgnoreZ;
        else
            material = materialFlat;
    } else {
        if(ignoreZ)
            material = materialRegularIgnoreZ;
        else
            material = materialRegular;
    }


    if(glass) {
        material = materialFlat;
        material->AlphaModulate(0.45f);
    } else {
        material->AlphaModulate(
            rgba.a() / 255.0f);
    }

    material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
    material->ColorModulate(
        rgba.r() / 255.0f,
        rgba.g() / 255.0f,
        rgba.b() / 255.0f);

	if (transparent)
		material->AlphaModulate(0);

    g_MdlRender->ForcedMaterialOverride(material);
}

void Chams::OnDrawModelExecute(
    IMatRenderContext* ctx,
    const DrawModelState_t &state,
    const ModelRenderInfo_t &info,
    matrix3x4_t *matrix)
{
    static auto fnDME = Hooks::mdlrender_hook.get_original<Hooks::DrawModelExecute>(index::DrawModelExecute);

    const auto mdl = info.pModel;

    bool is_arm = strstr(mdl->szName, "arms")             != nullptr;
    bool is_player = strstr(mdl->szName, "models/player") != nullptr;
    bool is_sleeve = strstr(mdl->szName, "sleeve")        != nullptr;
    bool is_weapon = strstr(mdl->szName, "weapons/v_")  != nullptr;

	if ( is_player && g_Options.esp_history == 2 && g_Options.legit_backtrack && g_Options.legit_enable)
	{
		for ( int t = 0; t <= g_Options.legit_backtrack; ++t )
		{
			if ( t == 0 )
				continue;
			if ( g_Options.esp_history_last && !( t == g_Options.legit_backtrack ) )
				continue;

			if ( headPositions[info.entity_index][t].simtime && headPositions[info.entity_index][t].simtime + 1 > g_LocalPlayer->m_flSimulationTime( ) )
			{
				static IMaterial* mat = materialRegular;
				mat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, g_Options.chams_player_ignorez );
				mat->ColorModulate( 1.f, 1.f, 1.f );
				mat->AlphaModulate( 1.f - ( t * 0.07f ) );

				g_MdlRender->ForcedMaterialOverride( mat );
				fnDME( g_MdlRender, ctx, state, info, bt_data[info.entity_index][t].bone );
				g_MdlRender->ForcedMaterialOverride( nullptr );
			}
		}
	}

    if(is_player && (g_Options.chams_player_enabled)) {
      
		if (!g_Options.chams_player_enabled) return;

		bool Normal = g_Options.chams_player_enabled == 1;
		bool Flat = g_Options.chams_player_enabled == 2;
		bool Wireframe = g_Options.chams_player_enabled == 3;
		bool Metallic = g_Options.chams_player_enabled == 5;

		auto ent = C_BasePlayer::GetPlayerByIndex(info.entity_index);

		if (ent && g_LocalPlayer && ent->IsAlive())
		{
			const auto enemy = ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
			if (!enemy && g_Options.esp_enemies_only)
				return;

			const auto clr_front = enemy ? g_Options.color_chams_player_enemy_visible : g_Options.color_chams_player_ally_visible;
			const auto clr_back = enemy ? g_Options.color_chams_player_enemy_occluded : g_Options.color_chams_player_ally_occluded;

			if (g_Options.chams_player_enabled == 4)
			{
				if (g_Options.chams_player_ignorez)
				{
					if ( !Metallic ) {
						OverrideMaterial( true, false, false, false, clr_back, false );
						fnDME( g_MdlRender, ctx, state, info, matrix );

						OverrideMaterial( true, false, true, false, clr_front, false );
						fnDME( g_MdlRender, ctx, state, info, matrix );

						OverrideMaterial( false, false, false, false, clr_front, false );
						fnDME( g_MdlRender, ctx, state, info, matrix );

						OverrideMaterial( false, false, true, false, clr_back, false );
						fnDME( g_MdlRender, ctx, state, info, matrix );
					}
					else {

						materialMetalic->ColorModulate( clr_back.r() / 255, clr_back.g() / 255, clr_back.b() / 255 );

						materialMetalic->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );

						g_MdlRender->ForcedMaterialOverride( materialMetalic );
						fnDME( g_MdlRender, ctx, state, info, matrix );

						materialMetalic->ColorModulate( clr_front.r() / 255, clr_front.g() / 255, clr_front.b() / 255 );
						materialMetalic->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
						g_MdlRender->ForcedMaterialOverride( materialMetalic );
						fnDME( g_MdlRender, ctx, state, info, matrix );

					}

				}
				else
				{
					if ( !Metallic ) {
						OverrideMaterial( false, false, false, false, clr_front, false );
						fnDME( g_MdlRender, ctx, state, info, matrix );

						OverrideMaterial( false, false, true, false, clr_back, false );
						fnDME( g_MdlRender, ctx, state, info, matrix );
					}
					else
					{
						materialMetalic->ColorModulate( clr_front.r() / 255, clr_front.g() / 255, clr_front.b() / 255 );
						materialMetalic->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
						g_MdlRender->ForcedMaterialOverride( materialMetalic );
						fnDME( g_MdlRender, ctx, state, info, matrix );
					}


				}
			}
			else
			{
				if (g_Options.chams_player_ignorez)
				{
					OverrideMaterial(true, Flat, Wireframe, false, clr_back, false);
					fnDME(g_MdlRender, ctx, state, info, matrix);
					OverrideMaterial(false, Flat, Wireframe, false, clr_front, false);
					fnDME(g_MdlRender, ctx, state, info, matrix);


				}
				else
				{
					OverrideMaterial(false, Flat, Wireframe, false, clr_front, false);
					fnDME(g_MdlRender, ctx, state, info, matrix);

				}
			}
		}
    } 
	else if(is_arm) 
	{
        static auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
        if(!material)
            return;
        if(g_Options.misc_no_hands)
		{
            material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
            g_MdlRender->ForcedMaterialOverride(material);
        }
		else if (g_Options.misc_arms_chams)
		{

			if (g_Options.misc_arms_chams == 4)
			{
				OverrideMaterial(false, false, false, false, g_Options.misc_arms_color, false);
				fnDME(g_MdlRender, ctx, state, info, matrix);

				OverrideMaterial(false, false, true, false, g_Options.hand_color2, false);
				fnDME(g_MdlRender, ctx, state, info, matrix);
			}
			else {
				bool Flat = g_Options.misc_arms_chams == 2;
				bool Wireframes = g_Options.misc_arms_chams == 3;

				OverrideMaterial(false, Flat, Wireframes, false, g_Options.misc_arms_color, false);
				fnDME(g_MdlRender, ctx, state, info, matrix);
			}
		}

    }
	else if (is_weapon && g_Options.misc_weapon_chams)
	{

		if (g_Options.misc_weapon_chams == 4)
		{
			OverrideMaterial(false, false, false, false, g_Options.misc_weapon_color, false);
			fnDME(g_MdlRender, ctx, state, info, matrix);

			OverrideMaterial(false, false, true, false, g_Options.weap_color2, false);
			fnDME(g_MdlRender, ctx, state, info, matrix);
		}
		else
		{
			bool Flat = g_Options.misc_weapon_chams == 2;
			bool Wireframes = g_Options.misc_weapon_chams == 3;

			OverrideMaterial(false, Flat, Wireframes, false, g_Options.misc_weapon_color, false);
			fnDME(g_MdlRender, ctx, state, info, matrix);
		}

	}
	
}