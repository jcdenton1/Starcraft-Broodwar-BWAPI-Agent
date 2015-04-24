#pragma once

#include <list>

namespace BWAPI
{
	class Unit;
}

namespace Action
{
	enum ActiveActions
	{
		UNKNOWN					= -1,
		DO_NOTHING				= 0,
		MOVE_UP					= 1,
		MOVE_DOWN				= 2,
		MOVE_LEFT				= 3,
		MOVE_RIGHT				= 4,
		MOVE_UP_LEFT			= 5,
		MOVE_UP_RIGHT			= 6,
		MOVE_DOWN_LEFT			= 7,
		MOVE_DOWN_RIGHT			= 8,
		CREATE_WORKER			= 10,
		MOVE_TO_MINERAL_FIELD	= 11,
		BUILD_BASE				= 12,
		MINE_CLOSEST_MINERAL	= 13,

	};
}

class ActionHandler
{
public:
	static void fillPossibleActions(BWAPI::Unit* unit, std::list<Action::ActiveActions>& possibleActions);
	static void commitAction(BWAPI::Unit* unit, Action::ActiveActions action);
};
