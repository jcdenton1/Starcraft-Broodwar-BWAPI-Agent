#pragma once

#include "ActionHandler.h"

struct StateInfo
{
	StateInfo() {}
	StateInfo(const StateInfo& other)
	{
		readyBasesNumber = other.readyBasesNumber;
		basesNumber = other.basesNumber;
		gatheringWorkersNumber = other.gatheringWorkersNumber;
		searchingWorkersNumber = other.searchingWorkersNumber;
		totalWorkersNumber = other.totalWorkersNumber;
		lastAction = other.lastAction;
	}

	int readyBasesNumber;
	int basesNumber;
	int gatheringWorkersNumber;			// out of 6
	int searchingWorkersNumber;			// out of 4
	int totalWorkersNumber;				// out of 10
	Action::ActiveActions lastAction;
};
