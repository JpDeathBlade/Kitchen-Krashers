#ifndef _TOOTHPICKANIMSTATES_H_
#define _TOOTHPICKANIMSTATES_H_

#include "AState.h"
#include "../Objects/AnimMesh.h"

class ATPLaunch;
class ATPIdle;

class AnimToothPick : public AnimLink
{
	ATPLaunch* ATPLaunchAnim;
	ATPIdle* ATPIdleAnim;
	bool m_bReversePushback;
public:
	bool m_bReleaseLaunch;
	RJ_Config::TOOTH_ANIM_STATES ToothAnimState;
	RJ_Config::TOOTH_ANIM_STATES PrevToothAnimState;
	AnimToothPick();
	~AnimToothPick();
	void Update(double dt);
	void Init();
};


class ATPLaunch :	public AState
{
private:
	ATPLaunch(void) {AnimType = RJ_Config::RJ_ANIM_TOOTHPICK;}
	ATPLaunch(const ATPLaunch &) {}
	const ATPLaunch & operator=(const ATPLaunch &) {}
public:
	~ATPLaunch(void);

	static ATPLaunch * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ATPIdle :	public AState
{
private:
	ATPIdle(void) {AnimType = RJ_Config::RJ_ANIM_TOOTHPICK;}
	ATPIdle(const ATPIdle &) {}
	const ATPIdle & operator=(const ATPIdle &) {}
public:
	~ATPIdle(void);

	static ATPIdle * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

#endif