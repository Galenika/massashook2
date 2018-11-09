#include "Config.h"
#include "../options.hpp"
#include "../valve_sdk/sdk.hpp"

#include <fstream>
#include <experimental/filesystem>

#include "../json.h"

nlohmann::json config;

void ConfigSys::CreateConfigFolder ( std::string path )
{
	if ( !std::experimental::filesystem::create_directory ( path ) ) return;
}

bool ConfigSys::FileExists ( std::string file )
{
	return std::experimental::filesystem::exists ( file );
}

const char* GetWeaponName ( int iWeaponID )
{
	switch ( iWeaponID )
	{
	default:
		return "none";
	case WEAPON_MP5_SD:
		return "mp5-sd";
	case WEAPON_DEAGLE:
		return "deagle";
	case WEAPON_DUALS:
		return "duals";
	case WEAPON_FIVE7:
		return "five7";
	case WEAPON_GLOCK:
		return "glock";
	case WEAPON_AK47:
		return "ak47";
	case WEAPON_AUG:
		return "aug";
	case WEAPON_AWP:
		return "awp";
	case WEAPON_FAMAS:
		return "famas";
	case WEAPON_G3SG1:
		return "g3sg1";
	case WEAPON_GALIL:
		return "galil";
	case WEAPON_M249:
		return "m249";
	case WEAPON_M4A1:
		return "m4a4";
	case WEAPON_MAC10:
		return "mac10";
	case WEAPON_P90:
		return "p90";
	case WEAPON_UMP45:
		return "ump45";
	case WEAPON_XM1014:
		return "xm1014";
	case WEAPON_BIZON:
		return "bizon";
	case WEAPON_MAG7:
		return "mag7";
	case WEAPON_NEGEV:
		return "negev";
	case WEAPON_SAWEDOFF:
		return "sawedoff";
	case WEAPON_TEC9:
		return "tec9";
	case WEAPON_TASER:
		return "taser";
	case WEAPON_USPS:
		return "usp-s";
	case WEAPON_MP7:
		return "mp7";
	case WEAPON_MP9:
		return "mp9";
	case WEAPON_NOVA:
		return "nova";
	case WEAPON_P250:
		return "p250";
	case WEAPON_SCAR20:
		return "scar20";
	case WEAPON_SG553:
		return "sg556";
	case WEAPON_SCOUT:
		return "ssg08";
	case WEAPON_M4A1S:
		return "m4a1-s";
	case WEAPON_P2000:
		return "p2000";
	case WEAPON_CZ75:
		return "cz75";
	case WEAPON_REVOLVER:
		return "revolver";
	case WEAPON_KNIFE_BAYONET:
		return "bayonet";
	case WEAPON_KNIFE_BUTTERFLY:
		return "butterfly";
	case WEAPON_KNIFE_FALCHION:
		return "falshion";
	case WEAPON_KNIFE_FLIP:
		return "flip";
	case WEAPON_KNIFE_GUT:
		return "gut";
	case WEAPON_KNIFE_KARAMBIT:
		return "karambit";
	case WEAPON_KNIFE_M9BAYONET:
		return "m9";
	case WEAPON_KNIFE_HUNTSMAN:
		return "huntsman";
	case WEAPON_KNIFE_BOWIE:
		return "bowie";
	case WEAPON_KNIFE_DAGGER:
		return "daggers";
	case weapon_knife_gypsy_jackknife:
		return "jackknife";
	case weapon_knife_stiletto:
		return "stiletto";
	case weapon_knife_ursus:
		return "ursos";
	case weapon_knife_widowmaker:
		return "widowmaker";
	}
	return "none";
}


void ConfigSys::SaveConfig(const std::string path)
{
	std::ofstream output_file(path);

	if (!output_file.good())
		return;

	Save(g_Options.misc_legit_aa, "misc_legit_aa");
	Save(g_Options.misc_autoaccept, "misc_autoaccept");
	Save(g_Options.misc_knife_switch, "misc_knife_switch");
	Save(g_Options.misc_rankreveal, "misc_rankreveal");
	Save(g_Options.misc_autopistol, "misc_autopistol");
	Save(g_Options.misc_nosmoke, "nosmoke");
	Save(g_Options.legit_aimbot_friendlyfire, "legit_aimbot_friendlyfire");
	Save(g_Options.legit_smoke_check, "legit_smoke_check");
	Save(g_Options.trigger_hc, "trigger_hc");
	Save(g_Options.rage_frindlyfire, "rage_frindlyfire");

	Save(g_Options.rage_aimbot_arms, "rage_aimbot_arms");
	Save(g_Options.rage_aimbot_autofire, "rage_aimbot_autofire");
	Save(g_Options.rage_aimbot_baim, "rage_aimbot_baim");
	Save(g_Options.rage_aimbot_chest, "rage_aimbot_chest");
	Save(g_Options.rage_aimbot_enable, "rage_aimbot_enable");
	Save(g_Options.rage_aimbot_feet, "rage_aimbot_feet");
	Save(g_Options.rage_aimbot_fov, "rage_aimbot_fov");
	Save(g_Options.rage_aimbot_head, "rage_aimbot_head");
	Save(g_Options.rage_aimbot_key, "rage_aimbot_key");
	Save(g_Options.rage_aimbot_legs, "rage_aimbot_legs");
	Save(g_Options.rage_aimbot_pelvis, "rage_aimbot_pelvis");
	Save(g_Options.rage_aimbot_pistol_hitchance, "rage_aimbot_pistol_hitchance");
	Save(g_Options.rage_aimbot_pistol_mindamage, "rage_aimbot_pistol_mindamage");
	Save(g_Options.rage_aimbot_rifle_hitchance, "rage_aimbot_rifle_hitchance");
	Save(g_Options.rage_aimbot_rifle_mindamage, "rage_aimbot_rifle_mindamage");
	Save(g_Options.rage_aimbot_silent, "rage_aimbot_silent");
	Save(g_Options.rage_aimbot_sniper_hitchance, "rage_aimbot_sniper_hitchance");
	Save(g_Options.rage_aimbot_sniper_mindamage, "rage_aimbot_sniper_mindamage");
	Save(g_Options.rage_enable, "rage_enable");

	Save(g_Options.legit_backtrack, "legit_backtrack");
	Save(g_Options.legit_enable, "legit_enable");
	Save(g_Options.legit_trigger_pistol, "legit_pistol");
	Save(g_Options.legit_trigger_rifle, "legit_rifle");
	Save(g_Options.legit_trigger_sniper, "legit_sniper");
	Save(g_Options.legit_trigger_arms, "legit_trigger_arms");
	Save(g_Options.legit_trigger_body, "legit_trigger_body");
	Save(g_Options.legit_trigger_delay, "legit_trigger_delay");
	Save(g_Options.legit_trigger_head, "legit_trigger_head");
	Save(g_Options.legit_trigger_key, "legit_trigger_key");
	Save(g_Options.legit_trigger_legs, "legit_trigger_legs");

	Save(g_Options.legit_rifle_bone, "legit_rifle_bone");
	Save(g_Options.legit_rifle_fov, "legit_rifle_fov");
	Save(g_Options.legit_rifle_key, "legit_rifle_key");
	Save(g_Options.legit_rifle_rcs, "legit_rifle_rcs");
	Save(g_Options.legit_rifle_smooth, "legit_rifle_smooth");
	Save(g_Options.legit_pistol_bone, "legit_pistol_bone");
	Save(g_Options.legit_pistol_fov, "legit_pistol_fov");
	Save(g_Options.legit_pistol_key, "legit_pistol_key");
	Save(g_Options.legit_pistol_rcs, "legit_pistol_rcs");
	Save(g_Options.legit_pistol_smooth, "legit_pistol_smooth");
	Save(g_Options.legit_sniper_bone, "legit_sniper_bone");
	Save(g_Options.legit_sniper_fov, "legit_sniper_fov");
	Save(g_Options.legit_sniper_key, "legit_sniper_key");
	Save(g_Options.legit_sniper_rcs, "legit_sniper_rcs");
	Save(g_Options.legit_sniper_smooth, "legit_sniper_smooth");

	Save(g_Options.esp_enabled, "esp_enable");
	Save(g_Options.esp_enemies_only, "esp_enemies_only");
	Save(g_Options.esp_player_boxes, "esp_player_boxes");
	Save(g_Options.esp_player_names, "esp_player_names");
	Save(g_Options.esp_player_health, "esp_player_health");
	Save(g_Options.esp_player_armour, "esp_player_armour");
	Save(g_Options.esp_skeleton, "esp_skeleton");
	Save(g_Options.esp_player_weapons, "esp_player_weapons");
	Save(g_Options.esp_player_snaplines, "esp_player_snaplines");
	Save(g_Options.esp_dropped_weapons, "esp_dropped_weapons");
	Save(g_Options.esp_defuse_kit, "esp_defuse_kit");
	Save(g_Options.esp_planted_c4, "esp_planted_c4");
	Save(g_Options.esp_history, "esp_history");

	Save(g_Options.glow_enabled, "glow_enabled");
	Save(g_Options.glow_alpha, "glow_alpha");
	Save(g_Options.misc_noflash, "misc_noflash");

	Save(g_Options.chams_player_enabled, "chams_player_enabled");
	Save(g_Options.chams_player_ignorez, "chams_player_ignorez");
	Save(g_Options.misc_arms_chams, "misc_arms_chams");
	Save(g_Options.misc_weapon_chams, "misc_weapon_chams");
	SaveColor(g_Options.misc_weapon_color, "misc_weapon_color");
	SaveColor(g_Options.misc_arms_color, "misc_arms_color");
	Save(g_Options.misc_disable_pp, "misc_disable_pp");
	Save(g_Options.misc_bhop, "misc_bhop");
	Save(g_Options.misc_autosrafe, "misc_autostrafe");
	Save(g_Options.misc_no_hands, "misc_no_hands");
	Save(g_Options.misc_thirdperson, "misc_thirdperson");
	Save(g_Options.misc_thirdperson_dist, "misc_thirdperson_dist");
	Save(g_Options.misc_clantag, "misc_clantag");
	Save(g_Options.misc_radar, "misc_radar");
	Save(g_Options.misc_noname, "misc_noname");
	Save(g_Options.misc_hitmark, "misc_hitmark");
	Save(g_Options.misc_hitmark_volume, "misc_hitmark_volume");
	Save(g_Options.misc_speclist, "misc_speclist");
	Save(g_Options.esp_grenadeprediction, "esp_grenadeprediction");

	Save(g_Options.misc_tp_key, "misc_tp_key");
	Save(g_Options.misc_viewmodel_fov, "misc_viewmodel_fov");
	Save(g_Options.misc_fov, "misc_fov");
	Save(g_Options.esp_autowall, "esp_autowall");
	Save(g_Options.esp_player_boxes_outline, "esp_player_boxes_outline");

	Save(g_Options.skins_enable, "skins_enable");
	Save(g_Options.skins_glove_model, "skins_glove_model");
	Save(g_Options.skins_glove_skin, "skins_glove_skin");
	Save(g_Options.skins_knife_model, "skins_knife_model");
	for (int id = 0; id < 64; id++)
	{
		Save(g_Options.W[id].ChangerSkin, GetWeaponName(id));
		Save(g_Options.advanced_legit[id].fov, std::string(GetWeaponName(id) + std::string("_fov")).c_str());
		Save(g_Options.advanced_legit[id].hitbox, std::string(GetWeaponName(id) + std::string("_hitbox")).c_str());
		Save(g_Options.advanced_legit[id].key, std::string(GetWeaponName(id) + std::string("_key")).c_str());
		Save(g_Options.advanced_legit[id].legit_trigger_arms, std::string(GetWeaponName(id) + std::string("_trigger_arms")).c_str());
		Save(g_Options.advanced_legit[id].legit_trigger_body, std::string(GetWeaponName(id) + std::string("_trigger_body")).c_str());
		Save(g_Options.advanced_legit[id].legit_trigger_head, std::string(GetWeaponName(id) + std::string("_trigger_head")).c_str());
		Save(g_Options.advanced_legit[id].legit_trigger_legs, std::string(GetWeaponName(id) + std::string("_trigger_legs")).c_str());
		Save(g_Options.advanced_legit[id].rcs, std::string(GetWeaponName(id) + std::string("_rcs")).c_str());
		Save(g_Options.advanced_legit[id].smooth, std::string(GetWeaponName(id) + std::string("_smooth")).c_str());
		Save(g_Options.advanced_legit[id].trigger, std::string(GetWeaponName(id) + std::string("_trigger")).c_str());
		Save(g_Options.advanced_legit[id].trigger_delay, std::string(GetWeaponName(id) + std::string("_trigger_delay")).c_str());
		Save(g_Options.advanced_legit[id].trigger_key, std::string(GetWeaponName(id) + std::string("_trigger_key")).c_str());
	}
	for (int id = 500; id <= 523; id++)
	{
		if ((id < 505 && id >= 501) || (id > 512 && id < 514) || id <= 523 && id >= 519)
			continue;
		Save(g_Options.W[id].ChangerSkin, GetWeaponName(id));
	}

	Save(g_Options.misc_event_log, "event_log");
	SaveColor(g_Options.color_chams_player_ally_occluded, "color_chams_player_ally_occluded");
	SaveColor(g_Options.color_chams_player_ally_visible, "color_chams_player_ally_visible");
	SaveColor(g_Options.color_chams_player_enemy_occluded, "color_chams_player_enemy_occluded");
	SaveColor(g_Options.color_chams_player_enemy_visible, "color_chams_player_enemy_visible");
	SaveColor(g_Options.color_esp_ally_occluded, "color_esp_ally_occluded");
	SaveColor(g_Options.color_esp_ally_visible, "color_esp_ally_visible");
	SaveColor(g_Options.color_esp_c4, "color_esp_c4");
	SaveColor(g_Options.color_esp_defuse, "color_esp_defuse");
	SaveColor(g_Options.color_esp_enemy_occluded, "color_esp_enemy_occluded");
	SaveColor(g_Options.color_esp_enemy_visible, "color_esp_enemy_visible");
	SaveColor(g_Options.color_esp_weapons, "color_esp_weapons");
	SaveColor(g_Options.color_glow_ally, "color_glow_ally");
	SaveColor(g_Options.color_glow_enemy, "color_glow_enemy");
	SaveColor(g_Options.color_grenadeprediction, "color_grenadeprediction");

	Save(g_Options.legit_pistol_rand_smooth, "legit_pistol_rand_smooth");
	Save(g_Options.legit_rifle_rand_smooth, "legit_rifle_rand_smooth");
	Save(g_Options.legit_sniper_rand_smooth, "legit_sniper_rand_smooth");
	Save(g_Options.esp_dead_only, "esp_dead_only");
	Save(g_Options.esp_visible_only, "esp_visible_only");
	Save(g_Options.misc_nightmode, "misc_nightmode");
	Save(g_Options.misc_col_sky, "misc_col_sky");
	SaveColor(g_Options.color_sky, "color_sky");
	SaveColor(g_Options.color_nightmode, "color_nightmode");

	SaveColor(g_Options.hand_color2, "hand_color2");
	SaveColor(g_Options.weap_color2, "weap_color2");

	Save( g_Options.bypass_obs, "bypass_obs" );
	Save( g_Options.obs_box, "obs_box" );
	Save( g_Options.obs_health, "obs_health" );
	Save( g_Options.obs_name, "obs_name" );

	Save( g_Options.misc_slidewalk, "misc_slidewalk" );
	Save(g_Options.esp_nade_esp, "esp_nade_esp");
	Save(g_Options.esp_playerlist, "esp_playerlist");
	Save(g_Options.misc_sniper_crosshair, "misc_sniper_crosshair");
	output_file << std::setw(4) << config << std::endl;
	output_file.close();
}

void ConfigSys::LoadConfig( const std::string path )
{
	std::ifstream input_file( path );

	if ( !input_file.good( ) )
		return;

	try
	{
		config << input_file;
	}
	catch ( ... )
	{
		input_file.close( );
		return;
	}

	LoadColor(&g_Options.hand_color2, "hand_color2");
	LoadColor(&g_Options.weap_color2, "weap_color2");

	Load( g_Options.misc_slidewalk, "misc_slidewalk" );

	Load( g_Options.bypass_obs, "bypass_obs" );
	Load( g_Options.obs_box, "obs_box" );
	Load( g_Options.obs_health, "obs_health" );
	Load( g_Options.obs_name, "obs_name" );

	Load(g_Options.misc_sniper_crosshair, "misc_sniper_crosshair");
	Load(g_Options.esp_playerlist, "esp_playerlist");
	Load(g_Options.esp_nade_esp, "esp_nade_esp");
	Load( g_Options.misc_legit_aa , "misc_legit_aa" );
	Load( g_Options.misc_knife_switch , "misc_knife_switch" );
	Load( g_Options.misc_autopistol , "misc_autopistol" );
	Load( g_Options.misc_nosmoke , "nosmoke" );
	Load( g_Options.misc_namesteal , "namesteal" );
	Load( g_Options.misc_clantag , "clantag" );
	Load( g_Options.misc_noname , "noname" );
	Load( g_Options.legit_aimbot_friendlyfire , "legit_aimbot_friendlyfire" );
	Load( g_Options.legit_smoke_check , "legit_smoke_check" );
	Load( g_Options.trigger_hc , "trigger_hc" );
	Load( g_Options.rage_frindlyfire , "rage_frindlyfire" );
	Load( g_Options.misc_event_log , "event_log" );
	LoadColor( &g_Options.color_nightmode , "color_nightmode" );
	Load( g_Options.misc_nightmode , "misc_nightmode" );
	Load( g_Options.misc_col_sky , "misc_col_sky" );
	LoadColor( &g_Options.color_sky , "color_sky" );
	Load( g_Options.misc_arms_chams , "misc_arms_chams" );
	Load( g_Options.misc_weapon_chams , "misc_weapon_chams" );
	LoadColor( &g_Options.misc_weapon_color , "misc_weapon_color" );
	LoadColor( &g_Options.misc_arms_color , "misc_arms_color" );

	Load( g_Options.esp_visible_only , "esp_visible_only" );
	Load( g_Options.esp_dead_only , "esp_dead_only" );
	Load( g_Options.misc_disable_pp , "misc_disable_pp" );
	Load( g_Options.misc_rankreveal , "misc_rankreveal" );
	Load( g_Options.rage_aimbot_arms , "rage_aimbot_arms" );
	Load( g_Options.rage_aimbot_autofire , "rage_aimbot_autofire" );
	Load( g_Options.rage_aimbot_baim , "rage_aimbot_baim" );
	Load( g_Options.rage_aimbot_chest , "rage_aimbot_chest" );
	Load( g_Options.rage_aimbot_enable , "rage_aimbot_enable" );
	Load( g_Options.rage_aimbot_feet , "rage_aimbot_feet" );
	Load( g_Options.rage_aimbot_fov , "rage_aimbot_fov" );
	Load( g_Options.rage_aimbot_head , "rage_aimbot_head" );
	Load( g_Options.rage_aimbot_key , "rage_aimbot_key" );
	Load( g_Options.rage_aimbot_legs , "rage_aimbot_legs" );
	Load( g_Options.rage_aimbot_pelvis , "rage_aimbot_pelvis" );
	Load( g_Options.rage_aimbot_pistol_hitchance , "rage_aimbot_pistol_hitchance" );
	Load( g_Options.rage_aimbot_pistol_mindamage , "rage_aimbot_pistol_mindamage" );
	Load( g_Options.rage_aimbot_rifle_hitchance , "rage_aimbot_rifle_hitchance" );
	Load( g_Options.rage_aimbot_rifle_mindamage , "rage_aimbot_rifle_mindamage" );
	Load( g_Options.rage_aimbot_silent , "rage_aimbot_silent" );
	Load( g_Options.rage_aimbot_sniper_hitchance , "rage_aimbot_sniper_hitchance" );
	Load( g_Options.rage_aimbot_sniper_mindamage , "rage_aimbot_sniper_mindamage" );
	Load( g_Options.rage_enable , "rage_enable" );

	Load( g_Options.esp_player_boxes_outline , "esp_player_boxes_outline" );

	Load( g_Options.legit_backtrack , "legit_backtrack" );
	Load( g_Options.legit_enable , "legit_enable" );
	Load( g_Options.legit_trigger_pistol , "legit_pistol" );
	Load( g_Options.legit_trigger_rifle , "legit_rifle" );
	Load( g_Options.legit_trigger_sniper , "legit_sniper" );
	Load( g_Options.legit_trigger_arms , "legit_trigger_arms" );
	Load( g_Options.legit_trigger_body , "legit_trigger_body" );
	Load( g_Options.legit_trigger_delay , "legit_trigger_delay" );
	Load( g_Options.legit_trigger_head , "legit_trigger_head" );
	Load( g_Options.legit_trigger_key , "legit_trigger_key" );
	Load( g_Options.legit_trigger_legs , "legit_trigger_legs" );

	Load( g_Options.legit_rifle_bone , "legit_rifle_bone" );
	Load( g_Options.legit_rifle_fov , "legit_rifle_fov" );
	Load( g_Options.legit_rifle_key , "legit_rifle_key" );
	Load( g_Options.legit_rifle_rcs , "legit_rifle_rcs" );
	Load( g_Options.legit_rifle_smooth , "legit_rifle_smooth" );
	Load( g_Options.legit_pistol_rand_smooth , "legit_pistol_rand_smooth" );
	Load( g_Options.legit_rifle_rand_smooth , "legit_rifle_rand_smooth" );
	Load( g_Options.legit_sniper_rand_smooth , "legit_sniper_rand_smooth" );
	Load( g_Options.legit_pistol_bone , "legit_pistol_bone" );
	Load( g_Options.legit_pistol_fov , "legit_pistol_fov" );
	Load( g_Options.legit_pistol_key , "legit_pistol_key" );
	Load( g_Options.legit_pistol_rcs , "legit_pistol_rcs" );
	Load( g_Options.legit_pistol_smooth , "legit_pistol_smooth" );
	Load( g_Options.legit_sniper_bone , "legit_sniper_bone" );
	Load( g_Options.legit_sniper_fov , "legit_sniper_fov" );
	Load( g_Options.legit_sniper_key , "legit_sniper_key" );
	Load( g_Options.legit_sniper_rcs , "legit_sniper_rcs" );
	Load( g_Options.legit_sniper_smooth , "legit_sniper_smooth" );

	Load( g_Options.esp_enabled , "esp_enable" );
	Load( g_Options.esp_enemies_only , "esp_enemies_only" );
	Load( g_Options.esp_player_boxes , "esp_player_boxes" );
	Load( g_Options.esp_player_names , "esp_player_names" );
	Load( g_Options.esp_player_health , "esp_player_health" );
	Load( g_Options.esp_player_armour , "esp_player_armour" );
	Load( g_Options.esp_player_weapons , "esp_player_weapons" );
	Load( g_Options.esp_player_snaplines , "esp_player_snaplines" );
	Load( g_Options.esp_dropped_weapons , "esp_dropped_weapons" );
	Load( g_Options.esp_defuse_kit , "esp_defuse_kit" );
	Load( g_Options.esp_planted_c4 , "esp_planted_c4" );
	Load( g_Options.esp_history , "esp_history" );
	Load( g_Options.esp_skeleton , "esp_skeleton" );
	Load( g_Options.esp_grenadeprediction , "esp_grenadeprediction" );
	Load( g_Options.misc_noflash , "misc_noflash" );

	Load( g_Options.glow_enabled , "glow_enabled" );
	Load( g_Options.glow_alpha , "glow_alpha" );

	Load( g_Options.chams_player_enabled , "chams_player_enabled" );
	Load( g_Options.chams_player_ignorez , "chams_player_ignorez" );

	Load( g_Options.misc_bhop , "misc_bhop" );
	Load( g_Options.misc_no_hands , "misc_no_hands" );
	Load( g_Options.misc_thirdperson , "misc_thirdperson" );
	Load( g_Options.misc_thirdperson_dist , "misc_thirdperson_dist" );
	Load( g_Options.misc_clantag , "misc_clantag" );
	Load( g_Options.misc_radar , "misc_radar" );
	Load( g_Options.misc_noname , "misc_noname" );
	Load( g_Options.misc_hitmark , "misc_hitmark" );
	Load( g_Options.misc_hitmark_volume , "misc_hitmark_volume" );
	Load( g_Options.misc_autosrafe , "misc_autostrafe" );
	Load( g_Options.misc_speclist , "misc_speclist" );

	Load( g_Options.skins_enable , "skins_enable" );
	Load( g_Options.skins_glove_model , "skins_glove_model" );
	Load( g_Options.skins_glove_skin , "skins_glove_skin" );
	Load( g_Options.skins_knife_model , "skins_knife_model" );
	for ( int id = 0; id < 64; id++ )
	{
		Load( g_Options.W[id].ChangerSkin , GetWeaponName( id ) );
		Load( g_Options.advanced_legit[id].fov , std::string( GetWeaponName( id ) + std::string( "_fov" ) ).c_str( ) );
		Load( g_Options.advanced_legit[id].hitbox , std::string( GetWeaponName( id ) + std::string( "_hitbox" ) ).c_str( ) );
		Load( g_Options.advanced_legit[id].key , std::string( GetWeaponName( id ) + std::string( "_key" ) ).c_str( ) );
		Load( g_Options.advanced_legit[id].legit_trigger_arms , std::string( GetWeaponName( id ) + std::string( "_trigger_arms" ) ).c_str( ) );
		Load( g_Options.advanced_legit[id].legit_trigger_body , std::string( GetWeaponName( id ) + std::string( "_trigger_body" ) ).c_str( ) );
		Load( g_Options.advanced_legit[id].legit_trigger_head , std::string( GetWeaponName( id ) + std::string( "_trigger_head" ) ).c_str( ) );
		Load( g_Options.advanced_legit[id].legit_trigger_legs , std::string( GetWeaponName( id ) + std::string( "_trigger_legs" ) ).c_str( ) );
		Load( g_Options.advanced_legit[id].rcs , std::string( GetWeaponName( id ) + std::string( "_rcs" ) ).c_str( ) );
		Load( g_Options.advanced_legit[id].smooth , std::string( GetWeaponName( id ) + std::string( "_smooth" ) ).c_str( ) );
		Load( g_Options.advanced_legit[id].trigger , std::string( GetWeaponName( id ) + std::string( "_trigger" ) ).c_str( ) );
		Load( g_Options.advanced_legit[id].trigger_delay , std::string( GetWeaponName( id ) + std::string( "_trigger_delay" ) ).c_str( ) );
		Load( g_Options.advanced_legit[id].trigger_key , std::string( GetWeaponName( id ) + std::string( "_trigger_key" ) ).c_str( ) );
	}

	for ( int id = 500; id <= 516; id++ )
	{
		if ( ( id < 505 && id >= 501 ) || ( id > 512 && id < 514 ) )
			continue;
		Load( g_Options.W[id].ChangerSkin , GetWeaponName( id ) );
	}

	Load( g_Options.misc_tp_key , "misc_tp_key" );
	Load( g_Options.misc_viewmodel_fov , "misc_viewmodel_fov" );
	Load( g_Options.misc_fov , "misc_fov" );
	Load( g_Options.esp_autowall , "esp_autowall" );
	Load( g_Options.misc_autoaccept , "misc_autoaccept" );

	LoadColor( &g_Options.color_chams_player_ally_occluded , "color_chams_player_ally_occluded" );
	LoadColor( &g_Options.color_chams_player_ally_visible , "color_chams_player_ally_visible" );
	LoadColor( &g_Options.color_chams_player_enemy_occluded , "color_chams_player_enemy_occluded" );
	LoadColor( &g_Options.color_chams_player_enemy_visible , "color_chams_player_enemy_visible" );
	LoadColor( &g_Options.color_esp_ally_occluded , "color_esp_ally_occluded" );
	LoadColor( &g_Options.color_esp_ally_visible , "color_esp_ally_visible" );
	LoadColor( &g_Options.color_esp_c4 , "color_esp_c4" );
	LoadColor( &g_Options.color_esp_defuse , "color_esp_defuse" );
	LoadColor( &g_Options.color_esp_enemy_occluded , "color_esp_enemy_occluded" );
	LoadColor( &g_Options.color_esp_enemy_visible , "color_esp_enemy_visible" );
	LoadColor( &g_Options.color_esp_weapons , "color_esp_weapons" );
	LoadColor( &g_Options.color_glow_ally , "color_glow_ally" );
	LoadColor( &g_Options.color_glow_enemy , "color_glow_enemy" );
	LoadColor( &g_Options.color_grenadeprediction , "color_grenadeprediction" );

	input_file.close( );
}


std::vector<std::string> ConfigSys::GetAllConfigs ( )
{

	namespace fs = std::experimental::filesystem;

	std::string fPath = std::string ( my_documents_folder ) + "\\massas.pw\\";

	std::vector<ConfigFile> config_files = GetAllConfigsInFolder ( fPath, ".json" );
	std::vector<std::string> config_file_names;

	for ( auto config = config_files.begin ( ); config != config_files.end ( ); config++ )
		config_file_names.emplace_back ( config->GetName ( ) );

	std::sort ( config_file_names.begin ( ), config_file_names.end ( ) );

	return config_file_names;
}

std::vector<ConfigFile> ConfigSys::GetAllConfigsInFolder ( const std::string path, const std::string ext )
{
	namespace fs = std::experimental::filesystem;

	std::vector<ConfigFile> config_files;

	if ( fs::exists ( path ) && fs::is_directory ( path ) )
	{
		for ( auto it = fs::recursive_directory_iterator ( path ); it != fs::recursive_directory_iterator ( ); it++ )
		{
			if ( fs::is_regular_file ( *it ) && it->path ( ).extension ( ) == ext )
			{
				std::string fPath = path + it->path ( ).filename ( ).string ( );

				std::string tmp_f_name = it->path ( ).filename ( ).string ( );
				size_t pos = tmp_f_name.find ( "." );
				std::string fName = ( std::string::npos == pos ) ? tmp_f_name : tmp_f_name.substr ( 0, pos );

				ConfigFile new_config ( fName, fPath );
				config_files.emplace_back ( new_config );
			}
		}
	}
	return config_files;
}

template<typename T>
void ConfigSys::Load ( T &value, std::string str )
{
	if ( config [ str ].empty ( ) )
	{
		value = 0;
		return;
	}
	value = config [ str ].get<T> ( );
}

void ConfigSys::LoadArray ( float_t value [ 4 ], std::string str )
{
	if ( config [ str ] [ "0" ].empty ( ) || config [ str ] [ "1" ].empty ( ) || config [ str ] [ "2" ].empty ( ) || config [ str ] [ "3" ].empty ( ) )
		return;

	value [ 0 ] = config [ str ] [ "0" ].get<float_t> ( );
	value [ 1 ] = config [ str ] [ "1" ].get<float_t> ( );
	value [ 2 ] = config [ str ] [ "2" ].get<float_t> ( );
	value [ 3 ] = config [ str ] [ "3" ].get<float_t> ( );
}

void ConfigSys::LoadArray ( bool value [ 14 ], std::string str )
{
	if ( config [ str ] [ "0" ].empty ( ) || config [ str ] [ "1" ].empty ( ) || config [ str ] [ "2" ].empty ( ) || config [ str ] [ "3" ].empty ( )
		 || config [ str ] [ "4" ].empty ( ) || config [ str ] [ "5" ].empty ( ) || config [ str ] [ "6" ].empty ( ) || config [ str ] [ "7" ].empty ( )
		 || config [ str ] [ "8" ].empty ( ) || config [ str ] [ "9" ].empty ( ) || config [ str ] [ "10" ].empty ( ) || config [ str ] [ "11" ].empty ( )
		 || config [ str ] [ "12" ].empty ( ) || config [ str ] [ "13" ].empty ( ) )
		return;

	value [ 0 ] = config [ str ] [ "0" ].get<bool> ( );
	value [ 1 ] = config [ str ] [ "1" ].get<bool> ( );
	value [ 2 ] = config [ str ] [ "2" ].get<bool> ( );
	value [ 3 ] = config [ str ] [ "3" ].get<bool> ( );
	value [ 4 ] = config [ str ] [ "4" ].get<bool> ( );
	value [ 5 ] = config [ str ] [ "5" ].get<bool> ( );
	value [ 6 ] = config [ str ] [ "6" ].get<bool> ( );
	value [ 7 ] = config [ str ] [ "7" ].get<bool> ( );
	value [ 8 ] = config [ str ] [ "8" ].get<bool> ( );
	value [ 9 ] = config [ str ] [ "9" ].get<bool> ( );
	value [ 10 ] = config [ str ] [ "10" ].get<bool> ( );
	value [ 11 ] = config [ str ] [ "11" ].get<bool> ( );
	value [ 12 ] = config [ str ] [ "12" ].get<bool> ( );
	value [ 13 ] = config [ str ] [ "13" ].get<bool> ( );
}


template<typename T>
void ConfigSys::Save ( T &value, std::string str )
{
	config [ str ] = value;
}

void ConfigSys::SaveArray ( float_t value [ 4 ], std::string str )
{
	config [ str ] [ "0" ] = value [ 0 ];
	config [ str ] [ "1" ] = value [ 1 ];
	config [ str ] [ "2" ] = value [ 2 ];
	config [ str ] [ "3" ] = value [ 3 ];
}

void ConfigSys::SaveArray ( bool value [ 14 ], std::string str )
{
	config [ str ] [ "0" ] = value [ 0 ];
	config [ str ] [ "1" ] = value [ 1 ];
	config [ str ] [ "2" ] = value [ 2 ];
	config [ str ] [ "3" ] = value [ 3 ];
	config [ str ] [ "4" ] = value [ 4 ];
	config [ str ] [ "5" ] = value [ 5 ];
	config [ str ] [ "6" ] = value [ 6 ];
	config [ str ] [ "7" ] = value [ 7 ];
	config [ str ] [ "8" ] = value [ 8 ];
	config [ str ] [ "9" ] = value [ 9 ];
	config [ str ] [ "10" ] = value [ 10 ];
	config [ str ] [ "11" ] = value [ 11 ];
	config [ str ] [ "12" ] = value [ 12 ];
	config [ str ] [ "13" ] = value [ 13 ];
}

void ConfigSys::SaveColor ( Color col, std::string str )
{
	config [ str ] [ "0" ] = col [ 0 ];
	config [ str ] [ "1" ] = col [ 1 ];
	config [ str ] [ "2" ] = col [ 2 ];
	config [ str ] [ "3" ] = col [ 3 ];
}

void ConfigSys::LoadColor ( Color* col, std::string str )
{
	if ( config [ str ] [ "0" ].empty ( ) || config [ str ] [ "1" ].empty ( ), config [ str ] [ "2" ].empty ( ), config [ str ] [ "3" ].empty ( ) )
		return;

	int red = config [ str ] [ "0" ].get<int> ( );
	int green = config [ str ] [ "1" ].get<int> ( );
	int blue = config [ str ] [ "2" ].get<int> ( );
	int alpha = config [ str ] [ "3" ].get<int> ( );

	col->SetColor ( red, green, blue, alpha );
}