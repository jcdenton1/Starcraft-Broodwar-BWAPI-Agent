#include "BWAPI.h"
#include "ActionHandler.h"
#include "FrameStats.h"

using namespace BWAPI;

const int MINERAL_DISTANCE = 80;
const int MINIMAL_DISTANCE_FOR_BASE = 700;

// ##### Helper functions #####

/// checks if a given unit can go to the given Position (x,y).
bool canMoveTo(Unit* unit, int x, int y)
{
	if (x > Broodwar->mapWidth() * TILE_SIZE || x < 0 ||
		y > Broodwar->mapHeight() * TILE_SIZE || y < 0)
	{
		return false;
	}

	BWAPI::Position newPosition(x, y);
	return unit->hasPath(newPosition);
}

/// gets the minerals which has no base near them.
/// if there are none of theme, returns all the minerals in the map.
void fillMinerals(std::set<Unit*>& minerals)
{
	for(std::set<Unit*>::iterator mineralIter = Broodwar->getStaticMinerals().begin();
		mineralIter != Broodwar->getStaticMinerals().end(); ++mineralIter)
	{
		if ((*mineralIter)->getPosition().x() / TILE_SIZE > Broodwar->mapWidth() ||
			(*mineralIter)->getPosition().y() / TILE_SIZE > Broodwar->mapHeight())
		{
			Broodwar->printf("WTF");
			continue;
		}
		std::set<Unit*>::const_iterator unitIter;
		for(unitIter = Broodwar->self()->getUnits().begin();
			unitIter != Broodwar->self()->getUnits().end(); ++unitIter)
		{
			if ((*unitIter)->getType().isResourceDepot() &&
				(*unitIter)->getDistance(*mineralIter) < MINIMAL_DISTANCE_FOR_BASE)
			{
				break;
			}
		}
		if (unitIter == Broodwar->self()->getUnits().end())
		{
			Broodwar->printf("minerals at (%d,%d)", (*mineralIter)->getPosition().x(), (*mineralIter)->getPosition().y());
			minerals.insert(*mineralIter);
		}
	}

	if (minerals.size() == 0)
	{
		minerals = Broodwar->getMinerals();
	}
	Broodwar->printf("there are %d minerals available", minerals.size());
}




// ##### Class implementation #####

void ActionHandler::fillPossibleActions(Unit* unit, std::list<Action::ActiveActions>& possibleActions)
{
	if (unit->getType().isResourceDepot() && 
		Broodwar->canMake(unit, Broodwar->self()->getRace().getWorker()) &&
		!unit->isTraining() &&
		Broodwar->self()->allUnitCount(Broodwar->self()->getRace().getWorker()) < 9 * FrameStats::getInstance()->getBaseNum())
	{
		possibleActions.push_back(Action::CREATE_WORKER);
	}
	else if (unit->getType().isWorker() && unit->isCompleted())
	{
		if (unit->isGatheringMinerals() || unit->isGatheringGas() ||
			unit->isCarryingMinerals() || unit->isCarryingGas())
		{
			//Broodwar->printf("unit mines");
			//possibleActions.push_back(Action::MINE_CLOSEST_MINERAL);
		}
		else
		{
			if (unit->isMoving() || unit->isConstructing())
			{
				return;
			}

			int distance = FrameStats::getInstance()->getDistance();
			if (Broodwar->canMake(unit, Broodwar->self()->getRace().getCenter()))
			{
				std::set<Unit*> minerals;
				fillMinerals(minerals);
				Unit* closestMineral = NULL;
				for(std::set<Unit*>::iterator mineralIter = minerals.begin();
					mineralIter != minerals.end(); ++mineralIter)
				{
					if (//(*mineralIter)->isVisible(Broodwar->self()) &&
						(closestMineral == NULL || unit->getDistance(*mineralIter) < unit->getDistance(closestMineral)))
					{
						closestMineral = *mineralIter;
					}
				}
				if (closestMineral != NULL)
				{
					if (unit->getDistance(closestMineral) < 200)
					{
						possibleActions.push_back(Action::BUILD_BASE);
						Broodwar->printf("build");
					}
					else
					{
						possibleActions.push_back(Action::MOVE_TO_MINERAL_FIELD);
					}
				}
			}
			possibleActions.push_back(Action::MINE_CLOSEST_MINERAL);
			
			if (Broodwar->self()->getStartLocation().x() < Broodwar->mapWidth() / 2)
			{
				if (canMoveTo(unit, unit->getPosition().x() + distance, unit->getPosition().y()))
				{
					possibleActions.push_back(Action::MOVE_RIGHT);
				}
				if (canMoveTo(unit, unit->getPosition().x(), unit->getPosition().y() + distance))
				{
					possibleActions.push_back(Action::MOVE_DOWN);
				}
				if (canMoveTo(unit, unit->getPosition().x(), unit->getPosition().y() - distance))
				{
					possibleActions.push_back(Action::MOVE_UP);
				}
				if (canMoveTo(unit, unit->getPosition().x() - distance, unit->getPosition().y()))
				{
					possibleActions.push_back(Action::MOVE_LEFT);
				}
				if (canMoveTo(unit, unit->getPosition().x() - distance, unit->getPosition().y() - distance))
				{
					possibleActions.push_back(Action::MOVE_UP_LEFT);
				}
				if (canMoveTo(unit, unit->getPosition().x() + distance, unit->getPosition().y() - distance))
				{
					possibleActions.push_back(Action::MOVE_UP_RIGHT);
				}
				if (canMoveTo(unit, unit->getPosition().x() - distance, unit->getPosition().y() + distance))
				{
					possibleActions.push_back(Action::MOVE_DOWN_LEFT);
				}
				if (canMoveTo(unit, unit->getPosition().x() + distance, unit->getPosition().y() + distance))
				{
					possibleActions.push_back(Action::MOVE_DOWN_RIGHT);
				}
			}
			else
			{
				if (canMoveTo(unit, unit->getPosition().x() - distance, unit->getPosition().y()))
				{
					possibleActions.push_back(Action::MOVE_LEFT);
				}
				if (canMoveTo(unit, unit->getPosition().x(), unit->getPosition().y() - distance))
				{
					possibleActions.push_back(Action::MOVE_UP);
				}
				if (canMoveTo(unit, unit->getPosition().x(), unit->getPosition().y() + distance))
				{
					possibleActions.push_back(Action::MOVE_DOWN);
				}
				if (canMoveTo(unit, unit->getPosition().x() + distance, unit->getPosition().y()))
				{
					possibleActions.push_back(Action::MOVE_RIGHT);
				}
				if (canMoveTo(unit, unit->getPosition().x() - distance, unit->getPosition().y() - distance))
				{
					possibleActions.push_back(Action::MOVE_UP_LEFT);
				}
				if (canMoveTo(unit, unit->getPosition().x() + distance, unit->getPosition().y() - distance))
				{
					possibleActions.push_back(Action::MOVE_UP_RIGHT);
				}
				if (canMoveTo(unit, unit->getPosition().x() - distance, unit->getPosition().y() + distance))
				{
					possibleActions.push_back(Action::MOVE_DOWN_LEFT);
				}
				if (canMoveTo(unit, unit->getPosition().x() + distance, unit->getPosition().y() + distance))
				{
					possibleActions.push_back(Action::MOVE_DOWN_RIGHT);
				}
			}
		}
	}
}

void ActionHandler::commitAction(Unit *unit, Action::ActiveActions action)
{
	int distance = FrameStats::getInstance()->getDistance();
	switch (action)
	{
		case Action::MOVE_UP:
		{
			BWAPI::Position newPosition(unit->getPosition().x(), unit->getPosition().y() - distance);
			unit->rightClick(newPosition);
			break;
		}
		case Action::MOVE_DOWN:
		{
			BWAPI::Position newPosition(unit->getPosition().x(), unit->getPosition().y() + distance);
			unit->rightClick(newPosition);
			break;
		}
		case Action::MOVE_LEFT:
		{
			BWAPI::Position newPosition(unit->getPosition().x() - distance, unit->getPosition().y());
			unit->rightClick(newPosition);
			break;
		}
		case Action::MOVE_RIGHT:
		{
			BWAPI::Position newPosition(unit->getPosition().x() + distance, unit->getPosition().y());
			unit->rightClick(newPosition);
			break;
		}
		case Action::MOVE_UP_LEFT:
		{
			BWAPI::Position newPosition(unit->getPosition().x() - distance, unit->getPosition().y() - distance);
			unit->rightClick(newPosition);
			break;
		}
		case Action::MOVE_UP_RIGHT:
		{
			BWAPI::Position newPosition(unit->getPosition().x() + distance, unit->getPosition().y() - distance);
			unit->rightClick(newPosition);
			break;
		}
		case Action::MOVE_DOWN_LEFT:
		{
			BWAPI::Position newPosition(unit->getPosition().x() - distance, unit->getPosition().y() + distance);
			unit->rightClick(newPosition);
			break;
		}
		case Action::MOVE_DOWN_RIGHT:
		{
			BWAPI::Position newPosition(unit->getPosition().x() + distance, unit->getPosition().y() + distance);
			unit->rightClick(newPosition);
			break;
		}
		case Action::CREATE_WORKER:
			//if this is a center, tell it to build the appropiate type of worker
			if (unit->getType().getRace()!=Races::Zerg)
			{
				unit->train(Broodwar->self()->getRace().getWorker());
			}
			else //if we are Zerg, we need to select a larva and morph it into a drone
			{
				std::set<Unit*> myLarva=unit->getLarva();
				if (myLarva.size()>0)
				{
					Unit* larva=*myLarva.begin();
					larva->morph(UnitTypes::Zerg_Drone);
				}
			}
			break;
		case Action::MOVE_TO_MINERAL_FIELD:
		{
			Broodwar->printf("unit to mineral field");
			// fill set of visible & non-based mineral fields.
			std::set<Unit*> minerals;
			fillMinerals(minerals);

			Unit* closestMineral = NULL;
			for(std::set<Unit*>::iterator mineralIter = minerals.begin();
				mineralIter != minerals.end(); ++mineralIter)
			{
				if (//(*mineralIter)->isVisible(Broodwar->self()) &&
					(closestMineral == NULL || unit->getDistance(*mineralIter) < unit->getDistance(closestMineral)))
				{
					Broodwar->printf("closestMineral is at (%d,%d)", (*mineralIter)->getPosition().x(), (*mineralIter)->getPosition().y());
					closestMineral = *mineralIter;
				}
			}
			if (closestMineral != NULL)
			{
				TilePosition nearMine((closestMineral->getPosition().x() + MINERAL_DISTANCE) / TILE_SIZE,
					(closestMineral->getPosition().y()) / TILE_SIZE);
				if (!Broodwar->canBuildHere(unit, nearMine, Broodwar->self()->getRace().getCenter()))
				{
					nearMine = TilePosition((closestMineral->getPosition().x() - MINERAL_DISTANCE) / TILE_SIZE,
						(closestMineral->getPosition().y()) / TILE_SIZE);
					if (!Broodwar->canBuildHere(unit, nearMine, Broodwar->self()->getRace().getCenter()))
					{
						nearMine = TilePosition((closestMineral->getPosition().x()) / TILE_SIZE,
							(closestMineral->getPosition().y() + MINERAL_DISTANCE) / TILE_SIZE);
						if (!Broodwar->canBuildHere(unit, nearMine, Broodwar->self()->getRace().getCenter()))
						{
							nearMine = TilePosition((closestMineral->getPosition().x()) / TILE_SIZE,
								(closestMineral->getPosition().y() - MINERAL_DISTANCE) / TILE_SIZE);
						}
					}
				}
				if (!Broodwar->canBuildHere(unit, nearMine, Broodwar->self()->getRace().getCenter()))
				{
					//Broodwar->printf("can't build here (%d,%d)", nearMine.x(), nearMine.y());
				}

				if (!unit->hasPath(Position(nearMine.x() * TILE_SIZE, nearMine.y() * TILE_SIZE)))
				{
					Broodwar->printf("has not path to (%d,%d)", nearMine.x() * TILE_SIZE, nearMine.y() * TILE_SIZE);
				}

				unit->rightClick(Position(nearMine.x() * TILE_SIZE, nearMine.y() * TILE_SIZE));
			}
			break;
		}
		case Action::BUILD_BASE:
		{
			TilePosition buildingPositionStart = TilePosition((unit->getPosition().x()) / TILE_SIZE,
				(unit->getPosition().y()) / TILE_SIZE);
			int moveScale = 1;
			int moveDir = 0;
			TilePosition buildingPosition(buildingPositionStart.x(), buildingPositionStart.y());
			while (!Broodwar->canBuildHere(unit, buildingPosition, Broodwar->self()->getRace().getCenter()) &&
				moveScale < 15)
			{
				switch (moveDir)
				{
					case 0:
						buildingPosition = TilePosition(buildingPositionStart.x() + moveScale, buildingPositionStart.y());
						break;
					case 1:
						buildingPosition = TilePosition(buildingPositionStart.x() - moveScale, buildingPositionStart.y());
						break;
					case 2:
						buildingPosition = TilePosition(buildingPositionStart.x(), buildingPositionStart.y() + moveScale);
						break;
					case 3:
						buildingPosition = TilePosition(buildingPositionStart.x(), buildingPositionStart.y() - moveScale);
						break;
					case 4:
						buildingPosition = TilePosition(buildingPositionStart.x() + moveScale, buildingPositionStart.y() + moveScale);
						break;
					case 5:
						buildingPosition = TilePosition(buildingPositionStart.x() - moveScale, buildingPositionStart.y() + moveScale);
						break;
					case 6:
						buildingPosition = TilePosition(buildingPositionStart.x() + moveScale, buildingPositionStart.y() - moveScale);
						break;
					case 7:
						buildingPosition = TilePosition(buildingPositionStart.x() - moveScale, buildingPositionStart.y() - moveScale);
						break;
				}
				moveDir++;
				if (moveDir % 8 == 0)
				{
					moveDir = 0;
					moveScale++;
				}
			}
			if (Broodwar->canBuildHere(unit, buildingPosition, Broodwar->self()->getRace().getCenter()))
			{
				Broodwar->printf("building base at (%d,%d)", buildingPosition.x() * TILE_SIZE, buildingPosition.y() * TILE_SIZE);
				if (!unit->build(buildingPosition, Broodwar->self()->getRace().getCenter()))
				{
					Broodwar->printf("Why not?!");
				}
			}
			else
			{
				Broodwar->printf("can't build here (%d,%d)", buildingPosition.x(), buildingPosition.y());
			}
			break;
		}
		case Action::MINE_CLOSEST_MINERAL:
		{
			Unit* closestMineral = NULL;
			for(std::set<Unit*>::iterator mineralIter = Broodwar->getMinerals().begin();
				mineralIter != Broodwar->getMinerals().end(); ++mineralIter)
			{
				if (closestMineral == NULL || unit->getDistance(*mineralIter) < unit->getDistance(closestMineral))
				{
					closestMineral = *mineralIter;
				}
			}
			if (closestMineral != NULL)
			{
				unit->rightClick(closestMineral);
			}
			break;
		}
		case Action::DO_NOTHING:
		default:
			break;
	}
}
