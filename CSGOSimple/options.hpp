#pragma once

#include <string>
#include "valve_sdk/Misc/Color.hpp"

#define OPTION(type, var, val) type var = val

enum MenuKnife
{
	Off,
	Bayonet,
	M9,
	Karam,
	Gut,
	Flip,
	Butter,
	Shadow,
	Flachion,
	Hunts,
	Bowei,
	Navja,
	Stiletto,
	Ursus,
	Talon
};

class Config
{
public:

	float Hitmark_alpha = 0.f;
	int CurrentPlayer;
	char* PlayeName;
	bool CanAutoPistol = false;

	int seq;

	bool beta;
	bool light;

	float Real_yaw;
	float Fake_yaw;

	bool esp_playerlist;

	OPTION( bool, obs_skeleton, false );
	OPTION( bool, bypass_obs, false );
	OPTION( bool, obs_box, false );
	OPTION( bool, obs_name, false );
	OPTION( bool, obs_health, false );

	struct
	{
		std::string PlayerName;

		bool bShittalk;
		bool bOverrideESPCol;
		bool bOverrideGlowCol;
		bool bOverrideChamsCol;

		bool is_forced_esp;
		bool is_friendly;

	}Players[64];

	OPTION(bool, rage_enable, false);
	OPTION(bool, rage_aimbot_enable, false);
	OPTION(int, rage_aimbot_key, 0);
	OPTION(float, rage_aimbot_fov, 39.f);
	OPTION(bool, rage_aimbot_autofire, false);
	OPTION(float, rage_aimbot_pistol_hitchance, 20.f);
	OPTION(float, rage_aimbot_rifle_hitchance, 50.f);
	OPTION(float, rage_aimbot_sniper_hitchance, 70.f);
	OPTION(float, rage_aimbot_pistol_mindamage, 10.f);
	OPTION(float, rage_aimbot_sniper_mindamage, 10.f);
	OPTION(float, rage_aimbot_rifle_mindamage, 10.f);
	OPTION(bool, rage_aimbot_head, true);
	OPTION(bool, rage_aimbot_chest, false);
	OPTION(bool, rage_aimbot_arms, false);
	OPTION(bool, rage_aimbot_pelvis, true);
	OPTION(bool, rage_aimbot_legs, false);
	OPTION(bool, rage_aimbot_feet, true);
	OPTION(int, rage_aimbot_baim, false);
	OPTION(bool, rage_aimbot_silent, true);
	OPTION(bool, rage_aimbot_resolver, false);

	OPTION( bool , rage_aa_enable , false );
	OPTION( int , rage_aa_fakelag , 0 );
	OPTION( int , rage_aa_fakewalk , 0 );
	OPTION( int , rage_aa_move_pitch , 0 );
	OPTION( int , rage_aa_move_real , 0 );
	OPTION( int , rage_aa_move_fake , 0 );
	OPTION( int , rage_aa_stand_pitch , 0 );
	OPTION( int , rage_aa_stand_real , 0 );
	OPTION( int , rage_aa_stand_fake , 0 );

	OPTION(bool, legit_enable, false);
	OPTION(int, legit_backtrack, false);
	OPTION(bool, legit_trigger_rifle, false);
	OPTION(bool, legit_trigger_pistol, false);
	OPTION(bool, legit_trigger_sniper, false);
	OPTION(int, legit_trigger_key, 0);
	OPTION(int, legit_trigger_delay, 2);
	OPTION(bool, legit_trigger_head, true);
	OPTION(bool, legit_trigger_body, true);
	OPTION(bool, legit_trigger_legs, false);
	OPTION(bool, legit_trigger_arms, false);
	OPTION(float, legit_rifle_fov, 6.f);
	OPTION(float, legit_rifle_rcs, 75.f);
	OPTION(int, legit_rifle_bone, 0);
	OPTION(float, legit_rifle_smooth, 3.8f);
	OPTION(float, legit_rifle_rand_smooth, 1.1f);
	OPTION(int, legit_rifle_key, 0);
	OPTION(int, legit_pistol_key, 0);
	OPTION(int, legit_sniper_key, 0);
	OPTION(float, legit_pistol_fov, 6.f);
	OPTION(float, legit_pistol_rcs, 50.f);
	OPTION(float, legit_pistol_rand_smooth, 1.1f);
	OPTION(int, legit_pistol_bone, 0);
	OPTION(float, legit_pistol_smooth, 3.8f);
	OPTION(float, legit_sniper_fov, 6.f);
	OPTION(float, legit_sniper_rcs, 0.f);
	OPTION(int, legit_sniper_bone, 0);
	OPTION(float, legit_sniper_smooth, 3.8f);
	OPTION(float, legit_sniper_rand_smooth, 1.1f);

	OPTION(int, legit_mode, 0);
	OPTION( bool , legit_aimbot_friendlyfire , false );
	OPTION( bool , rage_frindlyfire , false );
	OPTION( bool , legit_smoke_check , false );
	OPTION( float , trigger_hc , 50 );

	int current_legit_wep;
	struct
	{
		bool trigger = false;
		float fov = 0.f;
		int hitbox = 0;
		float smooth = 3.8f;
		float rcs = 0.f;
		int key = 0;

		int trigger_key = 0;
		int trigger_delay =  2;
		bool legit_trigger_head = true;
		bool legit_trigger_body = true;
		bool  legit_trigger_legs = false;
		bool legit_trigger_arms = false;
		float trigger_hc = 50.f;
		float RCS;

	} advanced_legit[519];

	OPTION( bool , misc_autoaccept , false );
	OPTION( bool , misc_legit_aa , false );
	OPTION( bool, misc_slidewalk, false );

    // 
    // ESP
    // 
    OPTION(bool, esp_enabled, false);
	OPTION(bool, esp_dead_only, false);
	OPTION(bool, esp_visible_only, false);
    OPTION(bool, esp_enemies_only, true);
    OPTION(int, esp_player_boxes, false);
	OPTION(bool, esp_player_boxes_outline, true);
    OPTION(int, esp_player_names, 0);
    OPTION(int, esp_player_health, false);
    OPTION(bool, esp_player_armour, false);
    OPTION(int, esp_player_weapons, 0);
    OPTION(bool, esp_player_snaplines, false);
    OPTION(bool, esp_dropped_weapons, false);
    OPTION(bool, esp_defuse_kit, false);
    OPTION(bool, esp_planted_c4, false);
	OPTION(bool, esp_nade_esp, false);
	OPTION(int, esp_history, 0);
	OPTION( bool, esp_history_last, false );
	OPTION(bool, esp_skeleton, false);
	OPTION(bool, esp_grenadeprediction, false);
	OPTION(bool, esp_autowall, false);


    // 
    // GLOW
    // 
    OPTION(bool, glow_enabled, false);
	OPTION(float, glow_alpha, 0.65f);

    //
    // CHAMS
    //
    OPTION(int, chams_player_enabled, 0);
    OPTION(bool, chams_player_ignorez, false);

    //
    // MISC
    //
    OPTION(bool, misc_bhop, false);
	OPTION(bool, misc_autosrafe, false);
    OPTION(bool, misc_no_hands, false);
	OPTION(int, misc_tp_key, 0);
	OPTION(bool, misc_thirdperson, false);
	OPTION(float, misc_thirdperson_dist, 130.f);
	OPTION(int, misc_clantag, 0);
	char tag[42];
	OPTION(bool, misc_noname, false);
	OPTION(bool, misc_radar, false);
	OPTION(bool, misc_nosmoke, false);
	OPTION(bool, misc_noflash, false);
	OPTION(int, misc_hitmark, false);
	OPTION(float, misc_hitmark_volume, 50.f);
	OPTION(bool, misc_speclist, false);
	OPTION(bool, misc_namesteal, false);
	OPTION(bool, misc_autopistol, false);
	OPTION(int, misc_chatspam, false);
	char spam[128];
	OPTION(float, misc_viewmodel_fov, 0);
	OPTION(bool, misc_rankreveal, false);
	bool misc_sniper_crosshair;
	OPTION(bool, misc_disable_pp, false);
	OPTION(bool, misc_nightmode, false);
	OPTION(bool, misc_col_sky, false);
	OPTION(float, misc_fov, 0);
	OPTION(int, misc_arms_chams, 0);
	OPTION(int, misc_weapon_chams, 0);
	OPTION(Color, misc_arms_color, Color(22, 81, 211, 255));
	OPTION(Color, misc_weapon_color, Color(227, 38, 38, 255));
	OPTION(bool, misc_event_log, false);
	OPTION( bool , misc_knife_switch , false );

	OPTION(Color, hand_color2, Color(211, 81, 22, 255));
	OPTION(Color, weap_color2, Color(38, 38, 227, 255));

	OPTION(bool, skins_enable, false);
	OPTION(int, skins_knife_model, 0);
	OPTION(int, skins_glove_model, 0);
	OPTION(int, skins_glove_skin, 0);

	int currentWeapon;
	struct
	{
		bool ChangerEnabled = false;
		int ChangerSkin = 0;
		char ChangerName[32] = "";
		bool stat;
		int statamount;
		bool customstat;
		int customstatamount;
		char Name;
	} W[519];

    // 
    // COLORS
    // 
    OPTION(Color, color_esp_ally_visible, Color(0, 128, 255));
    OPTION(Color, color_esp_enemy_visible, Color(111, 227, 40, 255));
    OPTION(Color, color_esp_ally_occluded, Color(0, 128, 255));
    OPTION(Color, color_esp_enemy_occluded, Color(180, 7, 233, 255));
    OPTION(Color, color_esp_weapons, Color(250, 250, 250));
    OPTION(Color, color_esp_defuse, Color(0, 128, 255));
    OPTION(Color, color_esp_c4, Color(255, 255, 0));

    OPTION(Color, color_glow_ally, Color(0, 128, 255));
    OPTION(Color, color_glow_enemy, Color(255, 255, 255));

    OPTION(Color, color_chams_player_ally_visible, Color(0, 128, 255));
    OPTION(Color, color_chams_player_ally_occluded, Color(0, 255, 128));
    OPTION(Color, color_chams_player_enemy_visible, Color(2, 255, 243, 255));
    OPTION(Color, color_chams_player_enemy_occluded, Color(159, 212, 13, 255));
	OPTION(Color, color_grenadeprediction, Color(40, 200, 100));
	OPTION(Color, color_sky, Color(170, 30, 130));
	OPTION(Color, color_nightmode, Color(50, 50, 50));

	bool update_world;

};

extern Config g_Options;
extern bool   g_Unload;