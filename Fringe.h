#pragma once

#include <map>
#include <list>
#include "FringeItem.h"

class Fringe
{
public:
	Fringe();

	void pushBack(unsigned int priority, const FringeItem& item);
	FringeItem popFirst();

	void empty();
	int size()
	{
		return m_size;
	}

private:
	std::map<unsigned int, std::list<FringeItem> > m_fringe;
	int m_size;
};
