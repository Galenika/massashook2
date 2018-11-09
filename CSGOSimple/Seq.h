#pragma once
#include "singleton.hpp"
#include "valve_sdk/sdk.hpp"

class Security : public Singleton<Security>
{
private:
	bool bCheckCommandline();
	bool bCheckDomain();
public:
	bool bRunAllChecks();
};