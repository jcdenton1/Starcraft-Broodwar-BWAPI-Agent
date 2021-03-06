#pragma once

#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>

#include "AStarAgent.h"
#include "StateInfo.h"
#include "Fringe.h"
#include "FrameStatsCreator.h"
#include "Logger.h"

extern bool analyzed;
extern bool analysis_just_finished;
extern BWTA::Region* home;
extern BWTA::Region* enemy_base;
DWORD WINAPI AnalyzeThread();

class IJ_AIModule : public BWAPI::AIModule
{
public:
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player* player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player* player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit* unit);
	virtual void onUnitEvade(BWAPI::Unit* unit);
	virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);
	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
	virtual void onUnitMorph(BWAPI::Unit* unit);
	virtual void onUnitRenegade(BWAPI::Unit* unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit *unit);
	void drawStats(); //not part of BWAPI::AIModule
	void drawBullets();
	void drawVisibilityData();
	void drawTerrainData();
	void showPlayers();
	void showForces();
	bool show_bullets;
	bool show_visibility_data;

private:
	int mineralsHeuristic(const StateInfo& state);

	int m_existingMinerals;
	int m_existingGeysers;
	int m_basesNumSoFar;
	FrameStatsCreator m_statsCreator;
	FrameStats* m_stats;
	AStarAgent m_agent;
	Fringe m_fringe;
	Logger* m_log;
};
