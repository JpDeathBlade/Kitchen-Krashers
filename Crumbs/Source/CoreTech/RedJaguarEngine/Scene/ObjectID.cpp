#include "ObjectID.h"


ObjectID * ObjectID::GetInstanceUpdate()
{
	static ObjectID state;
	return &state;
}
unsigned int ObjectID::CheckOutID()
{
	if(IDList.size())
	{
		unsigned int count = 0;
		for(std::deque<unsigned int>::iterator itr = IDList.begin(); itr != IDList.end(); itr++)
		{
			if(*itr != count)
			{
				IDList.insert(itr,count);
				return count;
			}
			++count;
		}
	}
	else
	{
		IDList.push_back(0);
		return 0;
	}
	unsigned int AddEnd = IDList.size();
	IDList.push_back(AddEnd);
	return AddEnd;
}
void ObjectID::ReturnID(unsigned int Id)
{
	if(!IDList.empty())
	{
		for(std::deque<unsigned int>::iterator itr = IDList.begin(); itr != IDList.end(); itr++)
		{
			if(*itr == Id)
			{
				IDList.erase(itr);
				return;
			}
			if(IDList.empty())
				return;
		}
	}
}