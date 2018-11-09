#pragma once
#include "../helpers/utils.hpp"

class CLobbyScreen
{
	bool IsHost()
	{
		return this ? *reinterpret_cast<bool*>(this + 0x30) : false;
	}
};
class CMatchEventsSubscription;
class CMatchSessionOnlineHost;

class PlayerLocal
{
public:
	int GetXUIDLow()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0x8);
	}

	int GetXUIDHigh()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0xC);
	}

	const char* GetName()
	{
		typedef const char*(__thiscall* tPaster)(void*);
		return CallVFunction<tPaster>(this, 2)(this);
	}
};
class PlayerManager
{
public:
	PlayerLocal * GetLocalPlayer(int un)
	{
		typedef PlayerLocal*(__thiscall* tTo)(void*, int);
		return CallVFunction<tTo>(this, 1)(this, un);
	}
};
class CMatchSystem
{
public:
	PlayerManager * GetPlayerManager()
	{
		typedef PlayerManager*(__thiscall* tKapis)(void*);
		return CallVFunction<tKapis>(this, 0)(this);
	}
};

class CMatchFramework
{
public:
	CMatchEventsSubscription * GetEventsSubscription()
	{
		typedef CMatchEventsSubscription*(__thiscall* tGetEventsSubscription)(void*);
		return CallVFunction<tGetEventsSubscription>(this, 11)(this);
	}

	CMatchSessionOnlineHost* GetMatchSession()
	{
		typedef CMatchSessionOnlineHost*(__thiscall* tGetMatchSession)(void*);
		return CallVFunction<tGetMatchSession>(this, 13)(this);
	}

	CMatchSystem* GetMatchSystem()
	{
		typedef CMatchSystem*(__thiscall* tGetMatchSystem)(void*);
		return CallVFunction<tGetMatchSystem>(this, 15)(this);
	}

};

class CMatchSessionOnlineHost
{
public:
	// Get an internal pointer to session system-specific data
	virtual KeyValues * GetSessionSystemData() = 0;

	// Get an internal pointer to session settings
	virtual KeyValues * GetSessionSettings() = 0;

	// Update session settings, only changing keys and values need
	// to be passed and they will be updated
	virtual void UpdateSessionSettings( KeyValues *pSettings ) = 0;

	// Issue a session command
	virtual void Command( KeyValues *pCommand ) = 0;
};