#include "EventManager.h"
#include "../../RJ_GameMain.h"


CEventManager::CEventManager() 
{
	m_bShutdown = 0; 
	m_bCEUpdateDequeCLEAN = 1; 
	m_bCERenderDequeCLEAN = 1;
}
CEventManager* CEventManager::GetInstanceThreaded(void)
{
	static CEventManager instance;
	return &instance;
}
bool CEventManager::Init()
{
	m_bShutdown = false; 
	m_bCEUpdateDequeCLEAN = true; 
	m_bCERenderDequeCLEAN = true;
	InitializeCriticalSection(&m_CritUpdateDeque);
	InitializeCriticalSection(&m_CritRenderDeque);
	return true;
}
void CEventManager::RegisterListenerUpdate(EVENTTYPE eventID, IListener *pListener)
{
	//Error check to validate that a valid pointer is passed in and that
	//	the listener hasn't already registered for the event(IF APPLICABLE)
	if(!pListener || CheckRegisteredUpdate(eventID, pListener))
		return;

	//Insert the listener / event into the hash table
	m_mListenersUpdate.insert(std::make_pair(eventID, pListener));
}
void CEventManager::RegisterListenerRender(EVENTTYPE eventID, IListener *pListener)
{
	if(!pListener || CheckRegisteredRender(eventID, pListener))
		return;
	m_mListenersRender.insert(std::make_pair(eventID, pListener));
}

void CEventManager::UnregisterListenerUpdate(EVENTTYPE eventID, IListener *pListener)
{
	//Create an iterator to iterate through all listeners registered for the event
	std::pair<multimap<EVENTTYPE, IListener*>::iterator, multimap<EVENTTYPE, IListener*>::iterator> range;

	//Locate all listeners registered for the event
	range = m_mListenersUpdate.equal_range(eventID);

	//Iterate through all listeners registered for the event
	for(multimap<EVENTTYPE, IListener*>::iterator theIterator = range.first; theIterator != range.second; theIterator++)
	{
		//If it locates the listener, remove it and break out of the loop
		if(theIterator->second == pListener)
		{
			theIterator = m_mListenersUpdate.erase(theIterator);
			break;
		}
	}
}
void CEventManager::UnregisterListenerRender(EVENTTYPE eventID, IListener *pListener)
{
	std::pair<multimap<EVENTTYPE, IListener*>::iterator, multimap<EVENTTYPE, IListener*>::iterator> range;
	range = m_mListenersRender.equal_range(eventID);
	for(multimap<EVENTTYPE, IListener*>::iterator theIterator = range.first; theIterator != range.second; theIterator++)
	{
		if(theIterator->second == pListener)
		{
			theIterator = m_mListenersRender.erase(theIterator);
			break;
		}
	}
}

void CEventManager::UnregisterListenerAllUpdate(IListener *pListener)
{
	//Create an iterator to loop through the map
	multimap<EVENTTYPE, IListener*>::iterator theIterator = m_mListenersUpdate.begin();
	//While the iterator isn't at the end of the map
	while(theIterator != m_mListenersUpdate.end())
	{
		//If it finds the listener, erase it from the list and update the iterator
		if(theIterator->second == pListener)
		{
			theIterator = m_mListenersUpdate.erase(theIterator);
		}
		//Else, increment the iterator
		else
			theIterator++;
	}
}
void CEventManager::UnregisterListenerAllRender(IListener *pListener)
{
	multimap<EVENTTYPE, IListener*>::iterator theIterator = m_mListenersRender.begin();
	while(theIterator != m_mListenersRender.end())
	{
		if(theIterator->second == pListener)
		{
			theIterator = m_mListenersRender.erase(theIterator);
		}
		else
			theIterator++;
	}
}

void CEventManager::SendEventUpdateCrossToRender(EVENTTYPE eventID, tBaseEventData *pData)
{
	//Create an event and add it to the vector
	CEvent* theEvent = new CEvent(eventID, pData);
	m_vOUTEventsUpdate.push_back(theEvent);
}
void CEventManager::SendEventRenderCrossToUpdate(EVENTTYPE eventID, tBaseEventData *pData)
{
	CEvent* theEvent = new CEvent(eventID, pData);
	m_vOUTEventsRender.push_back(theEvent);
}
void CEventManager::SendEventUpdateLocal(EVENTTYPE eventID, tBaseEventData *pData)
{
	CEvent* theEvent = new CEvent(eventID, pData);
	m_vINEventsUpdate.push_back(theEvent);
}
void CEventManager::SendEventRenderLocal(EVENTTYPE eventID, tBaseEventData *pData)
{
	CEvent* theEvent = new CEvent(eventID, pData);
	m_vINEventsRender.push_back(theEvent);
}

void CEventManager::ProcessEventsUpdate()
{
	//Loop through the vector of events and dispatch them
	//	then remove them from the vector
	CEvent* ListEnd = NULL;
	while(m_vINEventsUpdate.size())
	{
		if(m_vINEventsUpdate.front() == ListEnd)
			break;
		CEvent* CurrEvent = m_vINEventsUpdate.front();
		DispatchEventUpdate(CurrEvent);
		if(CurrEvent->m_nRefs == 0 && CurrEvent->GetHandled())
		{
			m_vINEventsUpdate.pop_front();
			CurrEvent->Trash();
			delete CurrEvent;
			CurrEvent = NULL;
			continue;
		}
		else if(!CurrEvent->GetHandled())
		{
			if(ListEnd == NULL)
				ListEnd = m_vINEventsUpdate.front();
			m_vINEventsUpdate.pop_front();
			m_vINEventsUpdate.push_back(CurrEvent);
			continue;
		}
		m_vINEventsUpdate.pop_front();
	}
}
void CEventManager::ProcessEventsRender()
{
	CEvent* ListEnd = NULL;
	while(m_vINEventsRender.size())
	{
		if(m_vINEventsRender.front() == ListEnd)
			break;
		CEvent* CurrEvent = m_vINEventsRender.front();
		DispatchEventRender(CurrEvent);
		if(CurrEvent->m_nRefs == 0 && CurrEvent->GetHandled())
		{
			m_vINEventsRender.pop_front();
			CurrEvent->Trash();
			delete CurrEvent;
			CurrEvent = NULL;
			continue;
		}
		else if(!CurrEvent->GetHandled())
		{
			if(ListEnd == NULL)
				ListEnd = m_vINEventsRender.front();
			m_vINEventsRender.pop_front();
			m_vINEventsRender.push_back(CurrEvent);
			continue;
		}
		m_vINEventsRender.pop_front();
	}
}
void CEventManager::ClearINEventsUpdate()
{
	//Simply clear out the vector of events
	while(!m_vINEventsUpdate.empty())
	{
		CEvent* CEDIE = m_vINEventsUpdate.front();
		m_vINEventsUpdate.pop_front();
		CEDIE->Trash();
		delete CEDIE;
		CEDIE = NULL;
	}
	m_vINEventsUpdate.clear();
}
void CEventManager::ClearOUTEventsUpdate()
{
	while(!m_vOUTEventsUpdate.empty())
	{
		CEvent* CEDIE = m_vOUTEventsUpdate.front();
		m_vOUTEventsUpdate.pop_front();
		CEDIE->Trash();
		delete CEDIE;
		CEDIE = NULL;
	}
	m_vOUTEventsUpdate.clear();
}
void CEventManager::ClearCENTRALEventsUpdate()
{
	while(!m_vCentralEventUpdate.empty())
	{
		CEvent* CEDIE = m_vCentralEventUpdate.front();
		m_vCentralEventUpdate.pop_front();
		CEDIE->Trash();
		delete CEDIE;
		CEDIE = NULL;
	}
	m_vCentralEventUpdate.clear();
}
void CEventManager::ClearINEventsRender()
{
	while(!m_vINEventsRender.empty())
	{
		CEvent* CEDIE = m_vINEventsRender.front();
		m_vINEventsRender.pop_front();
		CEDIE->Trash();
		delete CEDIE;
		CEDIE = NULL;
	}
	m_vINEventsRender.clear();
}
void CEventManager::ClearOUTEventsRender()
{
	while(!m_vOUTEventsRender.empty())
	{
		CEvent* CEDIE = m_vOUTEventsRender.front();
		m_vOUTEventsRender.pop_front();
		CEDIE->Trash();
		delete CEDIE;
		CEDIE = NULL;
	}
	m_vOUTEventsRender.clear();
}
void CEventManager::ClearCENTRALEventsRender()
{
	while(!m_vCentralEventRender.empty())
	{
		CEvent* CEDIE = m_vCentralEventRender.front();
		m_vCentralEventRender.pop_front();
		CEDIE->Trash();
		delete CEDIE;
		CEDIE = NULL;
	}
	m_vCentralEventRender.clear();
}
void CEventManager::ShutdownEventManager()
{
	m_bShutdown = true;
	ShutdownEventUpdate();
	ShutdownEventRender();
}
void CEventManager::ShutdownEventUpdate()
{
	// Clean up any remaining events and prepare for shutdown.
	if(m_bShutdown)
	{
		m_mListenersUpdate.clear();
		ClearOUTEventsUpdate();
		ClearINEventsUpdate();
		EnterCriticalSection(&m_CritUpdateDeque);
		while(m_bCEUpdateDequeCLEAN == false)
		{
			//SPIN SLEEP WHILE DIRTY
			//Sleep( 1 ); 
		}
		if(m_bCEUpdateDequeCLEAN == true)
		{
			m_bCEUpdateDequeCLEAN = false;
			ClearCENTRALEventsUpdate();
		}
		m_bCEUpdateDequeCLEAN = true;
		LeaveCriticalSection(&m_CritUpdateDeque);
		DeleteCriticalSection(&m_CritUpdateDeque);
	}
}
void CEventManager::ShutdownEventRender()
{
	if(m_bShutdown)
	{
		m_mListenersRender.clear();
		ClearOUTEventsRender();
		ClearINEventsRender();
		EnterCriticalSection(&m_CritRenderDeque);
		while(m_bCERenderDequeCLEAN == false)
		{
			//SPIN SLEEP WHILE DIRTY
			//Sleep( 1 ); 
		}
		if(m_bCERenderDequeCLEAN == true)
		{
			m_bCERenderDequeCLEAN = false;
			ClearCENTRALEventsRender();
		}
		m_bCERenderDequeCLEAN = true;
		LeaveCriticalSection(&m_CritRenderDeque);
		DeleteCriticalSection(&m_CritRenderDeque);
	}
}
void CEventManager::DispatchEventUpdate(CEvent *pEvent)
{
	//Create an iterator to iterate through all listeners registered for the event
	std::pair<multimap<EVENTTYPE, IListener*>::iterator, multimap<EVENTTYPE, IListener*>::iterator> range;

	//Find all listeners registered to the event
	range = m_mListenersUpdate.equal_range(pEvent->GetEventID());

	//Iterate through all listeners registered and have them handle the event
	if(!m_mListenersUpdate.empty())
	{
		for(multimap<EVENTTYPE, IListener*>::iterator theIterator = range.first; theIterator != range.second; theIterator++)
		{
			pEvent->m_nRefs++;
			theIterator->second->HandleEvent(pEvent);
			pEvent->SetHandled(true);
		}
	}
}
void CEventManager::DispatchEventRender(CEvent *pEvent)
{
	std::pair<multimap<EVENTTYPE, IListener*>::iterator, multimap<EVENTTYPE, IListener*>::iterator> range;

	range = m_mListenersRender.equal_range(pEvent->GetEventID());
	if(!m_mListenersRender.empty())
	{
		for(multimap<EVENTTYPE, IListener*>::iterator theIterator = range.first; theIterator != range.second; theIterator++)
		{
			pEvent->m_nRefs++;
			theIterator->second->HandleEvent(pEvent);
			pEvent->SetHandled(true);
		}
	}
}
bool CEventManager::CheckRegisteredUpdate(EVENTTYPE eventID, IListener *pListener)
{
	bool isRegistered = false;

	//Create an iterator to iterate through the map
	std::pair<multimap<EVENTTYPE, IListener*>::iterator, multimap<EVENTTYPE, IListener*>::iterator> range;

	//Find all listeners registered to the event
	range = m_mListenersUpdate.equal_range(eventID);

	//Iterate through the list of listeners registered for the event
	for(multimap<EVENTTYPE, IListener*>::iterator theIterator = range.first; theIterator != range.second; theIterator++)
	{
		//If we find the listener, flag the bool and break out
		if(theIterator->second == pListener)
		{
			isRegistered = true;
			break;
		}
	}

	return isRegistered;
}
bool CEventManager::CheckRegisteredRender(EVENTTYPE eventID, IListener *pListener)
{
	bool isRegistered = false;
	std::pair<multimap<EVENTTYPE, IListener*>::iterator, multimap<EVENTTYPE, IListener*>::iterator> range;
	range = m_mListenersRender.equal_range(eventID);

	for(multimap<EVENTTYPE, IListener*>::iterator theIterator = range.first; theIterator != range.second; theIterator++)
	{
		if(theIterator->second == pListener)
		{
			isRegistered = true;
			break;
		}
	}

	return isRegistered;
}
// MUST BE CALLED FROM RENDER SIDE
bool CEventManager::TEBTtoRenderIN()
{
	EnterCriticalSection(&m_CritRenderDeque);
	while(m_bCERenderDequeCLEAN == false)
	{
		//SPIN SLEEP WHILE DIRTY
		Sleep( 0 ); 
	}
	if(m_bCERenderDequeCLEAN == true)
	{
		m_bCERenderDequeCLEAN = false;
		while(!m_vCentralEventRender.empty())
		{
			m_vINEventsRender.push_back(m_vCentralEventRender.front());
			m_vCentralEventRender.pop_front();
		}
		m_vCentralEventRender.clear();
		m_bCERenderDequeCLEAN = true;
	}
	LeaveCriticalSection(&m_CritRenderDeque);
	return true;
}
// MUST BE CALLED FROM UPDATE SIDE
bool CEventManager::TEBTtoUpdateIN()
{
	EnterCriticalSection(&m_CritUpdateDeque);
	while(m_bCEUpdateDequeCLEAN == false)
	{
		//SPIN SLEEP WHILE DIRTY
		Sleep( 0 ); 
	}
	if(m_bCEUpdateDequeCLEAN == true)
	{
		m_bCEUpdateDequeCLEAN = false;
		
		while(!m_vCentralEventUpdate.empty())
		{
			m_vINEventsUpdate.push_back(m_vCentralEventUpdate.front());
			m_vCentralEventUpdate.pop_front();
		}
		m_vCentralEventUpdate.clear();
		
		m_bCEUpdateDequeCLEAN = true;
	}
	LeaveCriticalSection(&m_CritUpdateDeque);
	return true;
}
// MUST BE CALLED FROM UPDATE SIDE
bool CEventManager::TEBTtoRenderCentral()
{
	EnterCriticalSection(&m_CritRenderDeque);
	while(m_bCERenderDequeCLEAN == false)
	{
		//SPIN SLEEP WHILE DIRTY
		Sleep( 0 ); 
	}
	if(m_bCERenderDequeCLEAN == true)
	{
		m_bCERenderDequeCLEAN = false;
		while(!m_vOUTEventsUpdate.empty())
		{
			m_vCentralEventRender.push_back(m_vOUTEventsUpdate.front());
			m_vOUTEventsUpdate.pop_front();
		}
		m_vOUTEventsUpdate.clear();
		m_bCERenderDequeCLEAN = true;
	}
	LeaveCriticalSection(&m_CritRenderDeque);
	return true;
}
// MUST BE CALLED FROM RENDER SIDE
bool CEventManager::TEBTtoUpdateCentral()
{
	EnterCriticalSection(&m_CritUpdateDeque);
	while(m_bCEUpdateDequeCLEAN == false)
	{
		//SPIN SLEEP WHILE DIRTY
		//Sleep( 1 ); 
	}
	if(m_bCEUpdateDequeCLEAN == true)
	{
		m_bCEUpdateDequeCLEAN = false;
		while(!m_vOUTEventsRender.empty())
		{
			m_vCentralEventUpdate.push_back(m_vOUTEventsRender.front());
			m_vOUTEventsRender.pop_front();
		}
		m_vOUTEventsRender.clear();
		m_bCEUpdateDequeCLEAN = true;
	}
	LeaveCriticalSection(&m_CritUpdateDeque);
	return true;
}