#include "AnthonyAnimStates.h"
#include "../../CoreTech/RedJaguarEngine/Animations/AnimManager.h"
#include "../../CoreTech/RedJaguarEngine/Event/EventDefinitions.h"
#include "../../CoreTech/RedJaguarEngine/Event/EventManager.h"

class AnimMesh;

AnimAnthony::AnimAnthony()
{
	ARideAnim = NULL;
	AIdleAnim = NULL;
	APushBack = NULL;
	AtoGummyAnim = NULL;
	ALaunchAnim = NULL;
	ASweepForwardAnim = NULL;
	ASweepLeftAnim = NULL;
	ASweepRightAnim = NULL;
	ACollitionAnim = NULL;

	m_dCurrAnimRunTime = 0.0;
	m_AStateMachine = new AnimStateMachine();
	m_bSweepEnd = false;
	m_bLaunchDone = false;
}
AnimAnthony::~AnimAnthony()
{
	delete m_AStateMachine;
}
void AnimAnthony::Init()
{
	m_AStateMachine->Init();
	AIdleAnim = AAnthIdle::GetState();
	ARideAnim = AAnthIdleRide::GetState();
	AtoGummyAnim = AAnthtoGummy::GetState();
	APushBack = AAnthPushBack::GetState();
	ALaunchAnim = AAnthLaunch::GetState();
	ASweepForwardAnim = AAnthSweepForward::GetState();
	ASweepLeftAnim = AAnthSweepLeft::GetState();
	ASweepRightAnim = AAnthSweepRight::GetState();
	ACollitionAnim = AAnthCollition::GetState();
	m_AStateMachine->ChangeState(AIdleAnim);
	AnthAnimState = RJ_Config::ANTH_ANIM_IDLE;
	PrevAnthAnimState = RJ_Config::ANTH_ANIM_IDLE;
	m_dCurrAnimRunTime = 0.0;
	m_bSweepEnd = false;
	m_bLaunchDone = false;
}
void AnimAnthony::Update(double dt)
{
	double dfinaltime = dt;
	if(AnthAnimState == RJ_Config::ANTH_ANIM_PUSHBACK)
	{
		if(m_dCurrAnimRunTime > 3.55)
			m_bReversePushback = true;
		else if(m_dCurrAnimRunTime <= 0.05)
			m_bReversePushback = false;
		if(m_bReversePushback)
			dfinaltime = -dfinaltime;
	}
	else if(AnthAnimState == RJ_Config::ANTH_ANIM_IDLE)
	{
		m_bLaunchDone = false;
	}
	else if(AnthAnimState == RJ_Config::ANTH_ANIM_IDLETOGUMMY)
	{
		m_bLaunchDone = false;
		if(m_dCurrAnimRunTime > 0.95)
		{
			m_AStateMachine->ChangeState(AAnthPushBack::GetState());
			PrevAnthAnimState = AnthAnimState;
			AnthAnimState = RJ_Config::ANTH_ANIM_PUSHBACK;
			m_dCurrAnimRunTime = 0;
			m_bReversePushback = false;
		}
	}
	else if(AnthAnimState == RJ_Config::ANTH_ANIM_LAUNCH)
	{
		if(m_dCurrAnimRunTime < 3.7)
		{
			dfinaltime += ((3.7-m_dCurrAnimRunTime)*0.1);
			m_dCurrAnimRunTime += 3.7 - m_dCurrAnimRunTime;
		}
		else if(m_dCurrAnimRunTime > 4.54)
		{
			m_AStateMachine->ChangeState(AAnthIdleRide::GetState());
			PrevAnthAnimState = AnthAnimState;
			AnthAnimState = RJ_Config::ANTH_ANIM_IDLERIDE;
			m_dCurrAnimRunTime = 0.0;
			m_bLaunchDone = true;

			volatile tPlaySoundEnum* tSOUND = new tPlaySoundEnum(EM_PLAYSOUNDENUM, RJ_Config::RJ_ANTHONY_MOUNT,true); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_PLAYSOUND,(tBaseEventData*)tSOUND);
			tSOUND = NULL;
			volatile tAnimCollisionDone* tACD = new tAnimCollisionDone(EM_ANIMCOLLISIONDONE);
			CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_ANIMCOLLISIONDONE,(tBaseEventData*)tACD);
			tACD = NULL;
		}
	}
	else if(AnthAnimState == RJ_Config::ANTH_ANIM_SWEEPFORWARD || AnthAnimState == RJ_Config::ANTH_ANIM_SWEEPLEFT ||
		AnthAnimState == RJ_Config::ANTH_ANIM_SWEEPRIGHT || AnthAnimState == RJ_Config::ANTH_ANIM_SCRAPE || 
		AnthAnimState == RJ_Config::ANTH_ANIM_ENDSWEEP)
	{
		if(!m_bSweepEnd)
		{
			if(m_dCurrAnimRunTime >= 1.66)
				m_bReversePushback = true;
			else if(m_dCurrAnimRunTime <= 0.66)
				m_bReversePushback = false;
			if(m_bReversePushback)
				dfinaltime = -dfinaltime;
		}
		else
		{
			if(m_dCurrAnimRunTime < 1.66)
			{
				AnthAnimState = RJ_Config::ANTH_ANIM_ENDSWEEP;
				dfinaltime += (1.66 - m_dCurrAnimRunTime);
				if(dfinaltime < FLT_EPSILON)
					dfinaltime = FLT_EPSILON;
			}
			else if(m_dCurrAnimRunTime > 1.9)
			{
				m_AStateMachine->ChangeState(AAnthIdleRide::GetState());
				PrevAnthAnimState = AnthAnimState;
				AnthAnimState = RJ_Config::ANTH_ANIM_IDLERIDE;
				m_dCurrAnimRunTime = 0.0;
				m_bSweepEnd = false;
			}
		}
	}
	else if(AnthAnimState == RJ_Config::ANTH_ANIM_COLLITION)
	{
		if(m_dCurrAnimRunTime > 0.65)
		{
			m_AStateMachine->ChangeState(AAnthIdleRide::GetState());
			PrevAnthAnimState = AnthAnimState;
			AnthAnimState = RJ_Config::ANTH_ANIM_IDLERIDE;
			m_dCurrAnimRunTime = 0.0;
			volatile tAnimCollisionDone* tACD = new tAnimCollisionDone(EM_ANIMCOLLISIONDONE);
			CEventManager::GetInstanceThreaded()->SendEventRenderCrossToUpdate(ET_ANIMCOLLISIONDONE,(tBaseEventData*)tACD);
			tACD = NULL;
		}
	}
	else if(AnthAnimState == RJ_Config::ANTH_ANIM_WIN)
	{

	}
	else if(AnthAnimState == RJ_Config::ANTH_ANIM_FOUL)
	{

	}
	if((m_dCurrAnimRunTime + dfinaltime) < 0.0)
		dfinaltime = -(m_dCurrAnimRunTime + dfinaltime);
	if(dfinaltime < FLT_EPSILON && dfinaltime > -FLT_EPSILON)
		dfinaltime = FLT_EPSILON;
	m_dCurrAnimRunTime += dfinaltime;
	m_AStateMachine->Update((float)dfinaltime);
}



AAnthIdleRide::~AAnthIdleRide(void)
{
}
AAnthIdleRide * AAnthIdleRide::GetState()
{
	static AAnthIdleRide state;
	return &state;
}
void AAnthIdleRide::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_IdleRide"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthIdleRide::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthIdleRide::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}



AAnthIdle::~AAnthIdle(void)
{

}
AAnthIdle * AAnthIdle::GetState()
{
	static AAnthIdle state;
	return &state;
}
void AAnthIdle::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_Idle"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthIdle::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthIdle::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}




AAnthPushBack::~AAnthPushBack(void)
{
}
AAnthPushBack * AAnthPushBack::GetState()
{
	static AAnthPushBack state;
	return &state;
}
void AAnthPushBack::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_PushBack"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthPushBack::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthPushBack::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}



AAnthLaunch::~AAnthLaunch(void)
{
}
AAnthLaunch * AAnthLaunch::GetState()
{
	static AAnthLaunch state;
	return &state;
}
void AAnthLaunch::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_Launch"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthLaunch::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthLaunch::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

AAnthtoGummy::~AAnthtoGummy(void)
{
}
AAnthtoGummy * AAnthtoGummy::GetState()
{
	static AAnthtoGummy state;
	return &state;
}
void AAnthtoGummy::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_ToGummy"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthtoGummy::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthtoGummy::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}


AAnthSweepForward::~AAnthSweepForward(void)
{
}
AAnthSweepForward * AAnthSweepForward::GetState()
{
	static AAnthSweepForward state;
	return &state;
}
void AAnthSweepForward::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_SweepForward"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthSweepForward::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthSweepForward::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

AAnthSweepLeft::~AAnthSweepLeft(void)
{
}
AAnthSweepLeft * AAnthSweepLeft::GetState()
{
	static AAnthSweepLeft state;
	return &state;
}
void AAnthSweepLeft::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_SweepLeft"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthSweepLeft::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthSweepLeft::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

AAnthSweepRight::~AAnthSweepRight(void)
{
}
AAnthSweepRight * AAnthSweepRight::GetState()
{
	static AAnthSweepRight state;
	return &state;
}
void AAnthSweepRight::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_SweepRight"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthSweepRight::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthSweepRight::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

AAnthCollition::~AAnthCollition(void)
{
}
AAnthCollition * AAnthCollition::GetState()
{
	static AAnthCollition state;
	return &state;
}
void AAnthCollition::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_Collition"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthCollition::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthCollition::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}





AAnthFoul::~AAnthFoul(void)
{
}
AAnthFoul * AAnthFoul::GetState()
{
	static AAnthFoul state;
	return &state;
}
void AAnthFoul::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_Foul"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthFoul::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthFoul::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}
AAnthScrape::~AAnthScrape(void)
{
}
AAnthScrape * AAnthScrape::GetState()
{
	static AAnthScrape state;
	return &state;
}
void AAnthScrape::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_Scrape"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthScrape::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthScrape::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

AAnthWin::~AAnthWin(void)
{
}
AAnthWin * AAnthWin::GetState()
{
	static AAnthWin state;
	return &state;
}
void AAnthWin::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Anthony_Win"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AAnthWin::Execute(AnimStateMachine * pStateMachine, float fTime)
{
	pStateMachine->GetAnimObject().m_Interpolator.AddTime( fTime );
	pStateMachine->GetAnimObject().m_Interpolator.Process();

	int _frameListCount = (int)pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames().size();
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->GetAnimObject().m_vBlendedFrames.resize(_frameListCount);

	for(int i=0;i<_frameListCount;++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames[i]= pStateMachine->GetAnimObject().m_Interpolator.GetCurrentBoneFrames()[i].GetWorldMatDX();

}
void AAnthWin::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}