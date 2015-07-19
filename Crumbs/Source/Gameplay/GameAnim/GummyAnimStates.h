#ifndef _GUMMYANIMSTATES_H_
#define _GUMMYANIMSTATES_H_

#include "AState.h"
#include "../Objects/AnimMesh.h"

class AGWLaunch;
class AGWIdle;

class AnimGummyWorm : public AnimLink
{
	AGWLaunch* AGWLaunchAnim;
	AGWIdle* AGWIdleAnim;
	bool m_bReversePushback;
public:
	bool m_bReleaseLaunch;
	RJ_Config::GUMMY_ANIM_STATES GummyAnimState;
	RJ_Config::GUMMY_ANIM_STATES PrevGummyAnimState;
	AnimGummyWorm();
	~AnimGummyWorm();
	void Update(double dt);
	void Init();
};


class AGWLaunch :	public AState
{
private:
	AGWLaunch(void) {AnimType = RJ_Config::RJ_ANIM_GUMMY;}
	AGWLaunch(const AGWLaunch &) {}
	const AGWLaunch & operator=(const AGWLaunch &) {}
public:
	~AGWLaunch(void);

	static AGWLaunch * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class AGWIdle :	public AState
{
private:
	AGWIdle(void) {AnimType = RJ_Config::RJ_ANIM_GUMMY;}
	AGWIdle(const AGWIdle &) {}
	const AGWIdle & operator=(const AGWIdle &) {}
public:
	~AGWIdle(void);

	static AGWIdle * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};
#endif