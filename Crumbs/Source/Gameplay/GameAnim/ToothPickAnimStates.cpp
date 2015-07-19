#include "ToothPickAnimStates.h"
#include "../../CoreTech/RedJaguarEngine/Animations/AnimManager.h"

class AnimMesh;

AnimToothPick::AnimToothPick()
{
	ATPLaunchAnim = NULL;
	ATPIdleAnim = NULL;
	m_bReleaseLaunch = false;
	m_dCurrAnimRunTime = 0.0;
	m_AStateMachine = new AnimStateMachine();
}
AnimToothPick::~AnimToothPick()
{
	delete m_AStateMachine;
}
void AnimToothPick::Init()
{
	m_AStateMachine->Init();
	ATPLaunchAnim = ATPLaunch::GetState();
	ATPIdleAnim = ATPIdle::GetState();
	m_AStateMachine->ChangeState(ATPIdleAnim);
	ToothAnimState = RJ_Config::TOOTH_ANIM_IDLE;
	PrevToothAnimState = RJ_Config::TOOTH_ANIM_IDLE;
	m_bReleaseLaunch = false;
	m_dCurrAnimRunTime = 0.0;
}
void AnimToothPick::Update(double dt)
{
	double dfinaltime = dt;
	if(ToothAnimState == RJ_Config::TOOTH_ANIM_LAUNCH)
	{
		if(m_bReleaseLaunch == false)
		{
			if(m_dCurrAnimRunTime > 4.55)
  				m_bReversePushback = true;
			else if(m_dCurrAnimRunTime <= 1.05)
				m_bReversePushback = false;
			if(m_bReversePushback)
				dfinaltime = -dfinaltime;
		}
		else
		{
			if(m_dCurrAnimRunTime <= 4.7)
			{	dfinaltime = dfinaltime + (4.7 - m_dCurrAnimRunTime); 
				if(dfinaltime >= 0.1)
					dfinaltime += ((4.7-m_dCurrAnimRunTime)*0.1)-0.1;
			}
			if(m_dCurrAnimRunTime >= 6.0f)
			{
				m_bReversePushback = false;
				m_bReleaseLaunch = false;
				m_AStateMachine->ChangeState(ATPIdle::GetState());
				PrevToothAnimState = ToothAnimState;
				ToothAnimState = RJ_Config::TOOTH_ANIM_IDLE;
				m_dCurrAnimRunTime = 0;
			}
		}
	}
	m_dCurrAnimRunTime += dfinaltime;
	m_AStateMachine->Update((float)dfinaltime);
}



ATPLaunch::~ATPLaunch(void)
{
}
ATPLaunch * ATPLaunch::GetState()
{
	static ATPLaunch state;
	return &state;
}
void ATPLaunch::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_ToothPick_Launch"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ATPLaunch::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ATPLaunch::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ATPIdle::~ATPIdle(void)
{
}
ATPIdle * ATPIdle::GetState()
{
	static ATPIdle state;
	return &state;
}
void ATPIdle::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_ToothPick_Idle"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ATPIdle::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ATPIdle::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}