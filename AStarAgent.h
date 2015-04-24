#pragma once

#include <list>
#include <BWAPI.h>
#include <BWTA.h>
#include "SuccessorItem.h"

class AStarAgent
{
public:
	AStarAgent();

	StateInfo getStartState();
	bool isGoalState();
	std::list<SuccessorItem> getSuccessors(const StateInfo& state);
	int getCostOfActions()
	{
		return ACTION_COST;
	}


private:
	StateInfo getStateFromAction(const StateInfo& state, Action::ActiveActions action);

	static const int ACTION_COST = 1;
};
