#include "BroomAnimStates.h"
#include "../../CoreTech/RedJaguarEngine/Animations/AnimManager.h"

class AnimMesh;

AnimBroom::AnimBroom()
{
	ABRLaunchAnim = NULL;
	ABRIdleAnim = NULL;
	ABRtoGummyAnim = NULL;
	ABRPushBackAnim = NULL;
	ABRIdleRideAnim = NULL;
	ABRSweepForwardAnim = NULL;
	ABRSweepLeftAnim = NULL;
	ABRSweepRightAnim = NULL;
	ABRCollitionAnim = NULL;
	m_dCurrAnimRunTime = 0.0;
	m_bSweepEnd = false;
	m_AStateMachine = new AnimStateMachine();
}
AnimBroom::~AnimBroom()
{
	delete m_AStateMachine;
}
void AnimBroom::Init()
{
	m_AStateMachine->Init();
	ABRLaunchAnim = ABRLaunch::GetState();
	ABRIdleAnim = ABRIdle::GetState();
	ABRtoGummyAnim = ABRtoGummy::GetState();
	ABRPushBackAnim = ABRPushBack::GetState();
	ABRIdleRideAnim = ABRIdleRide::GetState();
	ABRSweepForwardAnim = ABRSweepForward::GetState();
	ABRSweepLeftAnim = ABRSweepLeft::GetState();
	ABRSweepRightAnim = ABRSweepRight::GetState();
	ABRCollitionAnim = ABRCollition::GetState();
	m_dCurrAnimRunTime = 0.0;
	m_AStateMachine->ChangeState(ABRIdleAnim);
	BroomAnimState = RJ_Config::BROOM_ANIM_IDLE;
	m_bSweepEnd = false;
}
void AnimBroom::Update(double dt)
{
	double dfinaltime = dt;
	if(BroomAnimState == RJ_Config::BROOM_ANIM_PUSHBACK)
	{
		if(m_dCurrAnimRunTime > 3.55)
			m_bReversePushback = true;
		else if(m_dCurrAnimRunTime <= 0.05)
			m_bReversePushback = false;
		if(m_bReversePushback)
			dfinaltime = -dfinaltime;
	}
	else if(BroomAnimState == RJ_Config::BROOM_ANIM_IDLETOGUMMY)
	{
		if(m_dCurrAnimRunTime > 0.95)
		{
			m_AStateMachine->ChangeState(ABRPushBack::GetState());
			PrevBroomAnimState = BroomAnimState;
			BroomAnimState = RJ_Config::BROOM_ANIM_PUSHBACK;
			m_dCurrAnimRunTime = 0;
			m_bReversePushback = false;
		}
	}
	else if(BroomAnimState == RJ_Config::BROOM_ANIM_LAUNCH)
	{
		if(m_dCurrAnimRunTime < 3.7)
		{
			dfinaltime += ((3.7-m_dCurrAnimRunTime)*0.1);
			m_dCurrAnimRunTime += 3.7 - m_dCurrAnimRunTime;
			if(dfinaltime <= 0.0)
				dfinaltime = dt;
		}
		else if(m_dCurrAnimRunTime > 4.54)
		{
			m_AStateMachine->ChangeState(ABRIdleRide::GetState());
			PrevBroomAnimState = BroomAnimState;
			BroomAnimState = RJ_Config::BROOM_ANIM_IDLERIDE;
			m_dCurrAnimRunTime = 0;
		}
	}
	else if(BroomAnimState == RJ_Config::BROOM_ANIM_SWEEPFORWARD || BroomAnimState == RJ_Config::BROOM_ANIM_SWEEPLEFT ||
		BroomAnimState == RJ_Config::BROOM_ANIM_SWEEPRIGHT || BroomAnimState == RJ_Config::BROOM_ANIM_SCRAPE ||
		BroomAnimState == RJ_Config::BROOM_ANIM_ENDSWEEP)
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
				BroomAnimState = RJ_Config::BROOM_ANIM_ENDSWEEP;
				dfinaltime += (1.66 - m_dCurrAnimRunTime);
				if(dfinaltime < FLT_EPSILON)
					dfinaltime = FLT_EPSILON;
			}
			else if(m_dCurrAnimRunTime > 1.9)
			{
				m_AStateMachine->ChangeState(ABRIdleRide::GetState());
				PrevBroomAnimState = BroomAnimState;
				BroomAnimState = RJ_Config::BROOM_ANIM_IDLERIDE;
				m_dCurrAnimRunTime = 0;
				m_bSweepEnd = false;
			}
		}
	}
	else if(BroomAnimState == RJ_Config::BROOM_ANIM_COLLITION)
	{
		if(m_dCurrAnimRunTime > 0.65)
		{
			m_AStateMachine->ChangeState(ABRIdleRide::GetState());
			PrevBroomAnimState = BroomAnimState;
			BroomAnimState = RJ_Config::BROOM_ANIM_IDLERIDE;
			m_dCurrAnimRunTime = 0;
		}
	}
	else if(BroomAnimState == RJ_Config::BROOM_ANIM_WIN)
	{

	}
	else if(BroomAnimState == RJ_Config::BROOM_ANIM_FOUL)
	{

	}
	if((m_dCurrAnimRunTime + dfinaltime) < 0.0)
		dfinaltime = -(m_dCurrAnimRunTime + dfinaltime);
	if(dfinaltime == 0.0)
		dfinaltime = FLT_EPSILON;
	m_dCurrAnimRunTime += dfinaltime;
	m_AStateMachine->Update((float)dfinaltime);
}



ABRLaunch::~ABRLaunch(void)
{
}
ABRLaunch * ABRLaunch::GetState()
{
	static ABRLaunch state;
	return &state;
}
void ABRLaunch::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_Launch"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRLaunch::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRLaunch::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ABRIdle::~ABRIdle(void)
{
}
ABRIdle * ABRIdle::GetState()
{
	static ABRIdle state;
	return &state;
}
void ABRIdle::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_Idle"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRIdle::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRIdle::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ABRIdleRide::~ABRIdleRide(void)
{
}
ABRIdleRide * ABRIdleRide::GetState()
{
	static ABRIdleRide state;
	return &state;
}
void ABRIdleRide::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_IdleRide"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRIdleRide::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRIdleRide::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ABRPushBack::~ABRPushBack(void)
{
}
ABRPushBack * ABRPushBack::GetState()
{
	static ABRPushBack state;
	return &state;
}
void ABRPushBack::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_PushBack"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRPushBack::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRPushBack::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ABRtoGummy::~ABRtoGummy(void)
{
}
ABRtoGummy * ABRtoGummy::GetState()
{
	static ABRtoGummy state;
	return &state;
}
void ABRtoGummy::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_ToGummy"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRtoGummy::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRtoGummy::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ABRSweepForward::~ABRSweepForward(void)
{
}
ABRSweepForward * ABRSweepForward::GetState()
{
	static ABRSweepForward state;
	return &state;
}
void ABRSweepForward::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_SweepForward"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRSweepForward::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRSweepForward::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ABRSweepLeft::~ABRSweepLeft(void)
{
}
ABRSweepLeft * ABRSweepLeft::GetState()
{
	static ABRSweepLeft state;
	return &state;
}
void ABRSweepLeft::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_SweepLeft"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRSweepLeft::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRSweepLeft::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ABRSweepRight::~ABRSweepRight(void)
{
}
ABRSweepRight * ABRSweepRight::GetState()
{
	static ABRSweepRight state;
	return &state;
}
void ABRSweepRight::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_SweepRight"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRSweepRight::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRSweepRight::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ABRCollition::~ABRCollition(void)
{
}
ABRCollition * ABRCollition::GetState()
{
	static ABRCollition state;
	return &state;
}
void ABRCollition::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_Collision"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRCollition::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRCollition::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}




ABRFoul::~ABRFoul(void)
{
}
ABRFoul * ABRFoul::GetState()
{
	static ABRFoul state;
	return &state;
}
void ABRFoul::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_Foul"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRFoul::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRFoul::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ABRScrape::~ABRScrape(void)
{
}
ABRScrape * ABRScrape::GetState()
{
	static ABRScrape state;
	return &state;
}
void ABRScrape::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_Scrape"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRScrape::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRScrape::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ABRWin::~ABRWin(void)
{
}
ABRWin * ABRWin::GetState()
{
	static ABRWin state;
	return &state;
}
void ABRWin::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Broom_Win"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ABRWin::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ABRWin::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}