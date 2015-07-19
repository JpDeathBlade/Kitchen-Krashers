#ifndef EVENT_MANAGER_H_ 
#define EVENT_MANAGER_H_ 

#include <map>
#include <deque>
#include <string>
#include "IListener.h"
#include "EventDefinitions.h"
using std::string;
using std::multimap;
using std::deque;

class RJ_GameMain;
//EVENT MESSAGE
//STORES A BASEEVENTDATA
class CEvent
{
	EVENTTYPE	m_EventID;	
	tBaseEventData*		m_pData;	
	bool		m_bHandled;

public:
	int m_nRefs;
	//No default constructor -- pass in all data when creating an instance
	CEvent(EVENTTYPE eventID, tBaseEventData* pData = NULL)
	{
		m_EventID = eventID;
		m_pData = pData;
		m_nRefs = 0;
		m_bHandled = false;
	}
	//Empty Destructor
	~CEvent() { Trash(); }
	void Trash() 
	{
		if(m_pData) 
			delete m_pData;
		m_pData = NULL;
		//delete this;
	}

	///////////////
	//	ACCESSORS
	///////////////
	EVENTTYPE GetEventID()	{return m_EventID;}
	tBaseEventData*	GetData()		{return m_pData;}
	bool GetHandled() {return m_bHandled;}
	void SetHandled(bool bHandled) { m_bHandled = bHandled;}
};
class CEventManager
{
	//A map of all listeners currently registered and keyed for events
	multimap<EVENTTYPE, IListener*>	m_mListenersUpdate;
	multimap<EVENTTYPE, IListener*>	m_mListenersRender;

	//A vector of all events needing to be dispatched
	deque<CEvent*>					m_vINEventsUpdate;
	deque<CEvent*>					m_vINEventsRender;
	deque<CEvent*>					m_vOUTEventsUpdate;
	deque<CEvent*>					m_vOUTEventsRender;
						//Central Event Transfer Deques and bool checks.
	//This is the box where Renders out is place into and where Updates IN pulls from.
	deque<CEvent*>					m_vCentralEventUpdate;
	volatile bool					m_bCEUpdateDequeCLEAN;
	CRITICAL_SECTION				m_CritUpdateDeque;
	//This is the box where Updates out is place into and where Renders IN pulls from.
	deque<CEvent*>					m_vCentralEventRender; 
	volatile bool					m_bCERenderDequeCLEAN;
	CRITICAL_SECTION				m_CritRenderDeque;

	//SHUTTING DOWN EVENT SYSTEM
	volatile bool					m_bShutdown;
	void ShutdownEventUpdate();
	void ShutdownEventRender();

	CEventManager();
	CEventManager(const CEventManager&);
	CEventManager& operator=(const CEventManager&);
	~CEventManager() { }

	//////////////////////////////////////////////////////////////
	//	HELPER FUNCTIONS
	//////////////////////////////////////////////////////////////
	void DispatchEventUpdate(CEvent* pEvent);
	bool CheckRegisteredUpdate(EVENTTYPE eventID, IListener* pListener);
	void DispatchEventRender(CEvent* pEvent);
	bool CheckRegisteredRender(EVENTTYPE eventID, IListener* pListener);

public:
	bool Init();
	//////////////////////////////////////////////////////////////
	//	Function:	GetInstance()
	//	Purpose:	Returns a pointer to the singleton class
	//	In:
	//	Out:		A CEventSystem pointer
	//////////////////////////////////////////////////////////////
	static CEventManager* GetInstanceThreaded(void);
	//////////////////////////////////////////////////////////////
	//	Function:	RegisterListener()
	//	Purpose:	Adds a listener to the map
	//	In:			EVENTID -- the event to listen for, IListener -- a this pointer
	//	Out:		
	//////////////////////////////////////////////////////////////
	void RegisterListenerUpdate(EVENTTYPE eventID, IListener* pListener);
	void RegisterListenerRender(EVENTTYPE eventID, IListener* pListener);
	//////////////////////////////////////////////////////////////
	//	Function:	UnregisterListener()
	//	Purpose:	Unregisters a listener from the passed in eventID
	//	In:			EVENTID -- the event to stop listening for, IListener -- a this pointer
	//	Out:		
	//////////////////////////////////////////////////////////////
	void UnregisterListenerUpdate(EVENTTYPE eventID, IListener* pListener);
	void UnregisterListenerRender(EVENTTYPE eventID, IListener* pListener);
	//////////////////////////////////////////////////////////////
	//	Function:	UnregisterListenerAll()
	//	Purpose:	Unregisters a listener from all currently subscribed events
	//	In:			IListener -- a this pointer
	//	Out:		
	//////////////////////////////////////////////////////////////
	void UnregisterListenerAllUpdate(IListener* pListener);
	void UnregisterListenerAllRender(IListener* pListener);
	//////////////////////////////////////////////////////////////
	//	Function:	SendEvent()
	//	Purpose:	Sends an event to be processed
	//	In:			EVENTID -- the ID to send, pData -- any additional data listeners can use
	//	Out:		
	//////////////////////////////////////////////////////////////
	void SendEventUpdateLocal(EVENTTYPE eventID, tBaseEventData* pData = NULL);
	void SendEventRenderLocal(EVENTTYPE eventID, tBaseEventData* pData = NULL);
	void SendEventUpdateCrossToRender(EVENTTYPE eventID, tBaseEventData* pData = NULL);
	void SendEventRenderCrossToUpdate(EVENTTYPE eventID, tBaseEventData* pData = NULL);
	//////////////////////////////////////////////////////////////
	//	Function:	ProcessEvents()
	//	Purpose:	Processes all events waiting to be dispatched
	//	In:
	//	Out:		
	//////////////////////////////////////////////////////////////
	void ProcessEventsUpdate();
	void ProcessEventsRender();
	//////////////////////////////////////////////////////////////
	//	Function:	ClearEvents()
	//	Purpose:	Clears all events waiting to be dispatched
	//	In:
	//	Out:		
	//////////////////////////////////////////////////////////////
	void ClearINEventsUpdate();
	void ClearOUTEventsUpdate();
	void ClearCENTRALEventsUpdate();
	void ClearINEventsRender();
	void ClearOUTEventsRender();
	void ClearCENTRALEventsRender();
	//////////////////////////////////////////////////////////////
	//	Function:	ShutdownEventSystem()
	//	Purpose:	Unregisters all listeners and removes all events
	//				waiting to be dispatched
	//	In:
	//	Out:		
	//////////////////////////////////////////////////////////////
	void ShutdownEventManager();
	// 0===================================================================0
	// | ~ THREADED EVENT BOX TRANSFERS(TEBT) ~                                             |
	// 0=====0=======================================================0=====0
	bool TEBTtoRenderIN();
	bool TEBTtoUpdateIN();
	bool TEBTtoRenderCentral();
	bool TEBTtoUpdateCentral();
};

#endif