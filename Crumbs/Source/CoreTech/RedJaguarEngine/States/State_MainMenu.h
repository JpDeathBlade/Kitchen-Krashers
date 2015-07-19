#ifndef  REDJAGUAR_JS_STATE_MAINMENU_H_		// Check to see if this .h is defined
#define  REDJAGUAR_JS_STATE_MAINMENU_H_		//  if not, define it

#include "RJ_IGameState.h"
#include "..\\Text\\CBitmapFont.h"
#include "../Event/EventDefinitions.h"

struct tStateConfig;

class State_MainMenu : public RJ_IGameState
{
	private:
		enum EMenu { eMIN = 0, eSTART = eMIN, eOPTIONS, eCREDITS, eEXIT, eMAX = eEXIT};
		int m_nSelection;

		CBitmapFont Font;
		tStateConfig* tSC;

		int texID;

	public:
		State_MainMenu(void);
		State_MainMenu(const State_MainMenu& _ref);
		State_MainMenu& operator= (const State_MainMenu& _ref);
		virtual ~State_MainMenu(void) { }

		virtual void OnPush(void);
		virtual void OnEnter(void);
		virtual void OnExit(void);

		virtual bool Input(double _fDelta);
		virtual bool InputUnder(double _fDelta);
		virtual bool Update(double _fDelta);
		virtual bool UpdateUnder(double _fDelta);
		virtual bool Render(double _fDelta);
		virtual bool RenderUnder(double _fDelta);

		bool LoadSettings();
};

#endif //end of REDJAGUAR_JS_STATE_MAINMENU_H_