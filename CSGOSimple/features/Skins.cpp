#include "Skinchanger.h"
#include "../hooks.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../options.hpp"

typedef void(*CL_FullUpdate_t) (void);
CL_FullUpdate_t CL_FullUpdate = nullptr;

template <class T>
static T* FindHudElement( const char* name )
{
	static auto pThis = *reinterpret_cast< DWORD** >( Utils::PatternScan( GetModuleHandleW( L"client_panorama.dll" ),
		"B9 ? ? ? ? E8 ? ? ? ? 85 C0 0F 84 ? ? ? ? 8D 58" ) + 1 );

	static auto find_hud_element = reinterpret_cast< DWORD( __thiscall* )( void*, const char* ) >( Utils::PatternScan( GetModuleHandleW( L"client_panorama.dll" ),
		"55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28" ) );
	return ( T* ) find_hud_element( pThis, name );
}

void C_Skins::Fullupdate( )
{
	static auto CL_FullUpdate2 = reinterpret_cast< CL_FullUpdate_t >( Utils::FindPattern( "engine.dll", reinterpret_cast< PBYTE >( "\xA1\x00\x00\x00\x00\xB9\x00\x00\x00\x00\x56\xFF\x50\x14\x8B\x34\x85" ), "x????x????xxxxxxx" ) );
	CL_FullUpdate2( );
}

static CreateClientClassFn GetWearableCreateFn()
{
	auto clazz = g_CHLClient->GetAllClasses();

	while (strcmp(clazz->m_pNetworkName, "CEconWearable"))
		clazz = clazz->m_pNext;

	return clazz->m_pCreateFn;
}
IClientNetworkable* CL_CreateDLLEntity(int iEnt, int iClass, int iSerialNum)
{
	ClientClass* pClient = g_CHLClient->GetAllClasses();
	if (!pClient)
		return false;

	while (pClient)
	{
		if (pClient->m_ClassID == iClass)
			return pClient->m_pCreateFn(iEnt, iSerialNum);
		pClient = pClient->m_pNext;
	}
	return false;
}

int GetGloveSkinByMenu(int gloveIndex, int skinIndex)
{
	if (gloveIndex == 0) // bloudhound
	{
		switch (skinIndex)
		{
		case 0:
			return 10006;
		case 1:
			return 10007;
		case 2:
			return 10008;
		case 3:
			return 10039;
		default:
			return 0;
		}
	}
	else if (gloveIndex == 1) // Sport
	{
		switch (skinIndex)
		{
		case 0:
			return 10038;
		case 1:
			return 10037;
		case 2:
			return 10018;
		case 3:
			return 10019;
		case 4:
			return 10048;
		case 5:
			return 10047;
		case 6:
			return 10045;
		case 7:
			return 10046;
		default:
			return 0;
		}
	}
	else if (gloveIndex == 2) // Driver
	{
		switch (skinIndex)
		{
		case 0:
			return 10013;
		case 1:
			return 10015;
		case 2:
			return 10016;
		case 3:
			return 10040;
		case 4:
			return 10043;
		case 5:
			return 10044;
		case 6:
			return 10041;
		case 7:
			return 10042;
		default:
			return 0;
		}
	}
	else if (gloveIndex == 3) // Wraps
	{
		switch (skinIndex)
		{
		case 0:
			return 10009;
		case 1:
			return 10010;
		case 2:
			return 10021;
		case 3:
			return 10036;
		case 4:
			return 10053;
		case 5:
			return 10054;
		case 6:
			return 10055;
		case 7:
			return 10056;
		default:
			return 0;
		}
	}
	else if (gloveIndex == 4) // Moto
	{
		switch (skinIndex)
		{
		case 0:
			return 10024;
		case 1:
			return 10026;
		case 2:
			return 10027;
		case 3:
			return 10028;
		case 4:
			return 10050;
		case 5:
			return 10051;
		case 6:
			return 10052;
		case 7:
			return 10049;

		default:
			return 0;
		}
	}
	else if (gloveIndex == 5) // Specialist
	{
		switch (skinIndex)
		{
		case 0:
			return 10030;
		case 1:
			return 10033;
		case 2:
			return 10034;
		case 3:
			return 10035;
		case 4:
			return 10061;
		case 5:
			return 10062;
		case 6:
			return 10063;
		case 7:
			return 10064;
		default:
			return 0;
		}
	}
	else if (gloveIndex == 6)
	{
		switch (skinIndex)
		{
		case 0:
			return 10057;
		case 1:
			return 10058;
		case 2:
			return 10059;
		case 3:
			return 10060;
		}
	}
	else
		return 0;
	return 0;
}

enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};

#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
void C_Skins::Glovechanger()
{
	auto local_id = g_EngineClient->GetLocalPlayer();
	if (!local_id) return;

	player_info_t localPlayerInfo;
	if (!g_EngineClient->GetPlayerInfo(local_id, &localPlayerInfo)) return;

	C_BasePlayer* local = (C_BasePlayer*)g_EntityList->GetClientEntity(local_id);
	if (!local) return;

	auto wearables = local->m_hMyWearables();
	if (!wearables) return;

	static CBaseHandle glove_handle = 0;
	auto glove = reinterpret_cast<CGloves*>(g_EntityList->GetClientEntityFromHandle(wearables[0]));

	if (!glove)
	{
		auto our_glove = reinterpret_cast<CGloves*>(g_EntityList->GetClientEntityFromHandle(glove_handle));

		if (our_glove) // Our glove still exists
		{
			wearables[0] = glove_handle;
			glove = our_glove;
		}
	}

	if (!local->IsAlive())
	{
		if (glove)
		{
			glove->SetDestroyedOnRecreateEntities();
			glove->Release();
		}
		return;
	}

	if (!glove)
	{
		int iEnt = g_EntityList->GetHighestEntityIndex() + 1;
		int iSerialNumber = RandomInt(0x0, 0xFFF);

		C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)CL_CreateDLLEntity(iEnt, (int)ClassId::ClassId_CEconWearable, iSerialNumber);
		if (pWeapon)
		{
			auto m_Index = iEnt | (iSerialNumber << 16);
			glove_handle = m_Index; 
			*(DWORD*)((DWORD)local + NetvarSys::Get().GetOffset("DT_BaseCombatCharacter", "m_hMyWearables")) = m_Index;
			glove = (CGloves*)g_EntityList->GetClientEntity(iEnt);
		}

		{
			static auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>
				(Utils::FindPattern( "client_panorama.dll", (BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1", "xxxxxxxxxxxx"));

			static const Vector new_pos = { 10000.f, 10000.f, 10000.f };

			set_abs_origin_fn(glove, new_pos);
		}
	}

	*reinterpret_cast<int*>(uintptr_t(glove) + 0x64) = -1;

	if (glove)
	{
		switch (g_Options.skins_glove_model)
		{
		case 1: //bloodhound
		{
			if (glove->m_Item().m_iItemDefinitionIndex() != 5027 || glove->m_nFallbackPaintKit() != GetGloveSkinByMenu(0, g_Options.skins_glove_skin))
			{
				glove->m_Item().m_iItemDefinitionIndex() = 5027;
				glove->m_nFallbackPaintKit() = GetGloveSkinByMenu(0, g_Options.skins_glove_skin);
				glove->m_Item().m_iEntityQuality() = 4;
				glove->m_Item().m_iItemIDHigh() = -1;
				glove->m_Item().m_iAccountID() = localPlayerInfo.userId;
				glove->SetGloveModelIndex(g_MdlInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 2: //sporty
		{
			if (glove->m_Item().m_iItemDefinitionIndex() != 5030 || glove->m_nFallbackPaintKit() != GetGloveSkinByMenu(1, g_Options.skins_glove_skin))
			{
				glove->m_Item().m_iItemDefinitionIndex() = 5030;
				glove->m_nFallbackPaintKit() = GetGloveSkinByMenu(1, g_Options.skins_glove_skin);
				glove->m_Item().m_iEntityQuality() = 4;
				glove->m_Item().m_iItemIDHigh() = -1;
				glove->m_Item().m_iAccountID() = localPlayerInfo.userId;
				glove->SetGloveModelIndex(g_MdlInfo->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 3://SLICK_GLOVES
		{
			if (glove->m_Item().m_iItemDefinitionIndex() != 5031 || glove->m_nFallbackPaintKit() != GetGloveSkinByMenu(2, g_Options.skins_glove_skin))
			{
				glove->m_Item().m_iItemDefinitionIndex() = 5031;
				glove->m_nFallbackPaintKit() = GetGloveSkinByMenu(2, g_Options.skins_glove_skin);
				glove->m_Item().m_iEntityQuality() = 4;
				glove->m_Item().m_iItemIDHigh() = -1;
				glove->m_Item().m_iAccountID() = localPlayerInfo.userId;
				glove->SetGloveModelIndex(g_MdlInfo->GetModelIndex("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 4://LEATHER_HANDWRAPS
		{
			if (glove->m_Item().m_iItemDefinitionIndex() != 5032|| glove->m_nFallbackPaintKit() != GetGloveSkinByMenu(3, g_Options.skins_glove_skin))
			{
				glove->m_Item().m_iItemDefinitionIndex() = 5032;
				glove->m_nFallbackPaintKit() = GetGloveSkinByMenu(3, g_Options.skins_glove_skin);
				glove->m_Item().m_iEntityQuality() = 4;
				glove->m_Item().m_iItemIDHigh() = -1;
				glove->m_Item().m_iAccountID() = localPlayerInfo.userId;
				glove->SetGloveModelIndex(g_MdlInfo->GetModelIndex("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 5://MOTORCYCLE_GLOVES
		{
			if (glove->m_Item().m_iItemDefinitionIndex() != 5033|| glove->m_nFallbackPaintKit() != GetGloveSkinByMenu(4, g_Options.skins_glove_skin))
			{
				glove->m_Item().m_iItemDefinitionIndex() = 5033;
				glove->m_nFallbackPaintKit() = GetGloveSkinByMenu(4, g_Options.skins_glove_skin);
				glove->m_Item().m_iEntityQuality() = 4;
				glove->m_Item().m_iItemIDHigh() = -1;
				glove->m_Item().m_iAccountID() = localPlayerInfo.userId;
				glove->SetGloveModelIndex(g_MdlInfo->GetModelIndex("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 6://SPECIALIST_GLOVES
		{
			if (glove->m_Item().m_iItemDefinitionIndex() != 5034 || glove->m_nFallbackPaintKit() != GetGloveSkinByMenu(5, g_Options.skins_glove_skin))
			{
				glove->m_Item().m_iItemDefinitionIndex() = 5034;
				glove->m_nFallbackPaintKit() = GetGloveSkinByMenu(5, g_Options.skins_glove_skin);
				glove->m_Item().m_iEntityQuality() = 4;
				glove->m_Item().m_iItemIDHigh() = -1;
				glove->m_Item().m_iAccountID() = localPlayerInfo.userId;
				glove->SetGloveModelIndex(g_MdlInfo->GetModelIndex("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		case 7://hydra
		{
			if (glove->m_Item().m_iItemDefinitionIndex() != 5035 || glove->m_nFallbackPaintKit() != GetGloveSkinByMenu(6, g_Options.skins_glove_skin))
			{
				glove->m_Item().m_iItemDefinitionIndex() = 5035;
				glove->m_nFallbackPaintKit() = GetGloveSkinByMenu(6, g_Options.skins_glove_skin);
				glove->m_Item().m_iEntityQuality() = 4;
				glove->m_Item().m_iItemIDHigh() = -1;
				glove->m_Item().m_iAccountID() = localPlayerInfo.userId;
				glove->SetGloveModelIndex(g_MdlInfo->GetModelIndex("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl"));
				glove->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
		break;
		}
	}
}


int GetCurrentKnifeModel(int currentKnife)
{
	switch (currentKnife)
	{
	case Bayonet:
		return g_MdlInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl"); //Bayo
	case Flip:
		return g_MdlInfo->GetModelIndex("models/weapons/v_knife_flip.mdl"); //Flip
	case Gut:
		return g_MdlInfo->GetModelIndex("models/weapons/v_knife_gut.mdl"); //Gut
	case Karam:
		return g_MdlInfo->GetModelIndex("models/weapons/v_knife_karam.mdl"); //Karambit
	case M9:
		return g_MdlInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl"); //M9 Bayo
	case Hunts:
		return g_MdlInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl"); //Huntsman
	case Flachion:
		return g_MdlInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl"); //Falchion
	case Bowei:
		return g_MdlInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl"); //Bowie
	case Butter:
		return g_MdlInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl"); //Butterfly
	case Shadow:
		return g_MdlInfo->GetModelIndex("models/weapons/v_knife_push.mdl"); //Daggers
	case Navja:
		return g_MdlInfo->GetModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl"); //Navja
	default:
		return 0;
	}
}

int get_lore(int knife)
{
	switch (knife)
	{
	default:
		return 0;
		break;
	case MenuKnife::Karam:
		return 561;
		break;
	case MenuKnife::Bayonet:
		return 558;
		break;
	case MenuKnife::M9:
		return 562;
		break;
	case MenuKnife::Flip:
		return 559;
		break;
	case MenuKnife::Gut:
		return 560;
		break;
	}
}

int get_autotronic(int knife)
{
	switch (knife)
	{
	default:
		return 0;
		break;
	case MenuKnife::Karam:
		return 576;
		break;
	case MenuKnife::Bayonet:
		return 573;
		break;
	case MenuKnife::M9:
		return 577;
		break;
	case MenuKnife::Flip:
		return 574;
		break;
	case MenuKnife::Gut:
		return 575;
		break;
	}
}

int get_black( int knife )
{
	switch ( knife )
	{
		default:
			return 0;
			break;
		case MenuKnife::Karam:
			return 566;
			break;
		case MenuKnife::Bayonet:
			return 563;
			break;
		case MenuKnife::M9:
			return 567;
			break;
		case MenuKnife::Flip:
			return 564;
			break;
		case MenuKnife::Gut:
			return 565;
			break;
	}
}

void C_Skins::Skinchanger()
{
	if (!g_Options.skins_enable)
		return;

	player_info_t localPlayerInfo;
	if (!g_EngineClient->GetPlayerInfo(g_EngineClient->GetLocalPlayer(), &localPlayerInfo))
		return;

	auto local_id = g_EngineClient->GetLocalPlayer();
	if (!local_id) return;
	C_BasePlayer* localplayer = (C_BasePlayer*)g_EntityList->GetClientEntity(local_id);
	if (!localplayer) return;

	int* hweapons = localplayer->GetWeapons();
	if (!hweapons)
		return;

	for (int i = 0; hweapons[i] != INVALID_EHANDLE_INDEX; i++)
	{
		C_BaseEntity* Weapon = (C_BaseEntity*)g_EntityList->GetClientEntity(hweapons[i] & 0xFFF);

		if (!Weapon)
			continue;

		C_BaseCombatWeapon* pWeapon = ( C_BaseCombatWeapon* ) Weapon;

		if ( !pWeapon )
			continue;

		if ( pWeapon->m_Item( ).m_iItemDefinitionIndex( ) == ItemDefinitionIndex::WEAPON_TASER )
			continue;

		if (localPlayerInfo.xuid_low != pWeapon->m_OriginalOwnerXuidLow())
			continue;

		if (localPlayerInfo.xuid_high != pWeapon->m_OriginalOwnerXuidHigh())
			continue;

		if (pWeapon->IsKnife(pWeapon->GetCSWeaponData()))
		{

			switch (g_Options.skins_knife_model)
			{
			case MenuKnife::Bayonet:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::WEAPON_KNIFE_BAYONET;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[ItemDefinitionIndex::WEAPON_KNIFE_BAYONET].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Bowei:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::WEAPON_KNIFE_BOWIE;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[ItemDefinitionIndex::WEAPON_KNIFE_BOWIE].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Butter:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Flachion:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::WEAPON_KNIFE_FALCHION;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[ItemDefinitionIndex::WEAPON_KNIFE_FALCHION].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Flip:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::WEAPON_KNIFE_FLIP;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[ItemDefinitionIndex::WEAPON_KNIFE_FLIP].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Gut:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::WEAPON_KNIFE_GUT;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[ItemDefinitionIndex::WEAPON_KNIFE_GUT].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Hunts:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::WEAPON_KNIFE_HUNTSMAN;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[ItemDefinitionIndex::WEAPON_KNIFE_HUNTSMAN].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Karam:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::M9:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::WEAPON_KNIFE_M9BAYONET;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[ItemDefinitionIndex::WEAPON_KNIFE_M9BAYONET].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Shadow:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::WEAPON_KNIFE_DAGGER;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[WEAPON_KNIFE_DAGGER].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Navja:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::weapon_knife_gypsy_jackknife;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[weapon_knife_gypsy_jackknife].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Stiletto:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::weapon_knife_stiletto;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[weapon_knife_stiletto].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Ursus:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::weapon_knife_ursus;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_ursus.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_ursus.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[weapon_knife_ursus].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			case MenuKnife::Talon:
				pWeapon->m_Item().m_iItemDefinitionIndex() = ItemDefinitionIndex::weapon_knife_widowmaker;
				pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_widowmaker.mdl");
				pWeapon->m_iViewModelIndex() = g_MdlInfo->GetModelIndex("models/weapons/v_knife_widowmaker.mdl");
				pWeapon->m_nFallbackPaintKit() = g_Options.W[weapon_knife_widowmaker].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
				break;
			}

			int pk = pWeapon->m_nFallbackPaintKit();
			int index = pWeapon->m_Item().m_iItemDefinitionIndex();

			if (g_Options.skins_knife_model)
			{
				pWeapon->m_Item().m_iEntityQuality() = 3;
				if ( pk == 1 )
					pWeapon->m_nFallbackPaintKit( ) = get_lore( g_Options.skins_knife_model );
				else if ( pk == 2 )
					pWeapon->m_nFallbackPaintKit( ) = get_autotronic( g_Options.skins_knife_model );
				else if ( pk == 582 && index == WEAPON_KNIFE_M9BAYONET )
					pWeapon->m_nFallbackPaintKit( ) = 581;
				else if ( pk == 578 && index == WEAPON_KNIFE_M9BAYONET )
					pWeapon->m_nFallbackPaintKit( ) = 579;
				else if ( pk == 3 )
					pWeapon->m_nFallbackPaintKit( ) = get_black( g_Options.skins_knife_model );
			}
		}
		else
		{
			int i = pWeapon->m_Item().m_iItemDefinitionIndex();

			if (g_Options.W[i].ChangerSkin)
			{
				pWeapon->m_nFallbackPaintKit() = g_Options.W[i].ChangerSkin;
				pWeapon->m_flFallbackWear() = 0.001f;
				pWeapon->m_Item().m_iItemIDHigh() = -1;
			}

		}

	}

}