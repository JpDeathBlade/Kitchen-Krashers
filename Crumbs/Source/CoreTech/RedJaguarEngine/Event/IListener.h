#ifndef ILISTENER_H_ 
#define ILISTENER_H_ 

class CEvent;

class IListener
{
public:
	IListener() { }
	virtual ~IListener() { }

	//All event-listening objects MUST HAVE THIS FUNCTION!
	//This function will be used to have it handle an event
	virtual void HandleEvent(CEvent* pEvent) = 0;
};
#endif