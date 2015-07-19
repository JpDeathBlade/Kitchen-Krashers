#ifndef  REDJAGUAR_JS_STATE_GAMEOVER_H_		// Check to see if this .h is defined
#define  REDJAGUAR_JS_STATE_GAMEOVER_H_		//  if not, define it

#include "RJ_IGameState.h"
#include "..\\Text\\CBitmapFont.h"
#include "../Event/EventDefinitions.h"
#include "State_Config.h"
#include <vector>
#include "../Event/IListener.h"

class CPlayer;
class CGameplay;

class State_GameOver: public RJ_IGameState, public IListener
{
	private:
		enum EMenu { eMIN = 0, ePLAYAGAIN = eMIN, eMENU/*, eEXIT*/, eMAX = eMENU};
		int m_nSelection;

		CBitmapFont Font;
		CBitmapFont DakotaFont;
		int Score[4];
		int Standing[4];
		int NumPlayers;
		int par;

		int m_vColors[4][3];

		CGameplay*		 Gameplay;
		tStateConfig*	 Config;
		tPlayerData playDat;

	public:
		State_GameOver(void);
		State_GameOver(const State_GameOver& _ref);
		State_GameOver& operator= (const State_GameOver& _ref);
		virtual ~State_GameOver(void) { }

		virtual void OnPush(void);
		virtual void OnEnter(void);
		virtual void OnExit(void);
		virtual void OnPop(void);

		virtual bool Input(double _fDelta);
		virtual bool InputUnder(double _fDelta);
		virtual bool Update(double _fDelta);
		virtual bool UpdateUnder(double _fDelta);
		virtual bool Render(double _fDelta);
		virtual bool RenderUnder(double _fDelta);

		void HandleEvent(CEvent* pEvent);
		void SendShutdownEvent(void);

};

#endif //end of REDJAGUAR_JS_STATE_GAMEOVER_H_