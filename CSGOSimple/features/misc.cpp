#include "misc.hpp"

#include "../valve_sdk/csgostructs.hpp"
#include "..\options.hpp"
#include "..\helpers\math.hpp"
#include "..\menu.hpp"
#include <mutex>

std::vector<std::string> ChatSpam =
{
	"Delicia, delicia, assim massas.pw me mata",
	"Hoje os massas.pw users vivem bem",
	"So vejo os sorrisos a baixar quando massas.pw se aproxima",
	"Indo eu, Indo eu, matar patos c\xc3\xa1 vou eu, instalei o massas.pw, ai jesus que l\xc3\xa1 vou eu!",
	"O meu nome \xc3\xa9 massas.pw user e o teu qual \xc3\xa9?",
	"\xc3\x93 Rosa, instala massas.pw, \xc3\x93 Rosa, injeta - o bem, \xc3\x93 Rosa, liga tudo, que o VAC n\xc3\xa3o detecta",
	"Alcool, cash, weed, bitch, massas.pw injetado, Ho yeah"
};

#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
void C_Misc::OnCreateMove( CUserCmd* cmd )
{
	if ( !g_LocalPlayer )
		return;

	if ( ( g_Options.misc_bhop || g_Options.misc_autosrafe ) )
		Bhop( cmd );

	if ( g_LocalPlayer && g_Options.misc_clantag )
	{
		ClanTag( );
	}

	if ( g_Options.misc_namesteal )
		NameStealer( );

	if ( g_Options.misc_autopistol )
		AutoPistol( cmd );

	if ( g_Options.misc_slidewalk )
		MemeWalk( cmd );

	if ( g_Options.misc_chatspam )
	{
		static float nextTime = 0.f;
		float flServerTime = g_LocalPlayer->m_nTickBase( ) * g_GlobalVars->interval_per_tick;

		if ( nextTime > flServerTime )
			return;

		nextTime = flServerTime + 0.5f;
		if ( g_Options.misc_chatspam == 2 )
			g_EngineClient->ClientCmd( std::string( std::string( "say " ) + g_Options.spam ).c_str( ) );
		else
			g_EngineClient->ClientCmd( std::string( std::string( "say " ) + ChatSpam[rand( ) % 7] ).c_str( ) );
	}
}

void C_Misc::SetClanTag(const char* tag, const char* name)
{

	static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(((DWORD)Utils::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx")));
	pSetClanTag(tag, name);

}

void C_Misc::SetName(const char* new_name)
{
	static auto name = g_CVar->FindVar("name");

	name->SetValue(new_name);
	*reinterpret_cast<int*>(uintptr_t(&name->m_fnChangeCallbacks) + 0xC) = 0;
}

void C_Misc::MemeWalk( CUserCmd* cmd )
{
	if ( g_LocalPlayer->m_nMoveType( ) == MOVETYPE_LADDER )
		return;

	if ( cmd->forwardmove > 0 )
	{
		cmd->buttons |= IN_BACK;
		cmd->buttons &= ~IN_FORWARD;
	}

	if ( cmd->forwardmove < 0 )
	{
		cmd->buttons |= IN_FORWARD;
		cmd->buttons &= ~IN_BACK;
	}

	if ( cmd->sidemove < 0 )
	{
		cmd->buttons |= IN_MOVERIGHT;
		cmd->buttons &= ~IN_MOVELEFT;
	}

	if ( cmd->sidemove > 0 )
	{
		cmd->buttons |= IN_MOVELEFT;
		cmd->buttons &= ~IN_MOVERIGHT;

	}

}

void C_Misc::AutoPistol(CUserCmd* cmd)
{

	if (!g_Options.misc_autopistol || Menu::Get().IsVisible() || !g_Options.CanAutoPistol || g_VGuiSurface->IsCursorVisible())
		return;

	auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
	if (!pWeapon) return;
	auto data = pWeapon.Get()->GetCSWeaponData();
	if (!pWeapon->IsPistol(data) || pWeapon->m_iClip1() == 0)
		return;

	auto isRevolver = pWeapon->m_Item().m_iItemDefinitionIndex() == WEAPON_REVOLVER;
	if (isRevolver && !GetAsyncKeyState(VK_RBUTTON) || !isRevolver && !GetAsyncKeyState(VK_LBUTTON))
		return;

	auto curtime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
	if (pWeapon->m_flNextPrimaryAttack() >= curtime)
		cmd->buttons &= isRevolver ? ~IN_ATTACK2 : ~IN_ATTACK;
	else
		cmd->buttons |= isRevolver ? IN_ATTACK2 : IN_ATTACK;
}

float get_delta(float hspeed, float maxspeed, float airaccelerate)
{
	auto term = (30.0 - (airaccelerate * maxspeed / 66.0)) / hspeed;

	if (term < 1.0f && term > -1.0f) {
		return acos(term);
	}

	return 0.f;
}

void C_Misc::NameStealer( )
{
	bool bDidMeme = false;
	int iNameIndex = -1;
	char chName[130];

	static ConVar* name = g_CVar->FindVar( "name" );

	if ( !bDidMeme )
		name->SetValue( "\n\xAD\xAD\xAD" );


	for ( int iPlayerIndex = 0; iPlayerIndex < g_EngineClient->GetMaxClients( ); iPlayerIndex++ )
	{
		C_BasePlayer *pEntity = ( C_BasePlayer* ) g_EntityList->GetClientEntity( iPlayerIndex );
		if ( !pEntity || pEntity == g_LocalPlayer || iPlayerIndex == g_EngineClient->GetLocalPlayer( ) )
			continue;
		if ( rand( ) % 3 == 1 )
		{
			iNameIndex = pEntity->EntIndex( );
			bDidMeme = true;
		}
	}
	if ( bDidMeme )
	{
		player_info_t pInfo;
		g_EngineClient->GetPlayerInfo( iNameIndex, &pInfo );
		sprintf( chName, "%s ", pInfo.szName );
		name->SetValue( chName );
	}

}

void C_Misc::Bhop(CUserCmd* cmd)
{
	auto local = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

	if (g_Options.misc_bhop)
	{

		if (cmd->buttons & IN_JUMP && !(local->m_nMoveType() & MOVETYPE_LADDER))
		{
			int iFlags = local->GetFlags();
			if (iFlags & FL_ONGROUND || iFlags & FL_PARTIALGROUND)
			{

			}
			else
			{
				cmd->buttons &= (~IN_JUMP);
			}
		}
	}

	if (g_Options.misc_autosrafe && GetAsyncKeyState(VK_SPACE))
	{
		static bool AutoStrafeFlip = false;

		cmd->forwardmove = 0.0f;
		cmd->sidemove = 0.0f;

		if (cmd->mousedx < 0.f)
		{
			cmd->sidemove = -450.0f;
		}
		else if (cmd->mousedx > 0.f)
		{
			cmd->sidemove = 450.0f;
		}
		else
		{
			if (AutoStrafeFlip)
			{
				QAngle new_ang;
				new_ang = QAngle(cmd->viewangles.pitch, cmd->viewangles.yaw - 1.0f, 0.0f);
				Math::ClampAngles(new_ang);

				cmd->viewangles = new_ang;
				cmd->sidemove = -450.0f;
				AutoStrafeFlip = false;
			}
			else
			{
				QAngle new_ang;
				new_ang = QAngle(cmd->viewangles.pitch, cmd->viewangles.yaw + 1.0f, 0.0f);
				Math::ClampAngles(new_ang);

				cmd->viewangles = new_ang;
				cmd->sidemove = 450.0f;
				AutoStrafeFlip = true;
			}
		}
	}
}

void C_Misc::ClanTag()
{
	int ratio = g_GlobalVars->curtime;

	static float nextTime = 0.f;
	float flServerTime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;

	if (nextTime > flServerTime)
		return;

	nextTime = flServerTime + 0.5f;

	switch (g_Options.misc_clantag)
	{
	case 1:
		switch (ratio % 2)
		{
		case 1:
			SetClanTag("massas.pw", "massas.pw");
			break;
		case 2:
			SetClanTag("massas.pw", "massas.pw");
			break;
		}
		break;
	case 2:
		switch (ratio % 12)
		{
		case 1: SetClanTag("massas.pw     ", "massas.pw"); break;
		case 2: SetClanTag("assas.pw     m", "massas.pw"); break;
		case 3: SetClanTag("ssas.pw     ma", "massas.pw"); break;
		case 4: SetClanTag("sas.pw     mas", "massas.pw"); break;
		case 5: SetClanTag("as.pw     mass", "massas.pw"); break;
		case 6: SetClanTag("s.pw     massa", "massas.pw"); break;
		case 7: SetClanTag(".pw     massas", "massas.pw"); break;
		case 8: SetClanTag("pw     massas.", "massas.pw"); break;
		case 9: SetClanTag("w     massas.p", "massas.pw"); break;
		case 10: SetClanTag("     massas.pw", "massas.pw"); break;
		case 11: SetClanTag("    massas.pw ", "massas.pw"); break;
		case 12: SetClanTag("   massas.pw  ", "massas.pw"); break;
		case 13: SetClanTag("  massas.pw   ", "massas.pw"); break;
		case 14: SetClanTag(" massas.pw    ", "massas.pw"); break;
		case 15: SetClanTag("massas.pw     ", "massas.pw"); break;
		}
		break;
	case 3:
		switch (ratio % 2)
		{
		case 1: SetClanTag(g_Options.tag, g_Options.tag); break;
		case 2:	SetClanTag(g_Options.tag, g_Options.tag); break;
		}
		break;
	}

}

Timer::Timer(int32_t interval)
{
	SetInterval(interval);
}

bool Timer::run(void)
{
	if (m_bFinished) {
		return true;
	}
	if (!m_StartTick) {
		m_StartTick = GetTickCount();
	}

	m_TickCount = GetTickCount();
	if (m_TickCount - m_StartTick >= m_Interval) {
		m_bFinished = true;
	}
	return m_bFinished;
}

bool Timer::finished(void) const
{
	return m_bFinished;
}

uint32_t Timer::interval(void) const
{
	return m_Interval;
}

uint32_t Timer::current(void) const
{
	return m_TickCount - m_StartTick;
}

float Timer::percentage(void) const
{
	auto p = static_cast< float >(m_TickCount - m_StartTick) / 100.f * (1.f / static_cast< float >(m_Interval)) * 10000.f;
	return p > 100.f ? 100.f : p;
}

void Timer::SetInterval(int32_t interval)
{
	m_Interval = static_cast< uint32_t >(interval);
}

void Timer::reset(void)
{
	m_bFinished = false;
	m_StartTick = 0;
}