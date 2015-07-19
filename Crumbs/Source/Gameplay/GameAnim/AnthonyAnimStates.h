#ifndef _ANTHONYANIMSTATES_H_
#define _ANTHONYANIMSTATES_H_

#include "AState.h"
#include "../Objects/AnimMesh.h"

class AAnthIdleRide;
class AAnthIdle;
class AAnthLaunch;
class AAnthPushBack;
class AAnthtoGummy;
class AAnthSweepForward;
class AAnthSweepLeft;
class AAnthSweepRight;
class AAnthCollition;
class AAnthFoul;
class AAnthScrape;
class AAnthWin;

class AnimAnthony : public AnimLink
{
	AAnthIdleRide* ARideAnim;
	AAnthIdle* AIdleAnim;
	AAnthPushBack* APushBack;
	AAnthLaunch* ALaunchAnim;
	AAnthtoGummy* AtoGummyAnim;
	AAnthSweepForward* ASweepForwardAnim;
	AAnthSweepLeft* ASweepLeftAnim;
	AAnthSweepRight* ASweepRightAnim;
	AAnthCollition* ACollitionAnim;
	AAnthFoul* AFoulAnim;
	AAnthScrape* AScrapeAnim;
	AAnthWin* AWinAnim;

	bool m_bReversePushback;
public:
	bool m_bSweepEnd;
	bool m_bLaunchDone;
	//enum ANTH_HATS{AH_NOHAT, AH_WHIPCREAM, AH_FORTUNECOOKIE, AH_CHERRY, AH_COLDTURKEY, AH_GUM};
	//ANTH_HATS AHCurrentHat;
	RJ_Config::ANTHONY_ANIM_STATES AnthAnimState;
	RJ_Config::ANTHONY_ANIM_STATES PrevAnthAnimState;
	AnimAnthony();
	~AnimAnthony();
	void Update(double dt);
	void Init();
};


class AAnthIdleRide :	public AState
{
private:
	AAnthIdleRide(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthIdleRide(const AAnthIdleRide &) {}
	const AAnthIdleRide & operator=(const AAnthIdleRide &) {}
public:
	~AAnthIdleRide(void);

	static AAnthIdleRide * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class AAnthIdle :	public AState
{
private:
	AAnthIdle(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthIdle(const AAnthIdle &) {}
	const AAnthIdle & operator=(const AAnthIdle &) {}
public:
	~AAnthIdle(void);

	static AAnthIdle * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class AAnthPushBack :	public AState
{
private:
	AAnthPushBack(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthPushBack(const AAnthPushBack &) {}
	const AAnthPushBack & operator=(const AAnthPushBack &) {}
public:
	~AAnthPushBack(void);

	static AAnthPushBack * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class AAnthLaunch :	public AState
{
private:
	AAnthLaunch(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthLaunch(const AAnthLaunch &) {}
	const AAnthLaunch & operator=(const AAnthLaunch &) {}
public:
	~AAnthLaunch(void);

	static AAnthLaunch * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class AAnthtoGummy : public AState
{
private:
	AAnthtoGummy(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthtoGummy(const AAnthtoGummy &) {}
	const AAnthtoGummy & operator=(const AAnthtoGummy &) {}
public:
	~AAnthtoGummy(void);

	static AAnthtoGummy * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class AAnthSweepForward : public AState
{
private:
	AAnthSweepForward(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthSweepForward(const AAnthSweepForward &) {}
	const AAnthSweepForward & operator=(const AAnthSweepForward &) {}
public:
	~AAnthSweepForward(void);

	static AAnthSweepForward * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class AAnthSweepLeft : public AState
{
private:
	AAnthSweepLeft(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthSweepLeft(const AAnthSweepLeft &) {}
	const AAnthSweepLeft & operator=(const AAnthSweepLeft &) {}
public:
	~AAnthSweepLeft(void);

	static AAnthSweepLeft * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class AAnthSweepRight : public AState
{
private:
	AAnthSweepRight(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthSweepRight(const AAnthSweepRight &) {}
	const AAnthSweepRight & operator=(const AAnthSweepRight &) {}
public:
	~AAnthSweepRight(void);

	static AAnthSweepRight * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class AAnthCollition : public AState
{
private:
	AAnthCollition(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthCollition(const AAnthCollition &) {}
	const AAnthCollition & operator=(const AAnthCollition &) {}
public:
	~AAnthCollition(void);

	static AAnthCollition * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};








class AAnthFoul : public AState
{
private:
	AAnthFoul(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthFoul(const AAnthFoul &) {}
	const AAnthFoul & operator=(const AAnthFoul &) {}
public:
	~AAnthFoul(void);

	static AAnthFoul * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class AAnthScrape : public AState
{
private:
	AAnthScrape(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthScrape(const AAnthScrape &) {}
	const AAnthScrape & operator=(const AAnthScrape &) {}
public:
	~AAnthScrape(void);

	static AAnthScrape * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class AAnthWin : public AState
{
private:
	AAnthWin(void) {AnimType = RJ_Config::RJ_ANIM_ANTHONY;}
	AAnthWin(const AAnthWin &) {}
	const AAnthWin & operator=(const AAnthWin &) {}
public:
	~AAnthWin(void);

	static AAnthWin * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};
#endif