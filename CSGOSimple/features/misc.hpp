#pragma once
#include "..\valve_sdk\sdk.hpp"

class C_Misc : public Singleton<C_Misc>
{
public:
	void OnCreateMove(CUserCmd* cmd);
	
	void SetClanTag(const char* tag, const char* name);
	void SetName(const char* new_name);
private:
	void Bhop(CUserCmd* cmd);
	void NameStealer();
	void ClanTag();
	void AutoPistol(CUserCmd* cmd);
	void MemeWalk( CUserCmd* cmd );
};

class Timer : public Singleton<Timer>
{
public:
	Timer(void) = default;
	explicit Timer(int32_t interval);

	/**
	* @brief      Run the timer procedure
	*
	* @return     <> bool <> finish state
	*/
	bool     run(void);

	/**
	* @brief      Return finish state
	*
	* @return     <> bool
	*/
	bool     finished(void) const;

	/**
	* @brief      Return the timer interval
	*
	* @return     <> custom interval
	*/
	uint32_t interval(void) const;

	/**
	* @brief      Return the current tick - start
	*
	* @return     <> current tick
	*/
	uint32_t current(void) const;

	/**
	* @brief      Return the time in percentage
	*
	* @return     <> percentage of current tick >= end tick
	*/
	float    percentage(void) const;

	/**
	* @brief      Set the timer interval
	*
	* @param[in]  interval  <> int <> time in ms
	*/
	void     SetInterval(int32_t interval);

	/**
	* @brief      Reset the timer
	*/
	void     reset(void);

private:
	bool     m_bFinished = false;
	uint32_t m_Interval = 0;
	uint32_t m_StartTick = 0;
	uint32_t m_TickCount = 0;
};

