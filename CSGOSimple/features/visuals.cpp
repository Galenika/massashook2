#include "visuals.hpp"

#include "../options.hpp"
#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"
#include "LagComp.h"
#include "..\options.hpp"
#include "..\helpers\Config.h"
#include "..\imgui\imgui.h"
#include "..\menu.hpp"
#include "Autowall.h"
#include "..\Resolver.h"

vgui::HFont esp_font;
vgui::HFont defuse_font;
vgui::HFont dropped_weapons_font;

// ESP Context
// This is used so that we dont have to calculate player color and position
// on each individual function over and over
struct
{
	C_BasePlayer* pl;
	bool          is_enemy;
	bool          is_visible;
	Color         clr;
	Vector        head_pos;
	Vector        feet_pos;
	RECT          bbox;
} esp_ctx;

RECT GetBBox(C_BaseEntity* ent)
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}
//--------------------------------------------------------------------------------
bool Visuals::CreateFonts()
{
	esp_font = g_VGuiSurface->CreateFont_();
	defuse_font = g_VGuiSurface->CreateFont_();
	dropped_weapons_font = g_VGuiSurface->CreateFont_();

	g_VGuiSurface->SetFontGlyphSet(esp_font, "Tahoma", 12, 800, 0, 0, FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(defuse_font, "Tahoma", 15, 700, 0, 0, FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(dropped_weapons_font, "Tahoma", 10, 700, 0, 0, FONTFLAG_DROPSHADOW);

	return true;
}
//--------------------------------------------------------------------------------
void Visuals::DestroyFonts()
{
	// Is there a way to destroy vgui fonts?
	// TODO: Find out
}
//--------------------------------------------------------------------------------
bool Visuals::Player::Begin(C_BasePlayer* pl)
{
	esp_ctx.pl = pl;
	esp_ctx.is_enemy = g_LocalPlayer->m_iTeamNum() != pl->m_iTeamNum();
	esp_ctx.is_visible = g_LocalPlayer->CanSeePlayer(pl, HITBOX_CHEST);

	if (!esp_ctx.is_enemy && g_Options.esp_enemies_only)
		return false;

	esp_ctx.clr = esp_ctx.is_enemy ? (esp_ctx.is_visible ? g_Options.color_esp_enemy_visible : g_Options.color_esp_enemy_occluded) : (esp_ctx.is_visible ? g_Options.color_esp_ally_visible : g_Options.color_esp_ally_occluded);

	if (g_Options.Players[esp_ctx.pl->EntIndex()].is_friendly)
		esp_ctx.clr = Color(30, 255, 100, 255);

	if (g_Options.esp_visible_only && !esp_ctx.is_visible)
	{
		esp_ctx.clr.SetColor(0, 0, 0, 0);
	}

	auto head = pl->GetHitboxPos(HITBOX_HEAD);
	auto origin = pl->m_vecOrigin();

	head.z += 15;

	if (!Math::WorldToScreen(head, esp_ctx.head_pos) ||
		!Math::WorldToScreen(origin, esp_ctx.feet_pos))
		return false;

	auto h = fabs(esp_ctx.head_pos.y - esp_ctx.feet_pos.y);
	auto w = h / 1.65f;

	esp_ctx.bbox.left = static_cast<long>(esp_ctx.feet_pos.x - w * 0.5f);
	esp_ctx.bbox.right = static_cast<long>(esp_ctx.bbox.left + w);
	esp_ctx.bbox.bottom = static_cast<long>(esp_ctx.feet_pos.y);
	esp_ctx.bbox.top = static_cast<long>(esp_ctx.head_pos.y);

	return true;
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderBox()
{
	float
		length_horizontal = (esp_ctx.bbox.right - esp_ctx.bbox.left) * 0.2f,
		length_vertical = (esp_ctx.bbox.bottom - esp_ctx.bbox.top) * 0.2f;

	Color col_black = Color(0, 0, 0, 200);
	if (!esp_ctx.is_visible && g_Options.esp_visible_only)
		return;

	switch (g_Options.esp_player_boxes)
	{
	case 0:
		break;

	case 1:
		g_VGuiSurface->DrawSetColor(esp_ctx.clr);
		g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left, esp_ctx.bbox.top, esp_ctx.bbox.right, esp_ctx.bbox.bottom);
		if (g_Options.esp_player_boxes_outline)
		{
			g_VGuiSurface->DrawSetColor(col_black);
			g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.top - 1, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom + 1);
			g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left + 1, esp_ctx.bbox.top + 1, esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - 1);
		}
		break;

	case 2:
		if (g_Options.esp_player_boxes_outline) {
			g_VGuiSurface->DrawSetColor(col_black);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.top - 1, esp_ctx.bbox.left + 1 + length_horizontal, esp_ctx.bbox.top + 2);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 1 - length_horizontal, esp_ctx.bbox.top - 1, esp_ctx.bbox.right + 1, esp_ctx.bbox.top + 2);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.bottom - 2, esp_ctx.bbox.left + 1 + length_horizontal, esp_ctx.bbox.bottom + 1);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 1 - length_horizontal, esp_ctx.bbox.bottom - 2, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom + 1);

			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.top + 2, esp_ctx.bbox.left + 2, esp_ctx.bbox.top + 1 + length_vertical);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 2, esp_ctx.bbox.top + 2, esp_ctx.bbox.right + 1, esp_ctx.bbox.top + 1 + length_vertical);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.bottom - 1 - length_vertical, esp_ctx.bbox.left + 2, esp_ctx.bbox.bottom - 2);
			g_VGuiSurface->DrawFilledRect(esp_ctx.bbox.right - 2, esp_ctx.bbox.bottom - 1 - length_vertical, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom - 2);
		}
		g_VGuiSurface->DrawSetColor(esp_ctx.clr);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.top, esp_ctx.bbox.left + length_horizontal - 1, esp_ctx.bbox.top);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - length_horizontal, esp_ctx.bbox.top, esp_ctx.bbox.right - 1, esp_ctx.bbox.top);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.bottom - 1, esp_ctx.bbox.left + length_horizontal - 1, esp_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - length_horizontal, esp_ctx.bbox.bottom - 1, esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - 1);

		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.top, esp_ctx.bbox.left, esp_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - 1, esp_ctx.bbox.top, esp_ctx.bbox.right - 1, esp_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.left, esp_ctx.bbox.bottom - length_vertical, esp_ctx.bbox.left, esp_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - length_vertical, esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - 1);
		break;
	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderName()
{
	wchar_t buf[128];

	player_info_t info = esp_ctx.pl->GetPlayerInfo();

	if (!esp_ctx.is_visible && g_Options.esp_visible_only)
		return;

	if (MultiByteToWideChar(CP_UTF8, 0, info.szName, -1, buf, 128) > 0) {
		int tw, th;
		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		if (g_Options.esp_player_names == 1)
			g_VGuiSurface->DrawSetTextColor(esp_ctx.clr);
		else if (g_Options.esp_player_names == 2)
			g_VGuiSurface->DrawSetTextColor(Color::White);
		g_VGuiSurface->DrawSetTextPos(esp_ctx.feet_pos.x - tw / 2, esp_ctx.head_pos.y - th);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderHealth()
{

	if (!esp_ctx.is_visible && g_Options.esp_visible_only)
		return;

	int health = esp_ctx.pl->m_iHealth();
	if (health > 100)
		health = 100;

	float box_h = (float)fabs(esp_ctx.bbox.bottom - esp_ctx.bbox.top);
	float off = 8;

	auto height = box_h - (((box_h * health) / 100));

	int x = esp_ctx.bbox.left - off;
	int y = esp_ctx.bbox.top;
	int w = 4;
	int h = box_h;

	if (g_Options.esp_player_health == 1 || g_Options.esp_player_health == 3)
	{
		Color col_black = Color(0, 0, 0, 200);
		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

		g_VGuiSurface->DrawSetColor(Color((255 - health * int(2.55f)), (health * int(2.55f)), 0, 240));
		g_VGuiSurface->DrawFilledRect(x + 1, y + height + 1, x + w - 1, y + h - 1);
	}

	if (g_Options.esp_player_health == 2 || g_Options.esp_player_health == 3)
	{

		std::string buf = std::string("HP: " + std::to_string(health));

		DrawString(esp_font, esp_ctx.bbox.right + 2, esp_ctx.bbox.top + 2, Color((255 - health * int(2.55f)), (health * int(2.55f)), 0, 240), 0, buf.c_str());
	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderArmour()
{
	auto  armour = esp_ctx.pl->m_ArmorValue();
	float box_h = (float)fabs(esp_ctx.bbox.bottom - esp_ctx.bbox.top);
	//float off = (box_h / 6.f) + 5;
	float off = 4;

	auto height = (((box_h * armour) / 100));

	int x = esp_ctx.bbox.right + off;
	int y = esp_ctx.bbox.top;
	int w = 4;
	int h = box_h;

	Color col_black = Color(0, 0, 0, 200);
	g_VGuiSurface->DrawSetColor(col_black);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

	g_VGuiSurface->DrawSetColor(Color(0, 120, 255, 255));
	g_VGuiSurface->DrawFilledRect(x + 1, y + height + 1, x + w - 1, y + h - 1);
}

void Visuals::Misc::RenderHitmark()
{

	if (g_Options.Hitmark_alpha == 1.f)
	{
		std::string fPath = std::string(ConfigSys::Get().my_documents_folder) + "\\massas.pw\\" + "hitsound.wav";
		if ( g_Options.misc_hitmark == 2 )
			PlaySoundA( ( LPCSTR ) fPath.c_str( ) , NULL , SND_FILENAME | SND_ASYNC );
		else if ( g_Options.misc_hitmark == 1 )
		{
			g_EngineClient->vClientCmd_Unrestricted( "play buttons\\arena_switch_press_02.wav" );
		}
	}

	if (g_Options.Hitmark_alpha < 0.f)
		g_Options.Hitmark_alpha = 0.f;
	else if (g_Options.Hitmark_alpha > 0.f)
		g_Options.Hitmark_alpha -= 0.01f;

	int W, H;
	g_EngineClient->GetScreenSize(W, H);

	if (g_Options.Hitmark_alpha > 0.f)
	{
		g_VGuiSurface->DrawSetColor(Color(255, 255, 255, (int)(g_Options.Hitmark_alpha * 255.f)));

		g_VGuiSurface->DrawLine(W / 2 - 10, H / 2 - 10, W / 2 - 5, H / 2 - 5);
		g_VGuiSurface->DrawLine(W / 2 - 10, H / 2 + 10, W / 2 - 5, H / 2 + 5);
		g_VGuiSurface->DrawLine(W / 2 + 10, H / 2 - 10, W / 2 + 5, H / 2 - 5);
		g_VGuiSurface->DrawLine(W / 2 + 10, H / 2 + 10, W / 2 + 5, H / 2 + 5);
	}

}

void Visuals::Player::RenderHistory()
{
	if (!esp_ctx.pl->IsAlive()) return;
	if (!g_LocalPlayer) return;

	if (g_Options.esp_history == 1)
	{

		if (g_LocalPlayer->IsAlive() && g_Options.legit_backtrack && g_Options.legit_enable)
		{
			for (int t = 0; t < g_Options.legit_backtrack; ++t)
			{
				Vector screenbacktrack[64][12];

				if (headPositions[esp_ctx.pl->EntIndex()][t].simtime && headPositions[esp_ctx.pl->EntIndex()][t].simtime + 1 > g_LocalPlayer->m_flSimulationTime())
				{
					if (Math::WorldToScreen(headPositions[esp_ctx.pl->EntIndex()][t].hitboxPos, screenbacktrack[esp_ctx.pl->EntIndex()][t]))
					{
						g_VGuiSurface->DrawSetColor(esp_ctx.clr);
						g_VGuiSurface->DrawOutlinedRect(screenbacktrack[esp_ctx.pl->EntIndex()][t].x, screenbacktrack[esp_ctx.pl->EntIndex()][t].y, screenbacktrack[esp_ctx.pl->EntIndex()][t].x + 2, screenbacktrack[esp_ctx.pl->EntIndex()][t].y + 2);

					}
				}
			}
		}
		else
		{
			memset(&headPositions[0][0], 0, sizeof(headPositions));
		}
	}
}

//--------------------------------------------------------------------------------
void Visuals::Player::RenderWeapon()
{
	wchar_t buf[80];

	if (!esp_ctx.is_visible && g_Options.esp_visible_only)
		return;

	auto weapon = esp_ctx.pl->m_hActiveWeapon().Get();

	if (!weapon) return;

	if (MultiByteToWideChar(CP_UTF8, 0, weapon->GetCSWeaponData()->weapon_name + 7, -1, buf, 80) > 0) {
		int tw, th;
		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		if (g_Options.esp_player_weapons == 1)
			g_VGuiSurface->DrawSetTextColor(esp_ctx.clr);
		else if (g_Options.esp_player_weapons == 2)
			g_VGuiSurface->DrawSetTextColor(Color::White);
		g_VGuiSurface->DrawSetTextPos(esp_ctx.feet_pos.x - tw / 2, esp_ctx.feet_pos.y);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderSnapline()
{
	int screen_w, screen_h;
	g_EngineClient->GetScreenSize(screen_w, screen_h);

	g_VGuiSurface->DrawSetColor(esp_ctx.clr);

	g_VGuiSurface->DrawLine(
		screen_w / 2,
		screen_h,
		esp_ctx.feet_pos.x,
		esp_ctx.feet_pos.y);
}
//--------------------------------------------------------------------------------
void Visuals::Misc::RenderCrosshair()
{
}
//--------------------------------------------------------------------------------
void Visuals::Misc::RenderWeapon(C_BaseCombatWeapon* ent)
{
	wchar_t buf[80];
	auto clean_item_name = [](const char* name) -> const char* {
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	// We don't want to Render weapons that are being held
	if (ent->m_hOwnerEntity().IsValid())
		return;

	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	g_VGuiSurface->DrawSetColor(g_Options.color_esp_weapons);
	if (g_Options.esp_player_boxes)
	{
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.right, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
	}

	auto name = clean_item_name(ent->GetClientClass()->m_pNetworkName);

	if (ent->m_Item().m_iItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_CZ75)
		name = "CZ-75";

	if (g_Options.esp_player_names)
	{
		if (MultiByteToWideChar(CP_UTF8, 0, name, -1, buf, 80) > 0) {
			int w = bbox.right - bbox.left;
			int tw, th;
			g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

			g_VGuiSurface->DrawSetTextFont(esp_font);
			g_VGuiSurface->DrawSetTextColor(g_Options.color_esp_weapons);
			g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
			g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
		}
	}
}

void Visuals::Misc::RenderNadeESP(C_BaseCombatWeapon* ent)
{
	const model_t* model = ent->GetModel();
	if (!model)
		return;

	studiohdr_t* hdr = g_MdlInfo->GetStudioModel(model);
	if (!hdr)
		return;

	wchar_t buf[80];
	Color entcolor;
	std::string entityName = hdr->szName, icon_character;

	switch (ent->GetClientClass()->m_ClassID)
	{
	case 9:
		if (entityName[16] == 's')
		{
			entcolor = Color(255, 255, 0, 200);
			entityName = "Flashbang";
		}
		else
		{
			entcolor = Color(255, 0, 0, 200);
			entityName = "HE Grenade";
		}
		break;
	case 134:
		entcolor = Color(170, 170, 170, 200);
		entityName = "Smoke Grenade";
		break;
	case 98:
		entcolor = Color(255, 0, 0, 200);
		entityName = "Molotov";
		break;
	case 41:
		entcolor = Color(255, 255, 0, 200);
		entityName = "Decoy Grenade";
		break;
	default:
		return;
	}

	Vector points_transformed[8];
	RECT bbox = GetBBox(ent);
	if (bbox.right == 0 || bbox.bottom == 0)
		return;


	g_VGuiSurface->DrawSetColor(entcolor);
	if (g_Options.esp_player_boxes)
	{
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.right, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
	}


	if (MultiByteToWideChar(CP_UTF8, 0, entityName.c_str(), -1, buf, 80) > 0) {
		int w = bbox.right - bbox.left;
		int tw, th;
		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(g_Options.color_esp_weapons);
		g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}

}

void AngleVectors3(const QAngle& angles, Vector* forward)
{
	float sp, sy, cp, cy;

	DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
	DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

void Visuals::Misc::RenderDamage()
{

}

//--------------------------------------------------------------------------------
void Visuals::Misc::RenderDefuseKit(C_BaseEntity* ent)
{
	if (ent->m_hOwnerEntity().IsValid())
		return;

	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	g_VGuiSurface->DrawSetColor(g_Options.color_esp_defuse);
	g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.left, bbox.top);
	g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
	g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
	g_VGuiSurface->DrawLine(bbox.right, bbox.bottom, bbox.left, bbox.bottom);

	const wchar_t* buf = L"Defuse Kit";

	int w = bbox.right - bbox.left;
	int tw, th;
	g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

	g_VGuiSurface->DrawSetTextFont(esp_font);
	g_VGuiSurface->DrawSetTextColor(esp_ctx.clr);
	g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
	g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
}
//--------------------------------------------------------------------------------

void Visuals::DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	g_VGuiSurface->GetTextSize(font, wbuf, width, height);

	if (alignment & 1)
		x -= width;
	if (alignment & 2)
		x -= width / 2;

	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextColor(r, g, b, a);
	g_VGuiSurface->DrawSetTextPos(x, y - height / 2);
	g_VGuiSurface->DrawPrintText(wbuf, wcslen(wbuf));
}

inline float CSGO_Armor(float flDamage, int ArmorValue)
{
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast<float>(ArmorValue)) {
			flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}

void Visuals::Player::RenderResolver()
{
	if (!esp_ctx.is_visible && g_Options.esp_visible_only)
		return;

	if (!esp_ctx.is_enemy) return;

	auto string = resolve_data[esp_ctx.pl->EntIndex()].mode.c_str();

	DrawString(esp_font, esp_ctx.feet_pos.x, esp_ctx.feet_pos.y + 15, Color::White, 2, string);
}

void Visuals::Player::RenderSkeleton()
{
	studiohdr_t *studioHdr = g_MdlInfo->GetStudioModel(esp_ctx.pl->GetModel());
	if (studioHdr)
	{
		static matrix3x4_t boneToWorldOut[128];
		if (esp_ctx.pl->SetupBones(boneToWorldOut, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, g_GlobalVars->curtime))
		{
			for (int i = 0; i < studioHdr->numbones; i++)
			{
				mstudiobone_t *bone = studioHdr->pBone(i);
				if (!bone || !(bone->flags & BONE_USED_BY_HITBOX) || bone->parent == -1)
					continue;

				Vector bonePos1;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[i][0][3], boneToWorldOut[i][1][3], boneToWorldOut[i][2][3]), bonePos1))
					continue;

				Vector bonePos2;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[bone->parent][0][3], boneToWorldOut[bone->parent][1][3], boneToWorldOut[bone->parent][2][3]), bonePos2))
					continue;

				g_VGuiSurface->DrawSetColor(esp_ctx.clr);
				g_VGuiSurface->DrawLine((int)bonePos1.x, (int)bonePos1.y, (int)bonePos2.x, (int)bonePos2.y);
			}
		}
	}
}

void Visuals::Misc::RenderPlantedC4(C_BasePlayer* entity)
{
	Vector vOrig; Vector vScreen;
	vOrig = entity->m_vecOrigin();
	CCSBomb* Bomb = (CCSBomb*)entity;
	if (!Bomb) return;
	float flBlow = Bomb->GetC4BlowTime();
	float lifetime = flBlow - (g_GlobalVars->interval_per_tick * g_LocalPlayer->m_nTickBase());
	float countdown = Bomb->GetC4DefuseCountDown() - (g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick);

	if (Bomb->IsBombDefused()) return;

	float flDistance = g_LocalPlayer->GetEyePos().DistTo(entity->GetEyePos());
	float a = 450.7f;
	float b = 75.68f;
	float c = 789.2f;
	float d = ((flDistance - b) / c);
	float flDamage = a * exp(-d * d) - 1;

	float damage = float((std::max)((int)ceilf(CSGO_Armor(flDamage, g_LocalPlayer->m_ArmorValue())), 0));

	char buffer[32];
	char buffer2[32];
	char buffer3[32];
	sprintf(buffer, "Bomb: %.2f", lifetime);
	sprintf(buffer2, "Damage: %.0f", damage);
	sprintf(buffer3, "Defuse: %.2f", countdown);

	if (Math::WorldToScreen(vOrig, vScreen))
	{
		if (damage >= g_LocalPlayer->m_iHealth())
		{
			Visuals::DrawString(defuse_font, vScreen.x, vScreen.y, Color(240, 30, 30), 2, buffer2);
		}
		else
		{
			Visuals::DrawString(defuse_font, vScreen.x, vScreen.y, Color(30, 240, 30), 2, buffer2);
		}

		if (Bomb->GetBombDefuser() > 0)
		{
			if (countdown > 0.01f)
			{
				if (lifetime > countdown)
				{
					Visuals::DrawString(defuse_font, vScreen.x, vScreen.y + 22, Color(30, 240, 30), 2, buffer3);
				}
				else
				{
					Visuals::DrawString(defuse_font, vScreen.x, vScreen.y + 22, Color(240, 30, 30), 2, buffer3);
				}
			}
		}

		Visuals::DrawString(defuse_font, vScreen.x, vScreen.y + 12, g_Options.color_esp_c4, 2, buffer);
	}


	int x, y;
	g_EngineClient->GetScreenSize(x, y);

	if (damage >= g_LocalPlayer->m_iHealth())
	{
		Visuals::DrawString(defuse_font, 40, y / 2 + 12, Color(240, 30, 30), 2, buffer2);
	}
	else
	{
		Visuals::DrawString(defuse_font, 40, y / 2 + 12, Color(30, 240, 30), 2, buffer2);
	}

	if (Bomb->GetBombDefuser() > 0)
	{
		if (countdown > 0.01f)
		{
			if (lifetime > countdown)
			{
				Visuals::DrawString(defuse_font, 40, y / 2 + 22, Color(30, 240, 30), 2, buffer3);
			}
			else
			{
				Visuals::DrawString(defuse_font, 40, y / 2 + 22, Color(240, 30, 30), 2, buffer3);
			}
		}
	}
	Visuals::DrawString(defuse_font, 40, y / 2, g_Options.color_esp_c4, 2, buffer);
}

std::list<int> GetObservervators(int playerId)
{
	std::list<int> list;

	if (!g_EngineClient->IsInGame())
		return list;

	C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(playerId);
	if (!player)
		return list;

	if (!player->IsAlive())
	{
		C_BasePlayer* observerTarget = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(player->m_hObserverTarget());
		if (!observerTarget)
			return list;

		player = observerTarget;
	}

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* pPlayer = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		if (!pPlayer)
			continue;

		if (pPlayer->IsDormant() || pPlayer->IsAlive())
			continue;

		C_BasePlayer* target = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(pPlayer->m_hObserverTarget());
		if (player != target)
			continue;

		list.push_back(i);
	}

	return list;
}

void Visuals::Misc::SpecList()
{

	int x, y;
	g_EngineClient->GetScreenSize(x, y);

	ImGui::Begin("SpecListemem", &g_Options.misc_speclist, ImVec2(90, 100), 0.7f, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::TextColored( ImGui::GetStyleColorVec4( ImGuiCol_MenuMain ), "Spectator List" );
	if (g_LocalPlayer && g_LocalPlayer->IsAlive())
	{
		for (int playerId : GetObservervators(g_EngineClient->GetLocalPlayer()))
		{
			C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(playerId);

			player_info_t entityInformation;
			g_EngineClient->GetPlayerInfo(playerId, &entityInformation);

			ImVec4 col = ImVec4(0, 0, 0, 0);

			if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
				col = ImVec4(0.1f, 1.f, 0.1f, 1.f);
			else
				col = ImVec4(1.f, 0.1f, 0.1f, 1.f);

			ImGui::TextColored(col, entityInformation.szName);
		}
	}
	ImGui::End();
}

//--------------------------------------------------------------------------------
void Visuals::Misc::ThirdPerson() {
	if (!g_LocalPlayer)
		return;

	if (g_Options.misc_thirdperson && g_LocalPlayer->IsAlive())
	{
		if (!g_Input->m_fCameraInThirdPerson)
		{
			g_Input->m_fCameraInThirdPerson = true;
		}

		float dist = g_Options.misc_thirdperson_dist;

		QAngle *view = g_LocalPlayer->GetVAngles();
		trace_t tr;
		Ray_t ray;

		Vector desiredCamOffset = Vector(cos(DEG2RAD(view->yaw)) * dist,
			sin(DEG2RAD(view->yaw)) * dist,
			sin(DEG2RAD(-view->pitch)) * dist
		);

		//cast a ray from the Current camera Origin to the Desired 3rd person Camera origin
		ray.Init(g_LocalPlayer->GetEyePos(), (g_LocalPlayer->GetEyePos() - desiredCamOffset));
		CTraceFilter traceFilter;
		traceFilter.pSkip = g_LocalPlayer;
		g_EngineTrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

		Vector diff = g_LocalPlayer->GetEyePos() - tr.endpos;

		float distance2D = sqrt(abs(diff.x * diff.x) + abs(diff.y * diff.y));// Pythagorean

		bool horOK = distance2D > (dist - 2.0f);
		bool vertOK = (abs(diff.z) - abs(desiredCamOffset.z) < 3.0f);

		float cameraDistance;

		if (horOK && vertOK)  // If we are clear of obstacles
		{
			cameraDistance = dist; // go ahead and set the distance to the setting
		}
		else
		{
			if (vertOK) // if the Vertical Axis is OK
			{
				cameraDistance = distance2D * 0.95f;
			}
			else// otherwise we need to move closer to not go into the floor/ceiling
			{
				cameraDistance = abs(diff.z) * 0.95f;
			}
		}
		g_Input->m_fCameraInThirdPerson = true;

		g_Input->m_vecCameraOffset.z = cameraDistance;
	}
	else
	{
		g_Input->m_fCameraInThirdPerson = false;
	}
}