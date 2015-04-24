#pragma once

#include "StateInfo.h"
#include "ActionHandler.h"

struct SuccessorItem
{
	BWAPI::Unit* unit;
	Action::ActiveActions action;
	StateInfo state;
	int actionCost;
};
