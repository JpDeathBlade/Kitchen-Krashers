#ifndef CSOUNDENGINE_H_
#define CSOUNDENGINE_H_

#include <windows.h>
#include <deque>
#include "../Event/IListener.h"
using std::deque;
#include "../../../Middleware/fmod.h"
#include "../../../Middleware/fmod.hpp"
#include "../../../Middleware/fmod_event.h"
#include "../../../Middleware/fmod_event.hpp"
#include "../../../Middleware/fmod_errors.h"
#include "../../../RJ_Config.h"

#define RJ_FMOD_LOOPING	   FMOD_LOOP_NORMAL | FMOD_DEFAULT

class CSoundEngine : public IListener
{
	//UNIVERSAL SOUND IDs
	int		m_nChevyMusic;
	int		m_nPowerSelect;
	int		m_nPowerRelease;
	int		m_nBrushing;
	int		m_nMetalClank;
	int		m_nSugarPickup;

	//VOLUMES
	float m_fSFXVolume;
	float m_fMusicVolume;

	//TIMERS
	float m_fFanfareIntroTimer;

	FMOD::System		*m_pSystem;
	FMOD::EventSystem	*m_pEvent;
	FMOD::Sound			*m_pFsb;
	FMOD::ChannelGroup  *m_pCGMusic;
	FMOD::ChannelGroup  *m_pCGSFX;

	FMOD::Channel		*m_MusicChannelOne; 

	struct tRJSoundInfo 
	{
		bool	bEvent;
		char filename[_MAX_PATH];
		int		nRefCount;
		FMOD_MODE unSoundFormat;
		FMOD::Sound	*fmSound;
		FMOD::Event *fmEvent;
		RJ_Config::SOUND_FILE sfEnum;
		deque<FMOD::Channel*> m_SoundChannels;
		tRJSoundInfo() 
		{ 
			bEvent = false;
			filename[0] = '\0';
			nRefCount = 0;
			memset(&unSoundFormat, 0, sizeof(unSoundFormat)); 
			fmSound = NULL; 
			fmEvent = NULL;
			sfEnum = RJ_Config::RJ_SOUND_NA;
		}
	};
	deque<tRJSoundInfo> m_SoundList;
	deque<tRJSoundInfo> m_MusicList;

	double m_dTimeAccumulator;						//	buffer used to accumulate the time that has passed
	DWORD m_dwCurrentTime, m_dwPreviousTime;		//	used to find out how much has passed since the object has been updated

	HWND m_hWnd;

	void HandleEvent(CEvent* pEvent);
	CSoundEngine( void ){}
	CSoundEngine( const CSoundEngine & );
	CSoundEngine &operator =( const CSoundEngine & );
	~CSoundEngine( void ){}
public:

	static CSoundEngine* GetInstanceRender();
	bool Init(HWND hWnd, int nMaxChannels = 50, FMOD_INITFLAGS unFlags = FMOD_INIT_NORMAL, void *vExtraDriverData = NULL);
	bool Shutdown();
	int LoadSound(const char *szFilename, FMOD_MODE  unMode = FMOD_DEFAULT, RJ_Config::SOUND_FILE sfSoundEnum = RJ_Config::RJ_SOUND_NA);
	bool LoadBank(const char *szFilename, FMOD_MODE  unMode = FMOD_DEFAULT);

	bool PlaySound(int nID);
	bool PlaySound(RJ_Config::SOUND_FILE sfSoundEnum);
	bool UnloadSound(int nID);
	bool ResetSound(int nID);
	bool StopSound(int nID);
	bool ResetSound(RJ_Config::SOUND_FILE sfSoundEnum);
	bool StopSound(RJ_Config::SOUND_FILE sfSoundEnum);
	bool SetVolume(int nID, float fVolume);
	bool SetFrequency(int nID, float fFreq);
	bool SetPan(int nID, float fPan);
	bool IsSoundPlaying(int nID);
	bool IsSoundPlaying(RJ_Config::SOUND_FILE sfSoundEnum);
	bool Update(void);
	bool ShutdownFModManager(void);
	bool InitIndexMap(FMOD::Sound *fsb);
	void ChangeMasterVolumeSFX();
	void ChangeMasterVolumeMusic();
	bool LoadSettings();

protected:
	void ReduceHandleCount(void);
};
#endif