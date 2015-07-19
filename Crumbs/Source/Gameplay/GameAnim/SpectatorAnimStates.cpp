#include "SpectatorAnimStates.h"
#include "../../CoreTech/RedJaguarEngine/Animations/AnimManager.h"

AnimSpectator::AnimSpectator()
{
	m_dCurrAnimRunTime = 0.0;
	m_AStateMachine = new AnimStateMachine();
	ASTCheer1Anim = NULL;
	ASTCheer2Anim = NULL;
	ASTCheer3Anim = NULL;
	ASTCheer4Anim = NULL;
}
AnimSpectator::~AnimSpectator()
{
	delete m_AStateMachine;
}
void AnimSpectator::Init()
{
	m_AStateMachine->Init();
	ASTCheer1Anim = ASTCheer1::GetState();
	ASTCheer2Anim = ASTCheer2::GetState();
	m_AStateMachine->ChangeState(ASTCheer1Anim);
	SpecAnimState = RJ_Config::SPEC_ANIM_CHEER1;
	PrevSpecAnimState = RJ_Config::SPEC_ANIM_CHEER1;
	m_dCurrAnimRunTime = 0.0;
}
void AnimSpectator::Update(double dt)
{
	double dfinaltime = dt;
	m_dCurrAnimRunTime += dfinaltime;
	m_AStateMachine->Update((float)dfinaltime);
}



ASTCheer1::~ASTCheer1(void)
{
}
ASTCheer1 * ASTCheer1::GetState()
{
	static ASTCheer1 state;
	return &state;
}
void ASTCheer1::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Spectator_Cheer1"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ASTCheer1::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ASTCheer1::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ASTCheer2::~ASTCheer2(void)
{
}
ASTCheer2 * ASTCheer2::GetState()
{
	static ASTCheer2 state;
	return &state;
}
void ASTCheer2::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Spectator_Cheer2"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ASTCheer2::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ASTCheer2::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ASTCheer3::~ASTCheer3(void)
{
}
ASTCheer3 * ASTCheer3::GetState()
{
	static ASTCheer3 state;
	return &state;
}
void ASTCheer3::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Spectator_Cheer4"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ASTCheer3::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ASTCheer3::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}

ASTCheer4::~ASTCheer4(void)
{
}
ASTCheer4 * ASTCheer4::GetState()
{
	static ASTCheer4 state;
	return &state;
}
void ASTCheer4::Enter(AnimStateMachine * pStateMachine)
{
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimation(AnimationManager::GetInstanceRender()->GetAnimation("A_Spectator_Cheer3"));
	pStateMachine->GetAnimObject().m_Interpolator.SetIsLooping(true);
	pStateMachine->GetAnimObject().m_Interpolator.SetAnimSpeed(1.0f);
}
void ASTCheer4::Execute(AnimStateMachine * pStateMachine, float fTime)
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
void ASTCheer4::Exit(AnimStateMachine * pStateMachine)
{
	int nBlendSize = pStateMachine->GetAnimObject().m_vBlendedFrames.size();
	for(int i = 0; i < nBlendSize; ++i)
		pStateMachine->GetAnimObject().m_vBlendedFrames.pop_front();
	pStateMachine->GetAnimObject().m_vBlendedFrames.clear();
	pStateMachine->SetPreviousState(this);
}