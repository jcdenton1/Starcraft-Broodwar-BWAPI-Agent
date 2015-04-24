#include <stdlib.h>
#include "BWAPI.h"
#include "FrameStats.h"

using namespace BWAPI;

FrameStats* FrameStats::m_instance = NULL;

FrameStats::FrameStats()
{
	m_instance = this;
}

void FrameStats::updateStats()
{
	m_basesNum = m_gasNum = m_gatheringMineralsNum = m_gatheringGasNum = m_workersNum = 0;

	for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		if ((*i)->getType().isResourceDepot() && (*i)->isCompleted())
		{
			++m_basesNum;
		}
		else if ((*i)->getType().isResourceContainer() && !(*i)->getType().isMineralField())
		{
			++m_gasNum;
		}
		else if ((*i)->getType().isWorker())// && (*i)->isCompleted())
		{
			++m_workersNum;
			if ((*i)->isGatheringMinerals())
			{
				++m_gatheringMineralsNum;
			}
			else if ((*i)->isGatheringGas())
			{
				++m_gatheringGasNum;
			}
		}
	}

	m_searchingWorkersNum = m_workersNum - m_gatheringGasNum - m_gatheringMineralsNum;
	m_distance = rand() % 1200 + 300;
	/*Broodwar->printf("bases %d, searchers %d, miners %d, total %d",
		m_basesNum, m_searchingWorkersNum, m_gatheringMineralsNum, m_workersNum);*/
}
