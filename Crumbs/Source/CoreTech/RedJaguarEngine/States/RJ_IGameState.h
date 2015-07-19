/**
 * 0===========================================================================0
 * | RJ_IGameState.h                                                           |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Author        : Joseph Stuhr                                            |
 * |   E-mail        : JpDeathBlade@gmail.com                                  |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   Date Created  : 08:43 pm (Eastern) on 05/26/2010                        |
 * |   Date Edited   : 09:25 pm (Eastern) on 05/27/2010                        |
 * 0=====0===============================================================0=====0
 *       |                                                               |      
 * 0=====0===============================================================0=====0
 * | Code Description:                                                         |
 * |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  |
 * |   An interface for a Game State.                                          |
 * 0===========================================================================0
 */

#ifndef  REDJAGUAR_JS_IGAMESTATE_H_		// Check to see if this .h is defined
#define  REDJAGUAR_JS_IGAMESTATE_H_		//  if not, define it

class RJ_IGameState
{
	public:
		RJ_IGameState(void)										{ }
		RJ_IGameState(const RJ_IGameState& _ref);
		RJ_IGameState& operator= (const RJ_IGameState& _ref);
		virtual ~RJ_IGameState(void) = 0						{ }

		virtual void OnPush(void)								{ }
		virtual void OnEnter(void)								{ }
		virtual void OnExit(void)								{ }
		virtual void OnPop(void)								{ delete this; }

		virtual bool Input(double _fDelta)						{ return true; }
		virtual bool InputUnder(double _fDelta)					{ return Input(_fDelta); }
		virtual bool Update(double _fDelta)						{ return true; }
		virtual bool UpdateUnder(double _fDelta)					{ return Update(_fDelta); }
		virtual bool Render(double _fDelta)						{ return true; }
		virtual bool RenderUnder(double _fDelta)					{ return Render(_fDelta); }
};

#endif //REDJAGUAR_JS_IGAMESTATE_H_