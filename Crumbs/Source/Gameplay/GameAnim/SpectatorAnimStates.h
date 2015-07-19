#ifndef _SPECTATORS_H_
#define _SPECTATORS_H_

#include "AState.h"
#include "../Objects/AnimMesh.h"

class ASTCheer1;
class ASTCheer2;
class ASTCheer3;
class ASTCheer4;

class AnimSpectator : public AnimLink
{
	ASTCheer1* ASTCheer1Anim;
	ASTCheer2* ASTCheer2Anim;
	ASTCheer3* ASTCheer3Anim;
	ASTCheer4* ASTCheer4Anim;
	bool m_bReversePushback;
public:
	RJ_Config::SPECTATOR_ANIM_STATES SpecAnimState;
	RJ_Config::SPECTATOR_ANIM_STATES PrevSpecAnimState;
	AnimSpectator();
	~AnimSpectator();
	void Update(double dt);
	void Init();
};


class ASTCheer1 :	public AState
{
private:
	ASTCheer1(void) {AnimType = RJ_Config::RJ_SPECTATOR;}
	ASTCheer1(const ASTCheer1 &) {}
	const ASTCheer1 & operator=(const ASTCheer1 &) {}
public:
	~ASTCheer1(void);

	static ASTCheer1 * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ASTCheer2 :	public AState
{
private:
	ASTCheer2(void) {AnimType = RJ_Config::RJ_SPECTATOR;}
	ASTCheer2(const ASTCheer2 &) {}
	const ASTCheer2 & operator=(const ASTCheer2 &) {}
public:
	~ASTCheer2(void);

	static ASTCheer2 * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ASTCheer3 :	public AState
{
private:
	ASTCheer3(void) {AnimType = RJ_Config::RJ_SPECTATOR;}
	ASTCheer3(const ASTCheer3 &) {}
	const ASTCheer3 & operator=(const ASTCheer3 &) {}
public:
	~ASTCheer3(void);

	static ASTCheer3 * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

class ASTCheer4 :	public AState
{
private:
	ASTCheer4(void) {AnimType = RJ_Config::RJ_SPECTATOR;}
	ASTCheer4(const ASTCheer4 &) {}
	const ASTCheer4 & operator=(const ASTCheer4 &) {}
public:
	~ASTCheer4(void);

	static ASTCheer4 * GetState();

	virtual void Enter(AnimStateMachine * pStateMachine);
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime);
	virtual void Exit(AnimStateMachine * pStateMachine);
};

#endif