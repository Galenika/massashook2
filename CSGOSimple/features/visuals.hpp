#pragma once

class C_BasePlayer;
class C_BaseEntity;
class C_BaseCombatWeapon;
class C_PlantedC4;
class Color;
class ClientClass;
class CUserCmd;

namespace Visuals
{
    namespace Player
    {
        bool Begin(C_BasePlayer* pl);

        void RenderBox();
        void RenderName();
        void RenderHealth();
        void RenderArmour();
        void RenderWeapon();
        void RenderSnapline();
		void RenderHistory();
		void RenderSkeleton();
		void RenderResolver();
    }

    namespace Misc
    {
		void RenderHitmark();
        void RenderCrosshair();
        void RenderWeapon(C_BaseCombatWeapon* ent);
		void RenderNadeESP(C_BaseCombatWeapon* ent);
        void RenderDefuseKit(C_BaseEntity* ent);
        void RenderPlantedC4(C_BasePlayer* ent);
		void ThirdPerson();
		void SpecList();
		void RenderDamage();
    }
	
	void DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...);

    bool CreateFonts();
    void DestroyFonts();
}
