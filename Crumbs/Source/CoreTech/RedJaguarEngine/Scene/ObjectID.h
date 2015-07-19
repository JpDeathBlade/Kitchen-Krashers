#ifndef _OBJECTID_H_
#define _OBJECTID_H_

#include <deque>

class ObjectID
{
public:
	static ObjectID * GetInstanceUpdate();
	std::deque<unsigned int> IDList;
	unsigned int CheckOutID();
	void ReturnID(unsigned int Id);
};

#endif