#pragma once

class FrameStats
{
public:
	static FrameStats* getInstance()
	{
		return m_instance;
	}

	void updateStats();

	int getBaseNum()
	{
		return m_basesNum;
	}
	int getGasStationsNum()
	{
		return m_gasNum;
	}
	int getWorkersNum()
	{
		return m_workersNum;
	}
	int getGatheringMineralsNum()
	{
		return m_gatheringMineralsNum;
	}
	int getGatheringGasNum()
	{
		return m_gatheringGasNum;
	}
	int getSearchingWorkersNum()
	{
		return m_searchingWorkersNum;
	}
	int getDistance()
	{
		return m_distance;
	}

private:
	friend class FrameStatsCreator;

	FrameStats();
	
	static FrameStats* m_instance;

	int m_basesNum;
	int m_gasNum;
	int m_workersNum;
	int m_gatheringMineralsNum;
	int m_gatheringGasNum;
	int m_searchingWorkersNum;
	int m_distance;
};
