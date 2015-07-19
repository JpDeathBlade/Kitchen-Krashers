#ifndef _ASTATE_H_
#define _ASTATE_H_
#include "../../RJ_Config.h"

class AnimStateMachine;
class CInterpolator;
class AnimMesh;

class AState
{
	//protected:
	//	AnimationBlender m_Blender;
public:
	RJ_Config::ANIM_TYPE AnimType;
	AState(void) {AnimType = RJ_Config::RJ_ANIM_NA;}

	virtual void Enter(AnimStateMachine * pStateMachine) = 0;
	virtual void Execute(AnimStateMachine * pStateMachine, float fTime) = 0;
	virtual void Exit(AnimStateMachine * pStateMachine) = 0;

	inline void SetType(RJ_Config::ANIM_TYPE type) {AnimType = type;}
	inline RJ_Config::ANIM_TYPE GetType() {return AnimType;}
	virtual ~AState(void){}
};

class AnimLink
{
	public:
	AnimLink(){AMesh = 0;}
	virtual ~AnimLink(){}

	virtual void Init() = 0;
	virtual void Update(double dt) = 0;

	RJ_Config::ANIM_TYPE AnimType;
	int		nLinkedID;
	AnimMesh* AMesh;
	AnimStateMachine* m_AStateMachine;
	double m_dCurrAnimRunTime;
};
#endif