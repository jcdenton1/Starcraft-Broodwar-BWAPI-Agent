#pragma once

#include "ActionHandler.h"
#include "StateInfo.h"

struct FringeItem
{
	StateInfo state;
	BWAPI::Unit* unit;
	Action::ActiveActions firstAction;
};
