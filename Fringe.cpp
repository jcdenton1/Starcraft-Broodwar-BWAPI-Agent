#include "Fringe.h"

Fringe::Fringe()
	:	m_size(0)
{ }

void Fringe::pushBack(unsigned int priority, const FringeItem& item)
{
	m_size++;
	for (std::map<unsigned int, std::list<FringeItem> >::iterator iter = m_fringe.begin();
		iter != m_fringe.end(); ++iter)
	{
		if (iter->first == priority)
		{
			iter->second.push_back(item);
			return;
		}
	}

	std::list<FringeItem> newList;
	newList.push_back(item);
	m_fringe[priority] = newList;
}

FringeItem Fringe::popFirst()
{
	FringeItem item;
	for (std::map<unsigned int, std::list<FringeItem> >::iterator iter = m_fringe.begin();
		iter != m_fringe.end(); ++iter)
	{
		item = iter->second.front();
		iter->second.pop_front();
		if (iter->second.size() == 0)
		{
			m_fringe.erase(iter->first);
		}

		m_size--;
		return item;
	}

	// TODO: throw error, asked for non-existing priority.
	return item;
}

void Fringe::empty()
{
	for (std::map<unsigned int, std::list<FringeItem> >::iterator iter = m_fringe.begin();
		iter != m_fringe.end(); ++iter)
	{
		iter->second.empty();
	}
	m_fringe.empty();
	m_size = 0;
}
