#include <list>
#include <sstream>
#include <math.h>
#include "IJ_AIModule.h"
#include "ActionHandler.h"

using namespace BWAPI;

const int DEPTH_SEARCH = 2;
const int SEARCHERS = 4;
const int GATHERERS = 5;

bool analyzed;
bool analysis_just_finished;
BWTA::Region* home;
BWTA::Region* enemy_base;

void IJ_AIModule::onStart()
{
	Broodwar->sendText("Hello world!");
	Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
	// Enable some cheat flags
	//Broodwar->enableFlag(Flag::UserInput);
	// Uncomment to enable complete map information
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	//read map information into BWTA so terrain analysis can be done in another thread
	BWTA::readMap();
	analyzed=false;
	analysis_just_finished=false;

	show_bullets=false;
	show_visibility_data=false;

	if (Broodwar->isReplay())
	{
		Broodwar->printf("The following players are in this replay:");
		for(std::set<Player*>::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
		{
			if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
			{
				Broodwar->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
			}
		}
	}
	else
	{
		Broodwar->printf("The match up is %s v %s",
		Broodwar->self()->getRace().getName().c_str(),
		Broodwar->enemy()->getRace().getName().c_str());
	}

	Broodwar->setLocalSpeed(5);

	m_log = new Logger();
	m_existingMinerals = Broodwar->getStaticMinerals().size();
	m_existingGeysers = Broodwar->getStaticGeysers().size();
	m_statsCreator.create();
	m_stats = FrameStats::getInstance();
	m_basesNumSoFar = 0;
}

void IJ_AIModule::onEnd(bool isWinner)
{
  if (isWinner)
  {
    //log win to file
  }

  m_statsCreator.destroy();
  delete m_log;
}

void IJ_AIModule::onFrame()
{
	if (show_visibility_data)
		drawVisibilityData();

	if (show_bullets)
		drawBullets();

	if (Broodwar->isReplay())
		return;

	drawStats();
	//if (analyzed)
	//{
		m_stats->updateStats();
		//m_fringe.empty();
		Fringe fringe;
		// TODO: maybe create a Fringe per unit.
		FringeItem firstItem;
		firstItem.unit = NULL;
		firstItem.firstAction = Action::UNKNOWN;
		firstItem.state = m_agent.getStartState();
		fringe.pushBack(mineralsHeuristic(firstItem.state), firstItem);

		std::stringstream dbg;
		dbg << "Fringe size: " << m_fringe.size();
		//m_log->logLine(dbg.str());

		//m_log->logLine("onFrame");
		
		for (int i = 0; i < DEPTH_SEARCH; ++i)
		{
			//m_log->logLine("depth++");
			int fringeStateNum = fringe.size();
			for (int j = 0; j < fringeStateNum; ++j)
			{
				FringeItem curItem = fringe.popFirst();
				StateInfo curState = curItem.state;
				std::stringstream message;
				message << "state from Fringe: bases " << curState.basesNumber << ", searchers " << curState.searchingWorkersNumber <<
					", miners " << curState.gatheringWorkersNumber << ", total " << curState.totalWorkersNumber;
				//m_log->logLine(message.str());

				std::list<SuccessorItem> successors = m_agent.getSuccessors(curState);
				for (std::list<SuccessorItem>::iterator sucIter = successors.begin();
					sucIter != successors.end(); ++sucIter)
				{
					FringeItem item;
					if (i == 0)
					{
						item.unit = sucIter->unit;
						item.firstAction = sucIter->action;
					}
					else
					{
						item.unit = curItem.unit;
						item.firstAction = curItem.firstAction;
					}
					item.state = sucIter->state;
					std::stringstream str;
					str << "state to Fringe: bases " << item.state.basesNumber << ", searchers " << item.state.searchingWorkersNumber <<
						", miners " << item.state.gatheringWorkersNumber << ", total " << item.state.totalWorkersNumber;
					//m_log->logLine(str.str());
					fringe.pushBack(mineralsHeuristic(item.state) + i + 1, item);
				}
			}
		}
		FringeItem best = fringe.popFirst();
		/*Broodwar->printf("best: bases %d, searchers %d, miners %d, total %d, action: %d",
			best.state.basesNumber, best.state.searchingWorkersNumber, best.state.gatheringWorkersNumber, best.state.totalWorkersNumber, best.firstAction);*/
		std::stringstream message;
		message << "best: bases " << best.state.basesNumber << ", searchers " << best.state.searchingWorkersNumber <<
			", miners " << best.state.gatheringWorkersNumber << ", total " << best.state.totalWorkersNumber <<
			", action: " << best.firstAction;
		//m_log->logLine(message.str());

		ActionHandler::commitAction(best.unit, best.firstAction);
	//}

	if (analyzed)
		drawTerrainData();

	if (analysis_just_finished)
	{
		Broodwar->printf("Finished analyzing map.");
		analysis_just_finished=false;
	}
}

void IJ_AIModule::onSendText(std::string text)
{
  if (text=="/show bullets")
  {
    show_bullets = !show_bullets;
  } else if (text=="/show players")
  {
    showPlayers();
  } else if (text=="/show forces")
  {
    showForces();
  } else if (text=="/show visibility")
  {
    show_visibility_data=!show_visibility_data;
  } else if (text=="/analyze")
  {
    if (analyzed == false)
    {
      Broodwar->printf("Analyzing map... this may take a minute");
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
    }
  } else
  {
    Broodwar->printf("You typed '%s'!",text.c_str());
    Broodwar->sendText("%s",text.c_str());
  }
}

void IJ_AIModule::onReceiveText(BWAPI::Player* player, std::string text)
{
  Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void IJ_AIModule::onPlayerLeft(BWAPI::Player* player)
{
  Broodwar->sendText("%s left the game.",player->getName().c_str());
}

void IJ_AIModule::onNukeDetect(BWAPI::Position target)
{
  if (target!=Positions::Unknown)
    Broodwar->printf("Nuclear Launch Detected at (%d,%d)",target.x(),target.y());
  else
    Broodwar->printf("Nuclear Launch Detected");
}

void IJ_AIModule::onUnitDiscover(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] has been discovered at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void IJ_AIModule::onUnitEvade(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] was last accessible at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void IJ_AIModule::onUnitShow(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] has been spotted at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void IJ_AIModule::onUnitHide(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] was last seen at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void IJ_AIModule::onUnitCreate(BWAPI::Unit* unit)
{
  if (Broodwar->getFrameCount()>1)
  {
    if (!Broodwar->isReplay())
      Broodwar->sendText("A %s [%x] has been created at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
    else
    {
      /*if we are in a replay, then we will print out the build order
      (just of the buildings, not the units).*/
      if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
      {
        int seconds=Broodwar->getFrameCount()/24;
        int minutes=seconds/60;
        seconds%=60;
        Broodwar->sendText("%.2d:%.2d: %s creates a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
      }
    }
  }
}

void IJ_AIModule::onUnitDestroy(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] has been destroyed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void IJ_AIModule::onUnitMorph(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay())
    Broodwar->sendText("A %s [%x] has been morphed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
  else
  {
    /*if we are in a replay, then we will print out the build order
    (just of the buildings, not the units).*/
    if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
    {
      int seconds=Broodwar->getFrameCount()/24;
      int minutes=seconds/60;
      seconds%=60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
    }
  }
}

void IJ_AIModule::onUnitRenegade(BWAPI::Unit* unit)
{
  if (!Broodwar->isReplay())
    Broodwar->sendText("A %s [%x] is now owned by %s",unit->getType().getName().c_str(),unit,unit->getPlayer()->getName().c_str());
}

void IJ_AIModule::onSaveGame(std::string gameName)
{
  Broodwar->printf("The game was saved to \"%s\".", gameName.c_str());
}

DWORD WINAPI AnalyzeThread()
{
  BWTA::analyze();

  //self start location only available if the map has base locations
  if (BWTA::getStartLocation(BWAPI::Broodwar->self())!=NULL)
  {
    home       = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
  }
  //enemy start location only available if Complete Map Information is enabled.
  if (BWTA::getStartLocation(BWAPI::Broodwar->enemy())!=NULL)
  {
    enemy_base = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
  }
  analyzed   = true;
  analysis_just_finished = true;
	//Broodwar->printf("AnalyzeThread end");
  return 0;
}

void IJ_AIModule::drawStats()
{
  std::set<Unit*> myUnits = Broodwar->self()->getUnits();
  Broodwar->drawTextScreen(5,0,"There are %d mines:", m_existingMinerals);
  Broodwar->drawTextScreen(5,16,"The map size is %dx%d (%dx%d)",
	  Broodwar->mapWidth() * 32, Broodwar->mapHeight() * 32, Broodwar->mapWidth(), Broodwar->mapHeight());
  Broodwar->drawTextScreen(5,32,"I have %d units:",myUnits.size());
  Broodwar->drawTextScreen(5,48,"we built %d bases:",m_basesNumSoFar);
  std::map<UnitType, int> unitTypeCounts;
  for(std::set<Unit*>::iterator i=myUnits.begin();i!=myUnits.end();i++)
  {
    if (unitTypeCounts.find((*i)->getType())==unitTypeCounts.end())
    {
      unitTypeCounts.insert(std::make_pair((*i)->getType(),0));
    }
    unitTypeCounts.find((*i)->getType())->second++;
  }
  int line=4;
  for(std::map<UnitType,int>::iterator i=unitTypeCounts.begin();i!=unitTypeCounts.end();i++)
  {
    Broodwar->drawTextScreen(5,16*line,"- %d %ss",(*i).second, (*i).first.getName().c_str());
    line++;
  }
}

void IJ_AIModule::drawBullets()
{
  std::set<Bullet*> bullets = Broodwar->getBullets();
  for(std::set<Bullet*>::iterator i=bullets.begin();i!=bullets.end();i++)
  {
    Position p=(*i)->getPosition();
    double velocityX = (*i)->getVelocityX();
    double velocityY = (*i)->getVelocityY();
    if ((*i)->getPlayer()==Broodwar->self())
    {
      Broodwar->drawLineMap(p.x(),p.y(),p.x()+(int)velocityX,p.y()+(int)velocityY,Colors::Green);
      Broodwar->drawTextMap(p.x(),p.y(),"\x07%s",(*i)->getType().getName().c_str());
    }
    else
    {
      Broodwar->drawLineMap(p.x(),p.y(),p.x()+(int)velocityX,p.y()+(int)velocityY,Colors::Red);
      Broodwar->drawTextMap(p.x(),p.y(),"\x06%s",(*i)->getType().getName().c_str());
    }
  }
}

void IJ_AIModule::drawVisibilityData()
{
  for(int x=0;x<Broodwar->mapWidth();x++)
  {
    for(int y=0;y<Broodwar->mapHeight();y++)
    {
      if (Broodwar->isExplored(x,y))
      {
        if (Broodwar->isVisible(x,y))
          Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Green);
        else
          Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Blue);
      }
      else
        Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Red);
    }
  }
}

void IJ_AIModule::drawTerrainData()
{
  //we will iterate through all the base locations, and draw their outlines.
  for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
  {
    TilePosition p=(*i)->getTilePosition();
    Position c=(*i)->getPosition();

    //draw outline of center location
    Broodwar->drawBox(CoordinateType::Map,p.x()*32,p.y()*32,p.x()*32+4*32,p.y()*32+3*32,Colors::Blue,false);

    //draw a circle at each mineral patch
    for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getStaticMinerals().begin();j!=(*i)->getStaticMinerals().end();j++)
    {
      Position q=(*j)->getInitialPosition();
      Broodwar->drawCircle(CoordinateType::Map,q.x(),q.y(),30,Colors::Cyan,false);
    }

    //draw the outlines of vespene geysers
    for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getGeysers().begin();j!=(*i)->getGeysers().end();j++)
    {
      TilePosition q=(*j)->getInitialTilePosition();
      Broodwar->drawBox(CoordinateType::Map,q.x()*32,q.y()*32,q.x()*32+4*32,q.y()*32+2*32,Colors::Orange,false);
    }

    //if this is an island expansion, draw a yellow circle around the base location
    if ((*i)->isIsland())
      Broodwar->drawCircle(CoordinateType::Map,c.x(),c.y(),80,Colors::Yellow,false);
  }

  //we will iterate through all the regions and draw the polygon outline of it in green.
  for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
  {
    BWTA::Polygon p=(*r)->getPolygon();
    for(int j=0;j<(int)p.size();j++)
    {
      Position point1=p[j];
      Position point2=p[(j+1) % p.size()];
      Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Green);
    }
  }

  //we will visualize the chokepoints with red lines
  for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
  {
    for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
    {
      Position point1=(*c)->getSides().first;
      Position point2=(*c)->getSides().second;
      Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Red);
    }
  }
}

void IJ_AIModule::showPlayers()
{
  std::set<Player*> players=Broodwar->getPlayers();
  for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
  {
    Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
  }
}

void IJ_AIModule::showForces()
{
  std::set<Force*> forces=Broodwar->getForces();
  for(std::set<Force*>::iterator i=forces.begin();i!=forces.end();i++)
  {
    std::set<Player*> players=(*i)->getPlayers();
    Broodwar->printf("Force %s has the following players:",(*i)->getName().c_str());
    for(std::set<Player*>::iterator j=players.begin();j!=players.end();j++)
    {
      Broodwar->printf("  - Player [%d]: %s",(*j)->getID(),(*j)->getName().c_str());
    }
  }
}

void IJ_AIModule::onUnitComplete(BWAPI::Unit *unit)
{
  if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
    Broodwar->sendText("A %s [%x] has been completed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
  
  if (unit->getType().isResourceDepot())
  {	  
	  ++m_basesNumSoFar;
  }
}

int IJ_AIModule::mineralsHeuristic(const StateInfo& state)
{
	if (state.basesNumber > FrameStats::getInstance()->getBaseNum())
	{
		return 1;
	}
	int retVal = //2000 *(m_existingMinerals - state.basesNumber) + 
				400 *(m_existingMinerals - state.readyBasesNumber) + 
				110 * ((GATHERERS + SEARCHERS) * state.basesNumber - state.totalWorkersNumber) +
				6 * abs(GATHERERS * state.basesNumber - state.gatheringWorkersNumber) +
				2 * abs(SEARCHERS * state.basesNumber - state.searchingWorkersNumber);

	/*Broodwar->printf("state: bases %d, searchers %d, miners %d, total %d, ret val: %d",
		state.basesNumber, state.searchingWorkersNumber, state.gatheringWorkersNumber, state.totalWorkersNumber, retVal);*/
	std::stringstream message;
	message << "state: bases " << state.basesNumber << ", searchers " << state.searchingWorkersNumber <<
		", miners " << state.gatheringWorkersNumber << ", total " << state.totalWorkersNumber <<
		", ret val: " << retVal;
	//m_log->logLine(message.str());

	return retVal;
}
