#include "AStarAgent.h"
#include "FrameStats.h"

using namespace BWAPI;

AStarAgent::AStarAgent()
{ }

StateInfo AStarAgent::getStartState()
{
	FrameStats* stats = FrameStats::getInstance();
	StateInfo startingState;
	startingState.readyBasesNumber = stats->getBaseNum();
	startingState.basesNumber = stats->getBaseNum();
	startingState.gatheringWorkersNumber = stats->getGatheringMineralsNum();
	startingState.searchingWorkersNumber = stats->getSearchingWorkersNum();
	startingState.totalWorkersNumber = stats->getWorkersNum();
	startingState.lastAction = Action::UNKNOWN;
	/*Broodwar->printf("start state: bases %d, searchers %d, miners %d, total %d",
		startingState.basesNumber, startingState.searchingWorkersNumber, startingState.gatheringWorkersNumber, startingState.totalWorkersNumber);*/
	return startingState;
}

bool AStarAgent::isGoalState()
{
	// TODO: if number of bases equals to existing number of bases.
	return false;
}

std::list<SuccessorItem> AStarAgent::getSuccessors(const StateInfo& state)
{
	std::list<SuccessorItem> successors;
	for(std::set<Unit*>::const_iterator unitIter = Broodwar->self()->getUnits().begin();
		unitIter!=Broodwar->self()->getUnits().end(); ++unitIter)
	{
		std::list<Action::ActiveActions> possibleActions;
		ActionHandler::fillPossibleActions(*unitIter, possibleActions);
		for (std::list<Action::ActiveActions>::iterator actionIter = possibleActions.begin();
			actionIter != possibleActions.end(); ++actionIter)
		{
			SuccessorItem successor;
			successor.unit = *unitIter;
			successor.action = *actionIter;
			successor.actionCost = ACTION_COST;
			successor.state = getStateFromAction(state, *actionIter);
			successors.push_back(successor);
		}
		/*Broodwar->printf("possible actions:");
		for (std::list<Action::ActiveActions>::iterator actionIter = possibleActions.begin();
			actionIter != possibleActions.end(); ++actionIter)
		{
			Broodwar->printf("unit: %s, action: %d", (*unitIter)->getType().c_str(), *actionIter);
		}*/		
	}
	return successors;
}

StateInfo AStarAgent::getStateFromAction(const StateInfo& state, Action::ActiveActions action)
{
	/*Broodwar->printf("state: bases %d, searchers %d, miners %d, total %d, action: %d",
		state.basesNumber, state.searchingWorkersNumber, state.gatheringWorkersNumber, state.totalWorkersNumber, action);*/
	StateInfo newState(state);
	newState.lastAction = action;
	/*if (state.lastAction == Action::UNKNOWN)
	{
		return newState;
	}*/

	switch (action)
	{
		case Action::MOVE_UP:
		case Action::MOVE_DOWN:
		case Action::MOVE_LEFT:
		case Action::MOVE_RIGHT:
		case Action::MOVE_UP_LEFT:
		case Action::MOVE_UP_RIGHT:
		case Action::MOVE_DOWN_LEFT:
		case Action::MOVE_DOWN_RIGHT:
			if ((state.lastAction == Action::MINE_CLOSEST_MINERAL ||
				state.lastAction == Action::BUILD_BASE) &&
				state.gatheringWorkersNumber > 0)
			{
				newState.searchingWorkersNumber++; // TODO: need to check if the previous action was different.
				if (state.lastAction == Action::MINE_CLOSEST_MINERAL)
				{
					newState.gatheringWorkersNumber--;
				}
			}
			break;
		case Action::CREATE_WORKER:
			newState.totalWorkersNumber++;
			//Broodwar->printf("worker++");
			break;
		case Action::MOVE_TO_MINERAL_FIELD:
			newState.readyBasesNumber++;
			break;
		case Action::BUILD_BASE:
			newState.basesNumber++;
			//Broodwar->printf("base++");
			break;
		case Action::MINE_CLOSEST_MINERAL:
			if (state.lastAction != Action::MINE_CLOSEST_MINERAL &&
				state.searchingWorkersNumber > 0)
			{
				newState.gatheringWorkersNumber++; // TODO: need to check if the previous action was different.
				if (state.lastAction == Action::MOVE_UP ||
					state.lastAction == Action::MOVE_DOWN ||
					state.lastAction == Action::MOVE_LEFT ||
					state.lastAction == Action::MOVE_RIGHT ||
					state.lastAction == Action::MOVE_UP_LEFT ||
					state.lastAction == Action::MOVE_UP_RIGHT ||
					state.lastAction == Action::MOVE_DOWN_LEFT ||
					state.lastAction == Action::MOVE_DOWN_RIGHT ||
					state.lastAction == Action::UNKNOWN)
				{
					newState.searchingWorkersNumber--;
				}
			}
			break;
		case Action::DO_NOTHING:
		default:
			break;
	}

	return newState;
}
