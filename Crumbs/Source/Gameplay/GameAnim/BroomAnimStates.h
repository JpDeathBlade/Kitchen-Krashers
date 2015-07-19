#ifndef _BROOMANIMSTATES_H_
#define _BROOMANIMSTATES_H_

#include "AState.h"
#include "../Objects/AnimMesh.h"

class ABRLaunch;
class ABRIdleRide;
class ABRIdle;
class ABRtoGummy;
class ABRPushBack;
class ABRSweepForward;
class ABRSweepLeft;
class ABRSweepRight;
class ABRCollition;
class ABRFoul;
class ABRScrape;
class ABRWin;

class AnimBroom : public AnimLink
{
	ABRLaunch* ABRLaunchAnim;
	ABRIdle* ABRIdleAnim;
	ABRtoGummy* ABRtoGummyAnim;
	ABRPushBack* ABRPushBackAnim;
	ABRIdleRide* ABRIdleRideAnim;
	ABRSweepForward* ABRSweepForwardAnim;
	ABRSweepLeft* ABRSweepLeftAnim;
	ABRSweepRight* ABRSweepRightAnim;
	ABRCollition* ABRCollitionAnim;
	ABRFoul* ABRFoulAnim;
	ABRScrape* ABRScrapeAnim;
	ABRWin* ABRWinAnim;

	bool m_bReversePushback;
public:
	bool m_bSweepEnd;
	RJ_Config::BROOM_ANIM_STATES BroomAnimState;
	RJ_Config::BROOM_ANIM_STATES PrevBroomAnimState;
	AnimBroom();
	~AnimBroom();
	void Update(double dt);
	void Init();
};

class ABRIdleRide :	public AState
{
private:
	ABRIdleRide(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRIdleRide(const ABRIdleRide &) {}
	const ABRIdleRide & operator=(const ABRIdleRide &) {}
public:
	~ABRIdleRide(void);

	static ABRIdleRide * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ABRLaunch :	public AState
{
private:
	ABRLaunch(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRLaunch(const ABRLaunch &) {}
	const ABRLaunch & operator=(const ABRLaunch &) {}
public:
	~ABRLaunch(void);

	static ABRLaunch * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ABRIdle :	public AState
{
private:
	ABRIdle(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRIdle(const ABRIdle &) {}
	const ABRIdle & operator=(const ABRIdle &) {}
public:
	~ABRIdle(void);

	static ABRIdle * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ABRtoGummy :	public AState
{
private:
	ABRtoGummy(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRtoGummy(const ABRtoGummy &) {}
	const ABRtoGummy & operator=(const ABRtoGummy &) {}
public:
	~ABRtoGummy(void);

	static ABRtoGummy * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ABRPushBack :	public AState
{
private:
	ABRPushBack(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRPushBack(const ABRPushBack &) {}
	const ABRPushBack & operator=(const ABRPushBack &) {}
public:
	~ABRPushBack(void);

	static ABRPushBack * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ABRSweepForward : public AState
{
private:
	ABRSweepForward(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRSweepForward(const ABRSweepForward &) {}
	const ABRSweepForward & operator=(const ABRSweepForward &) {}
public:
	~ABRSweepForward(void);

	static ABRSweepForward * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ABRSweepLeft : public AState
{
private:
	ABRSweepLeft(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRSweepLeft(const ABRSweepLeft &) {}
	const ABRSweepLeft & operator=(const ABRSweepLeft &) {}
public:
	~ABRSweepLeft(void);

	static ABRSweepLeft * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ABRSweepRight : public AState
{
private:
	ABRSweepRight(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRSweepRight(const ABRSweepRight &) {}
	const ABRSweepRight & operator=(const ABRSweepRight &) {}
public:
	~ABRSweepRight(void);

	static ABRSweepRight * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ABRCollition : public AState
{
private:
	ABRCollition(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRCollition(const ABRCollition &) {}
	const ABRCollition & operator=(const ABRCollition &) {}
public:
	~ABRCollition(void);

	static ABRCollition * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};





class ABRFoul : public AState
{
private:
	ABRFoul(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRFoul(const ABRFoul &) {}
	const ABRFoul & operator=(const ABRFoul &) {}
public:
	~ABRFoul(void);

	static ABRFoul * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ABRScrape : public AState
{
private:
	ABRScrape(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRScrape(const ABRScrape &) {}
	const ABRScrape & operator=(const ABRScrape &) {}
public:
	~ABRScrape(void);

	static ABRScrape * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ABRWin : public AState
{
private:
	ABRWin(void) {AnimType = RJ_Config::RJ_ANIM_BROOM;}
	ABRWin(const ABRWin &) {}
	const ABRWin & operator=(const ABRWin &) {}
public:
	~ABRWin(void);

	static ABRWin * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};
#endif