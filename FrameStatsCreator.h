#pragma once

#include "FrameStats.h"

class FrameStatsCreator
{
public:
	void create()
	{
		stats = new FrameStats();
	}

	void destroy()
	{
		delete FrameStats::m_instance;
	}

private:
	FrameStats* stats;
};
