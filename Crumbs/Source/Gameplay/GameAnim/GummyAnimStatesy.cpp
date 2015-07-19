#include "GummyAnimStates.h"
#include "../../CoreTech/RedJaguarEngine/Animations/AnimManager.h"

class AnimMesh;

AnimGummyWorm::AnimGummyWorm()
{
	AGWLaunchAnim = NULL;
	AGWIdleAnim = NULL;
	m_bReleaseLaunch = false;
	m_dCurrAnimRunTime = 0.0;
	m_AStateMachine = new AnimStateMachine();
}
AnimGummyWorm::~AnimGummyWorm()
{
	delete m_AStateMachine;
}
void AnimGummyWorm::Init()
{
	m_AStateMachine->Init();
	AGWLaunchAnim = AGWLaunch::GetState();
	AGWIdleAnim = AGWIdle::GetState();
	AGWIdle::GetState();
	m_AStateMachine->ChangeState(AGWIdleAnim);
	GummyAnimState = RJ_Config::GUMMY_ANIM_IDLE;
	PrevGummyAnimState = RJ_Config::GUMMY_ANIM_IDLE;
	m_dCurrAnimRunTime = 0.0;
	m_bReleaseLaunch = false;
}
void AnimGummyWorm::Update(double dt)
{
	double dfinaltime = dt;
	if(GummyAnimState == RJ_Config::GUMMY_ANIM_LAUNCH)
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
			{
				dfinaltime = dfinaltime + (4.7 - m_dCurrAnimRunTime); 
				if(dfinaltime >= 0.1)
					dfinaltime += ((4.7-m_dCurrAnimRunTime)*0.1)-0.1;
			}
			if(m_dCurrAnimRunTime >= 6.0f)
			{
				m_bReversePushback = false;
				m_bReleaseLaunch = false;
				m_AStateMachine->ChangeState(AGWIdle::GetState());
				PrevGummyAnimState = GummyAnimState;
				GummyAnimState = RJ_Config::GUMMY_ANIM_IDLE;
				m_dCurrAnimRunTime = 0;
			}
		}
	}
	m_dCurrAnimRunTime += dfinaltime;
	m_AStateMachine->Update((float)dfinaltime);
}



AGWLaunch::~AGWLaunch(void)
{
}
AGWLaunch * AGWLaunch::GetState()
{
	static AGWLaunch state;
	return &state;
}
void AGWLaunch::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Gummy_Launch"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AGWLaunch::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void AGWLaunch::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

AGWIdle::~AGWIdle(void)
{
}
AGWIdle * AGWIdle::GetState()
{
	static AGWIdle state;
	return &state;
}
void AGWIdle::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Gummy_Idle"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void AGWIdle::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void AGWIdle::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}