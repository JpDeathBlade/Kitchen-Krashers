#include "CSoundEngine.h"
#include <assert.h>
#include "../Event/EventDefinitions.h"
#include "../Event/EventManager.h"
#include "GameAudio.h"
#define TIMER_COUNT 5.0f
#include <fstream>

CSoundEngine* CSoundEngine::GetInstanceRender()
{
	static CSoundEngine instance;
	return &instance;
}

bool CSoundEngine::Init(HWND hWnd, int nMaxChannels, FMOD_INITFLAGS unFlags, void *vExtraDriverData)
{
	if (!hWnd || nMaxChannels < 0 || unFlags < 0) 
		return false;
	FMOD_RESULT result;

	//	Create the main system object.
	if ((result = FMOD::System_Create(&m_pSystem)) != FMOD_OK)
	{
		return false;
	}

	// Initialize FMOD.
	if ((result = m_pSystem->init( nMaxChannels, unFlags, vExtraDriverData ) ) != FMOD_OK)
	{
		return false;
	}
	if(result = FMOD::EventSystem_Create(&m_pEvent))
		return false;
	if(result = m_pEvent->init(50, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL))
		return false;
	if(result = m_pEvent->setMediaPath("Resources/Sounds/"))
		return false;
	if(result = m_pEvent->load("GameAudio.fev", 0, 0))
		return false;

	m_nChevyMusic = LoadSound("Resources/Sounds/ChevyChase.mp3", FMOD_CREATESTREAM | FMOD_NONBLOCKING | FMOD_HARDWARE, RJ_Config::RJ_MUSIC_CHASE);

	if(!LoadBank("Resources/Sounds/GameAudio.fsb",FMOD_CREATESTREAM | FMOD_NONBLOCKING | FMOD_HARDWARE))
		return false;

	m_pSystem->createChannelGroup("Music", &m_pCGMusic);
	m_pSystem->createChannelGroup("SFX", &m_pCGSFX);
	m_pSystem->playSound(FMOD_CHANNEL_FREE, m_SoundList[m_nChevyMusic].fmSound, true, &m_MusicChannelOne);
	m_MusicChannelOne->stop();
	m_SoundList[ m_nChevyMusic ].m_SoundChannels.push_back( m_MusicChannelOne );
	m_MusicChannelOne->setChannelGroup(m_pCGMusic);

	m_hWnd = hWnd;
	
	m_dwCurrentTime = GetTickCount();
	m_dwPreviousTime = GetTickCount();
	//LOAD ALL UNIVERSAL SOUNDS
	//m_nPowerRelease = LoadSound("Resources/Sounds/PowerRelease.wav", FMOD_CREATESTREAM | FMOD_HARDWARE, RJ_Config::RJ_SOUND_POWERRELEASE);

	m_fSFXVolume = 80.0f;
	m_fMusicVolume = 70.0f;

	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_PLAYSOUND,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_STOPSOUND,this);
	CEventManager::GetInstanceThreaded()->RegisterListenerRender(ET_ADJUSTVOLUME,this);

	m_fFanfareIntroTimer = 0.0f;
	ChangeMasterVolumeSFX();
	ChangeMasterVolumeMusic();

	return true;
}
void CSoundEngine::HandleEvent(CEvent* pEvent)
{
	if(pEvent->GetEventID() == ET_PLAYSOUND)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_PLAYSOUNDID)
			{
				tPlaySoundID* tPS;
				tPS = (tPlaySoundID*)pEvent->GetData();
				if(tPS->m_bIfNotPlay)
				{
					if(!IsSoundPlaying(tPS->m_nSoundID))
						PlaySound(tPS->m_nSoundID);
				}
				else
					PlaySound(tPS->m_nSoundID);
			}
			else if (pEvent->GetData()->GetMsgID() == EM_PLAYSOUNDENUM)
			{
				tPlaySoundEnum* tPS;
				tPS = (tPlaySoundEnum*)pEvent->GetData();
				if(tPS->m_bIfNotPlay)
				{
					if(!IsSoundPlaying(tPS->m_sfSoundEnum))
						PlaySound(tPS->m_sfSoundEnum);
				}
				else
					PlaySound(tPS->m_sfSoundEnum);
			}
		}
	}
	else if(pEvent->GetEventID() == ET_STOPSOUND)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_STOPSOUNDID)
			{
				tStopSoundID* tPS;
				tPS = (tStopSoundID*)pEvent->GetData();
				StopSound(tPS->m_nSoundID);
			}
			else if (pEvent->GetData()->GetMsgID() == EM_STOPSOUNDENUM)
			{
				tStopSoundEnum* tPS;
				tPS = (tStopSoundEnum*)pEvent->GetData();
				StopSound(tPS->m_sfSoundEnum);
			}
		}
	}
	else if(pEvent->GetEventID() == ET_ADJUSTVOLUME)
	{
		if(pEvent->GetData() != NULL)
		{
			if(pEvent->GetData()->GetMsgID() == EM_ADJUSTVOLUME)
			{
				tVolumeData* tPS;
				tPS = (tVolumeData*)pEvent->GetData();
				if(tPS->m_eType == 2)
				{
					if(m_fMusicVolume != tPS->m_fVolume)
					{
						m_fMusicVolume = tPS->m_fVolume;
						ChangeMasterVolumeMusic();
					}
				}
				else if(tPS->m_eType == 1)
				{		
					if(m_fSFXVolume != tPS->m_fVolume)
					{
						m_fSFXVolume = tPS->m_fVolume;
						ChangeMasterVolumeSFX();
						if(!IsSoundPlaying(RJ_Config::RJ_POWERUP_ANTHONY))
							PlaySound(RJ_Config::RJ_POWERUP_ANTHONY);
					}
				}
			}
		}
	}
	pEvent->m_nRefs--;
}
int CSoundEngine::LoadSound(const char *szFilename, FMOD_MODE  unMode, RJ_Config::SOUND_FILE sfSoundEnum)
{
	if( !m_pSystem )
		return -1;

	if( !szFilename )
		return -1;

	int nHandle = -1;	
	int counter = 0;	

	FMOD_RESULT result;

	deque<tRJSoundInfo>::iterator vIter = m_SoundList.begin();	

	while( vIter != m_SoundList.end() ) 
	{
		if( _stricmp( (*vIter).filename, szFilename ) == 0 ) 
		{
			(*vIter).nRefCount++;
			nHandle = counter;
			break;
		}
		vIter++;
		counter++;
	}

	if( nHandle != -1) return nHandle;		//	We've found our handle!
	counter = 0;

	//	if one of the already used sounds is done with, reuse that spot
	//	NOTE:
	//	(only use this if I can reset each object
	while( vIter != m_SoundList.end() ) 
	{
		if( (*vIter).nRefCount == 0 ) 
		{
			nHandle = counter;
			break;
		}
		vIter++;
		counter++;
	}

	//	the sound has not been loaded before,
	//	so a new one must be made based on this file
	tRJSoundInfo newSound;

	if( nHandle == -1 ) 
	{
		//	copy name into object
		strncpy_s( newSound.filename, _MAX_PATH, szFilename, strlen( szFilename ) );

		//	first instance of this sound
		newSound.nRefCount = 1;

		//	set flags to the sound
		newSound.unSoundFormat |= unMode;
		newSound.sfEnum = sfSoundEnum;
		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE
		if( ( result = m_pSystem->createSound( newSound.filename, newSound.unSoundFormat, 0, &newSound.fmSound ) ) != FMOD_OK )
		{
			return -1;
		}
		m_SoundList.push_back( newSound );
		return (int)m_SoundList.size() - 1;
	}
	else
	{
		//	release anything still in m_SoundList[ nHandle ];
		//	then copy above 

	}
	return nHandle;
}
bool CSoundEngine::PlaySound(int nID)
{
	if( !m_pSystem ) return false;
	//assert( nID > -1 && nID < (int)m_SoundList.size() && "ID is out of range" );
	FMOD::Channel *channel;
	FMOD_RESULT result;
	if(!m_SoundList[ nID ].bEvent)
	{
		if( ( result = m_pSystem->playSound(FMOD_CHANNEL_FREE, m_SoundList[ nID ].fmSound , false, &channel) ) != FMOD_OK ) 
			return false;
	}
	else
	{
		m_SoundList[ nID ].fmEvent->start();
	}
	m_SoundList[ nID ].m_SoundChannels.push_back( channel );
	return true;
}
bool CSoundEngine::PlaySound(RJ_Config::SOUND_FILE sfSoundEnum)
{
	if( !m_pSystem ) return false;
	//assert( nID > -1 && nID < (int)m_SoundList.size() && "ID is out of range" );
	FMOD::Channel *channel;
	FMOD_RESULT result;
	int nID = -1;
	int nsoundsize = m_SoundList.size();
	for(int SiD = 0; SiD < nsoundsize; ++SiD)
	{
		if(m_SoundList[SiD].sfEnum == sfSoundEnum)
			nID = SiD;
	}
	if(nID == -1)
		return false;
	if(!m_SoundList[ nID ].bEvent)
	{
		if( ( result = m_pSystem->playSound(FMOD_CHANNEL_FREE, m_SoundList[ nID ].fmSound , false, &channel) ) != FMOD_OK ) 
			return false;
	}
	else
	{
		if(IsSoundPlaying(sfSoundEnum))
			m_SoundList[ nID ].fmEvent->stop(true);
		m_SoundList[ nID ].fmEvent->start();
	}
	if(m_nChevyMusic == nID)
		channel->setChannelGroup(m_pCGMusic);
	m_SoundList[ nID ].m_SoundChannels.push_back( channel );
	return true;
}
bool CSoundEngine::UnloadSound(int nID)
{
	if( !m_pSystem ) return false;
	//assert( nID > -1 && nID < (int)m_SoundList.size() && "ID is out of range" );
	bool bOutcome = false;
	FMOD_RESULT result;
	if( m_SoundList[nID].nRefCount == 0 || ( result = m_SoundList[nID].fmSound->release() ) == FMOD_OK || ( result = m_SoundList[nID].fmEvent->release() ) == FMOD_OK) 
	{
		ZeroMemory(m_SoundList[nID].filename, _MAX_PATH);
		m_SoundList[nID].fmSound = NULL;
		m_SoundList[nID].nRefCount = 0;
		bOutcome = true;
	}
	else
		return false;
	return bOutcome;
}
bool CSoundEngine::ResetSound(int nID)
{
	if( !m_pSystem ) return false;
	//assert( nID > -1 && nID < (int)m_SoundList.size() && "ID is out of range" );
	FMOD_RESULT result;
	bool bOutcome = false;
	deque<FMOD::Channel*>::iterator iter = m_SoundList[nID].m_SoundChannels.begin();
	while( iter != m_SoundList[nID].m_SoundChannels.end() ) 
	{
		if( ( result = (*iter)->isPlaying( &bOutcome ) ) == FMOD_OK ) 
		{
			(*iter)->setPosition( 0,  FMOD_TIMEUNIT_MS);
		}
		iter++;
	}
	return bOutcome;
}
bool CSoundEngine::StopSound(int nID)
{
	if( !m_pSystem ) return false;
	//assert( nID > -1 && nID < (int)m_SoundList.size() && "ID is out of range" );
	FMOD_RESULT result;
	bool bOutcome = false;
	if(!m_SoundList[ nID ].bEvent)
	{
		deque<FMOD::Channel*>::iterator iter = m_SoundList[nID].m_SoundChannels.begin();
		while( iter != m_SoundList[nID].m_SoundChannels.end() )
		{
			if( ( result = (*iter)->isPlaying( &bOutcome ) ) == FMOD_OK ) 
			{
				(*iter)->stop();
			}
			iter++;
		}
	}
	else
		m_SoundList[ nID ].fmEvent->stop();
	return bOutcome;
}
bool CSoundEngine::StopSound(RJ_Config::SOUND_FILE sfSoundEnum)
{
	if( !m_pSystem ) return false;
	//assert( nID > -1 && nID < (int)m_SoundList.size() && "ID is out of range" );
	FMOD_RESULT result;
	bool bOutcome = false;
	int nID = -1;
	int nsoundsize = m_SoundList.size();
	for(int SiD = 0; SiD < nsoundsize; ++SiD)
	{
		if(m_SoundList[SiD].sfEnum == sfSoundEnum)
			nID = SiD;
	}
	if(nID == -1)
		return false;
	if(!m_SoundList[ nID ].bEvent)
	{
		deque<FMOD::Channel*>::iterator iter = m_SoundList[nID].m_SoundChannels.begin();
		while( iter != m_SoundList[nID].m_SoundChannels.end() )
		{
			if( ( result = (*iter)->isPlaying( &bOutcome ) ) == FMOD_OK ) 
			{
				(*iter)->stop();
			}
			iter++;
		}
	}	
	else
		m_SoundList[ nID ].fmEvent->stop();
	return bOutcome;
}
bool CSoundEngine::SetVolume(int nID, float fVolume)
{
	if( !m_pSystem ) return false;
	//assert( nID > -1 && nID < (int)m_SoundList.size() && "ID is out of range" );
	FMOD_RESULT result;
	bool bOutcome = false;
	deque<FMOD::Channel*>::iterator iter = m_SoundList[nID].m_SoundChannels.begin();
	while( iter != m_SoundList[nID].m_SoundChannels.end() ) 
	{
		if( ( result = (*iter)->isPlaying( &bOutcome ) ) == FMOD_OK ) 
		{
			float currVolume = 0.0f;
			(*iter)->getVolume(&currVolume);
			(*iter)->setVolume( currVolume + fVolume );
		}
		iter++;
	}
	return bOutcome;
}
bool CSoundEngine::SetFrequency(int nID, float fFreq)
{
	if( !m_pSystem ) return false;
	//assert( nID > -1 && nID < (int)m_SoundList.size() && "ID is out of range" );
	FMOD_RESULT result;
	bool bOutcome = false;
	deque<FMOD::Channel*>::iterator iter = m_SoundList[nID].m_SoundChannels.begin();
	while( iter != m_SoundList[nID].m_SoundChannels.end() )
	{
		if( ( result = (*iter)->isPlaying( &bOutcome ) ) == FMOD_OK ) 
		{
			(*iter)->setFrequency( fFreq );
		}
		iter++;
	}
	return bOutcome;
}
bool CSoundEngine::SetPan(int nID, float fPan)
{
	if( !m_pSystem ) return false;
	//assert( nID > -1 && nID < (int)m_SoundList.size() && "ID is out of range" );
	FMOD_RESULT result;
	bool bOutcome = false;
	deque<FMOD::Channel*>::iterator iter = m_SoundList[nID].m_SoundChannels.begin();
	while( iter != m_SoundList[nID].m_SoundChannels.end() )
	{
		if( ( result = (*iter)->isPlaying( &bOutcome ) ) == FMOD_OK ) 
		{
			(*iter)->setPan( fPan );
		}
		iter++;
	}
	return bOutcome;
}
bool CSoundEngine::IsSoundPlaying(int nID)
{
	if( !m_pSystem ) return false;
	//assert( nID > -1 && nID < (int)m_SoundList.size() && "ID is out of range" );
	FMOD_RESULT result;
	bool bOutcome = false;
	deque<FMOD::Channel*>::iterator iter = m_SoundList[nID].m_SoundChannels.begin();
	while( iter != m_SoundList[nID].m_SoundChannels.end() ) 
	{
		if(!m_SoundList[ nID ].bEvent)
		{
			if( ( result = (*iter)->isPlaying( &bOutcome ) ) == FMOD_OK ) 
			{
				break;
			}
		}
		else
		{
			m_SoundList[ nID ].fmEvent->getPaused(&bOutcome);
		}
		iter++;
	}
	return bOutcome;
}
bool CSoundEngine::IsSoundPlaying(RJ_Config::SOUND_FILE sfSoundEnum)
{
	if( !m_pSystem ) return false;
	//assert( nID > -1 && nID < (int)m_SoundList.size() && "ID is out of range" );
	FMOD_RESULT result;
	bool bOutcome = false;
	int nID = -1;
	int nsoundsize = m_SoundList.size();
	for(int SiD = 0; SiD < nsoundsize; ++SiD)
	{
		if(m_SoundList[SiD].sfEnum == sfSoundEnum)
			nID = SiD;
	}
	if(nID == -1)
		return false;
	deque<FMOD::Channel*>::iterator iter = m_SoundList[nID].m_SoundChannels.begin();
	while( iter != m_SoundList[nID].m_SoundChannels.end() ) 
	{
		if(!m_SoundList[ nID ].bEvent)
		{
			if( ( result = (*iter)->isPlaying( &bOutcome ) ) == FMOD_OK ) 
			{
				break;
			}
		}
		else
		{
			m_SoundList[ nID ].fmEvent->getPaused(&bOutcome);
			//bOutcome = !bOutcome;
			break;
		}
		iter++;
	}
	return bOutcome;
}
bool CSoundEngine::Update(void)
{

	FMOD_RESULT result;
	if( ( result = m_pSystem->update() ) != FMOD_OK)
		return false;
	if( ( result = m_pEvent->update() ) != FMOD_OK)
		return false;
	m_dwPreviousTime = m_dwCurrentTime;
	m_dwCurrentTime = GetTickCount();
	double dDeltaTime = static_cast<double>( (m_dwCurrentTime - m_dwPreviousTime) / 1000.0f);
	m_dTimeAccumulator += dDeltaTime;
	if( m_dTimeAccumulator >= TIMER_COUNT ) 
	{
		ReduceHandleCount( );
		m_dTimeAccumulator -= TIMER_COUNT;
	}
	if(m_fFanfareIntroTimer < 21.0f)
	{
		m_fFanfareIntroTimer += (float)dDeltaTime;
		if(m_fFanfareIntroTimer > 8.0f && m_fFanfareIntroTimer < 14.0f && !IsSoundPlaying(RJ_Config::RJ_GUI_FANFARE3))
			PlaySound(RJ_Config::RJ_GUI_FANFARE3);
	}
	else
	{
		if(!IsSoundPlaying(RJ_Config::RJ_MUSIC_CHASE))
			PlaySound(RJ_Config::RJ_MUSIC_CHASE);
	}



	return true;
}
bool CSoundEngine::ShutdownFModManager(void)
{
	CEventManager::GetInstanceThreaded()->UnregisterListenerAllRender(this);
	//m_pEvent->release();
	deque<tRJSoundInfo>::iterator vIter = m_SoundList.begin();
	for(int counter = 0; counter < (int)m_SoundList.size(); counter++) 
	{
		if( UnloadSound( counter ) ) { }
	}
	m_pEvent->release();
	m_pFsb->release();
	m_SoundList.clear();
	m_pSystem->close();
	m_pSystem->release();
	return true;
}
void CSoundEngine::ReduceHandleCount(void)
{
	FMOD_RESULT result;
	bool bOutcome = false;
	deque<tRJSoundInfo>::iterator deqIter = m_SoundList.begin();
	while( deqIter != m_SoundList.end() )
	{
		deque<FMOD::Channel *>::iterator ChanIter = (*deqIter).m_SoundChannels.begin();
		while( ChanIter != (*deqIter).m_SoundChannels.end() ) 
		{
			bOutcome = false;
			if( ( result = (*ChanIter)->isPlaying( &bOutcome ) ) == FMOD_ERR_INVALID_HANDLE || result != FMOD_OK ) 
			{
				ChanIter = (*deqIter).m_SoundChannels.erase( ChanIter );
			}
			else
				ChanIter++;
		}
		deqIter++;
	}
}
bool CSoundEngine::LoadBank(const char *szFilename, FMOD_MODE  unMode)
{
	FMOD_RESULT result;
	if(result = m_pSystem->createStream("Resources/Sounds/GameAudio.fsb", FMOD_CREATESTREAM | FMOD_NONBLOCKING | FMOD_HARDWARE, 0, &m_pFsb))
		return false;
	int numsubsounds, i;
	tRJSoundInfo RJSoundInfo;
	FMOD_OPENSTATE openstate;
	do
	{
		m_pFsb->getOpenState(&openstate, 0, 0);
	} while (openstate != FMOD_OPENSTATE_READY);

	if(result = m_pFsb->getNumSubSounds(&numsubsounds))
		return false;

	for (i = 0; i < numsubsounds; ++i)
	{
		FMOD::Sound *sound;
		if(result = m_pFsb->getSubSound(i, &sound))
			return false;
		do
		{
			m_pFsb->getOpenState(&openstate, 0, 0);
		} while (openstate != FMOD_OPENSTATE_READY);

		if(result = sound->getName(RJSoundInfo.filename, _MAX_PATH))
			return false;
		float VolAdjust = 0.0f;
		//ASSIGN AN ENUM AND EVENT
		//*************************************AMBIENCE************************************
		if(strncmp(RJSoundInfo.filename, "a_ambience_freezer.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Ambience/a_ambience_freezer",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_AMBIENT_LVL1FREEZER;
			RJSoundInfo.fmEvent->getVolume(&VolAdjust);
			RJSoundInfo.fmEvent->setVolume(VolAdjust*0.5f);
		}
		else if(strncmp(RJSoundInfo.filename, "a_ambience_kitchen.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Ambience/a_ambience_kitchen",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_AMBIENT_LVL2KITCHEN;
			RJSoundInfo.fmEvent->getVolume(&VolAdjust);
			RJSoundInfo.fmEvent->setVolume(VolAdjust*0.5f);
		
		}
		else if(strncmp(RJSoundInfo.filename, "a_ambience_sinkdrip_01.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Ambience/a_ambience_sinkdrip",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_AMBIENT_SINKDRIP;
			RJSoundInfo.fmEvent->getVolume(&VolAdjust);
			RJSoundInfo.fmEvent->setVolume(VolAdjust*0.5f);

		}//*************************************ANTHONY************************************
		else if(strncmp(RJSoundInfo.filename, "a_anthony_collision01.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_collision",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLISION_ANTHONY;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_sweep.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_sweep",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_SWEEP_ANTHONY;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_ride_idle.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_ride_idle",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_RIDEIDLE;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_ride_water.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_ridewater",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_RIDEWATER;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_powerup.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_powerup",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_POWERUP_ANTHONY;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_scrape.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_scrape",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_SCRAPE;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_hazard1.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_hazard",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_HAZARD;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_launch1.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_launch",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_LAUNCH;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_mount1.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_mount",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_MOUNT;
		}
		else if(strncmp(RJSoundInfo.filename, "ShotAngle.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_shotangle",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_ANGLE;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_browniepoint1.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_shotover1",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_CHEER1;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_browniepoint2.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_shotover2",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_CHEER2;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_browniepoint3.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_shotover3",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_CHEER3;
		}
		else if(strncmp(RJSoundInfo.filename, "a_anthony_browniepoint4.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_shotover4",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_CHEER4;
		}
		else if(strncmp(RJSoundInfo.filename, "BrowniePoint.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Anthony/a_anthony_browniepoint",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_ANTHONY_BROWNIEPOINT;
		}//*************************************GUI******************************************
		else if(strncmp(RJSoundInfo.filename, "a_gui_select01.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/GUI/a_gui_select",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_UI_SELECT;
			RJSoundInfo.fmEvent->getVolume(&VolAdjust);
			RJSoundInfo.fmEvent->setVolume(VolAdjust*2.6f);
		}
		else if(strncmp(RJSoundInfo.filename, "a_gui_navigate01.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/GUI/a_gui_navigate",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_UI_NAVIGATE;
			RJSoundInfo.fmEvent->getVolume(&VolAdjust);
			RJSoundInfo.fmEvent->setVolume(VolAdjust*2.6f);
		}
		else if(strncmp(RJSoundInfo.filename, "a_gui_increase.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/GUI/a_gui_increase",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_UI_INCREASE;
			RJSoundInfo.fmEvent->getVolume(&VolAdjust);
			RJSoundInfo.fmEvent->setVolume(VolAdjust*1.6f);
		}
		else if(strncmp(RJSoundInfo.filename, "a_gui_scoretotal.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/GUI/a_gui_scoretotal",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_GUI_SCORETOTAL;
		}
		else if(strncmp(RJSoundInfo.filename, "a_gui_scoretally.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/GUI/a_gui_scoretally",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_GUI_SCORETALLY;
		}
		else if(strncmp(RJSoundInfo.filename, "a_gui_decrease.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/GUI/a_gui_decrease",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_UI_DECREASE;
			RJSoundInfo.fmEvent->getVolume(&VolAdjust);
			RJSoundInfo.fmEvent->setVolume(VolAdjust*1.6f);
		}
		else if(strncmp(RJSoundInfo.filename, "Menu_toaster_up.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/GUI/a_gui_toastup",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_GUI_TOASTDOWN;
		}
		else if(strncmp(RJSoundInfo.filename, "Menu_toaster_down.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/GUI/a_gui_toastdown",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_GUI_TOASTUP;
		}
		else if(strncmp(RJSoundInfo.filename, "Fanfare 3.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/GUI/a_gui_fanfare3",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_GUI_FANFARE3;
		}//*************************************OBJECTS***************************************
		else if(strncmp(RJSoundInfo.filename, "a_object_gummyworm.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Objects/a_object_gummyworm",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_SOUND_POWERSELECT;
		}
		else if(strncmp(RJSoundInfo.filename, "a_object_stonecollide.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Objects/a_object_stone",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_STONE;
		}
		else if(strncmp(RJSoundInfo.filename, "a_object_bullseye.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Objects/a_object_bullseye",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_BULLSEYE;
		}
		else if(strncmp(RJSoundInfo.filename, "a_object_sugartap01.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Objects/a_object_sugar01",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_SUGAR_1;
		}
		else if(strncmp(RJSoundInfo.filename, "a_object_sugartap02.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Objects/a_object_sugar02",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_SUGAR_2;
		}
		else if(strncmp(RJSoundInfo.filename, "a_object_sugartap03.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Objects/a_object_sugar03",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_SUGAR_3;
		}//*************************************OBSTABLES************************************
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_icecream.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_icecream",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_ICECREAM;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_fish.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_fish",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_FISH;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_freezerseal.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_freezerseal",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_FREEZERSEAL;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_icecube.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_icecube",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_ICECUBE;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_icetray.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_icetray",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_ICETRAY;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_peas.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_peas",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_PEAS;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_pie_bitten.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_pie_bitten",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_BITTEN;
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_pie_whole",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_WHOLE;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_pizza.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_pizza",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_PIZZA;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_popsicle.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_popsicle",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_POPSICLE;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_soda.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_soda",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_SODA;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_coffeepot1.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_coffeepot",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_COFFEEPOT;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_cerealbox1.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_cerealbox",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_CEREALBOX;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_juicecup.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_juicecup",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_JUICECUP;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_lightswitch_up.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_lightswitch",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_LIGHTSWITCH;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_kitchensink1.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_kitchensink",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_KITCHENSINK;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_knife.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_knife",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_KNIFE;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_fruit.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_fruit",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_FRUIT;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_gelatin1.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_gelatin",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_GELATIN;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_duck.mp3", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_duck",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_DUCK;
		}
		else if(strncmp(RJSoundInfo.filename, "a_obstacle_carrot.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Obstacles/a_obstacle_carrot",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_COLLIDE_CARROT;
		}//*************************************POWERUPS************************************
		else if(strncmp(RJSoundInfo.filename, "a_powerup_whipcreamhit.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Powerups/a_powerup_whipcream",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_POWERUP_WHIPCREAM;
		}
		else if(strncmp(RJSoundInfo.filename, "a_powerup_cherries.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Powerups/a_powerup_cherries",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_POWERUP_CHERRIES;
		}
		else if(strncmp(RJSoundInfo.filename, "a_powerup_gum.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Powerups/a_powerup_gum",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_POWERUP_GUM;
		}
		else if(strncmp(RJSoundInfo.filename, "a_powerup_fortunecookie.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Powerups/a_powerup_fortunecookie",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_POWERUP_FORTUNECOOKIE;
		}
		else if(strncmp(RJSoundInfo.filename, "a_powerup_peanut.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Powerups/a_powerup_peanut",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_POWERUP_PEANUT;
		}
		else if(strncmp(RJSoundInfo.filename, "a_powerup_carrot.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Powerups/a_powerup_carrot",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_POWERUP_CARROT;
		}
		else if(strncmp(RJSoundInfo.filename, "a_powerup_butter1.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Powerups/a_powerup_butter",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_POWERUP_BUTTER;
		}
		else if(strncmp(RJSoundInfo.filename, "ColdTurkey.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Powerups/a_powerup_coldturkeyUse",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_POWERUP_COLDTURKEYUSE;
		}
		else if(strncmp(RJSoundInfo.filename, "Gobble.wav", _MAX_PATH) == 0)
		{
			m_pEvent->getEvent("GameAudio/GameAudio/Powerups/a_powerup_coldturkey",
				unMode,&RJSoundInfo.fmEvent);
			RJSoundInfo.sfEnum = RJ_Config::RJ_POWERUP_COLDTURKEY;
		}
		else
		{
			continue;
			RJSoundInfo.fmEvent = NULL;
			RJSoundInfo.sfEnum = RJ_Config::RJ_SOUND_NA;
		}


		RJSoundInfo.nRefCount = 1;
		RJSoundInfo.unSoundFormat |= unMode;
		RJSoundInfo.bEvent = true;
		RJSoundInfo.fmSound = sound;
		m_SoundList.push_back(RJSoundInfo);
		sound = NULL;
	}
	return true;
}
void CSoundEngine::ChangeMasterVolumeSFX()
{
	float truevolume = m_fSFXVolume * 0.01f;
	int soundsize = m_SoundList.size();
	for (int soundid = 0; soundid < soundsize; ++soundid)
	{
		m_SoundList[soundid].fmEvent->setVolume(truevolume);
	}
}
void CSoundEngine::ChangeMasterVolumeMusic()
{
	float truevolume = m_fMusicVolume * 0.01f;
	m_pCGMusic->setVolume(truevolume);
	//m_SoundList[m_nChevyMusic].fmSound->setMusicChannelVolume(,truevolume);
}
bool CSoundEngine::Shutdown()
{
	ShutdownFModManager();
	m_hWnd = NULL;
	return true;
}
bool CSoundEngine::LoadSettings()
{
	// parse file
	std::fstream file;
	file.open("Resources/Data/data.sav", std::ios_base::in | std::ios_base::binary);
	if (file.is_open())
	{
		// load info
		file.read( (char*)&m_fSFXVolume, sizeof(float) );
		file.read( (char*)&m_fMusicVolume, sizeof(float) );
		file.close();
		return true;
	}
	return false;
}