#include "AnimManager.h"
#include <fstream>
using std::ifstream;
using std::fstream;

AnimationManager::AnimationManager(void)
{
	m_nAnimationSize = 0;

}

AnimationManager * AnimationManager::GetInstanceRender()
{
	static AnimationManager instance;
	return &instance;
}

AnimationManager::~AnimationManager(void)
{
	Shutdown();
}
CAnimation::CAnimation()
{
	m_fDuration = 0.0f;
	m_fPreHitTime = 0.0f;
	m_fHitTime = 0.0f;
	m_fPostHitTime = 0.0f;


}

bool AnimationManager::AddAnimation(std::string szName)
{
	std::string szFilePath = "Resources/AnimFiles/";
	szFilePath+=szName;
	//unsigned int nAnimationSize = m_vAnimations.size();
	for(unsigned int iAnim = 0; iAnim < m_nAnimationSize; iAnim++)
	{
		if(szName == m_vAnimations[iAnim]->m_szName)
			return false;
	}
	CAnimation* temp = new CAnimation;
	//temp->LoadFromXML(szName);
	temp->LoadFromBinary(szFilePath);
	m_vAnimations.push_back(temp);
	m_nAnimationSize++;
	return true;
}

bool AnimationManager::AddAnimation(CAnimation *pAnimation)
{
	for(unsigned int iAnim = 0; iAnim < m_nAnimationSize; iAnim++)
	{
		if(pAnimation->m_szName == m_vAnimations[iAnim]->m_szName)
			return false;
	}

	m_vAnimations.push_back(pAnimation);
	m_nAnimationSize++;
	return true;
}

CAnimation * AnimationManager::RemoveAnimation(std::string szName)
{
	for(unsigned int iAnim = 0; iAnim < m_nAnimationSize; iAnim++)
	{
		if(szName == m_vAnimations[iAnim]->m_szName)	
		{
			CAnimation * pAnim = m_vAnimations[iAnim];
			m_vAnimations.erase(m_vAnimations.begin() + iAnim);
			m_nAnimationSize--;
			return pAnim;
		}
	}

	return NULL;
}

void AnimationManager::Shutdown()
{
	for(unsigned int nAnim = 0; nAnim < m_nAnimationSize; nAnim++)
	{
		delete m_vAnimations[nAnim];
		//m_nAnimationSize--;
	}

	m_vAnimations.clear();
}

void AnimationManager::Init()
{
	//ANTHONY
	AddAnimation("A_Anthony_IdleRide.cra");
	AddAnimation("A_Anthony_Launch.cra");
	AddAnimation("A_Anthony_PushBack.cra");
	AddAnimation("A_Anthony_Idle.cra");
	AddAnimation("A_Anthony_ToGummy.cra");
	AddAnimation("A_Anthony_SweepForward.cra");
	AddAnimation("A_Anthony_SweepLeft.cra");
	AddAnimation("A_Anthony_SweepRight.cra");
	AddAnimation("A_Anthony_Collition.cra");
	AddAnimation("A_Anthony_Win.cra");
	AddAnimation("A_Anthony_Scrape.cra");
	AddAnimation("A_Anthony_Foul.cra");
	//BROOM
	AddAnimation("A_Broom_IdleRide.cra");
	AddAnimation("A_Broom_Launch.cra");
	AddAnimation("A_Broom_PushBack.cra");
	AddAnimation("A_Broom_Idle.cra");
	AddAnimation("A_Broom_ToGummy.cra");
	AddAnimation("A_Broom_SweepForward.cra");
	AddAnimation("A_Broom_SweepLeft.cra");
	AddAnimation("A_Broom_SweepRight.cra");
	AddAnimation("A_Broom_Collision.cra");
	AddAnimation("A_Broom_Win.cra");
	AddAnimation("A_Broom_Scrape.cra");
	AddAnimation("A_Broom_Foul.cra");
	//GUMMY
	AddAnimation("A_Gummy_Launch.cra");
	AddAnimation("A_Gummy_Idle.cra");
	//TOOTHPICK
	AddAnimation("A_ToothPick_Launch.cra");
	AddAnimation("A_ToothPick_Idle.cra");
	//SPECTATORS
	AddAnimation("A_Spectator_Cheer1.cra");
	AddAnimation("A_Spectator_Cheer2.cra");
	AddAnimation("A_Spectator_Cheer3.cra");
	AddAnimation("A_Spectator_Cheer4.cra");
}
void CAnimation::LoadFromBinary(std::string fileName)
{
	std::fstream inputStream;
	inputStream.clear();
	inputStream.open(fileName.c_str(),std::ios_base::in | std::ios_base::binary);

	if(!inputStream.is_open())
		return;
	if(!inputStream.good())
		return;

	size_t nNameLength;
	inputStream.read((char*)&nNameLength,sizeof(size_t));
	char * name = new char[nNameLength];
	inputStream.read(name,nNameLength);
	m_szName = name;
	delete[] name;

	float duration = 0;
	m_fDuration = 0;
	//float duration=0;
	inputStream.read((char*)&m_fDuration,sizeof(float));
	//m_fDuration=duration;

	size_t numBones;
	inputStream.read((char*)&numBones,sizeof(size_t));

	for(unsigned int boneCount=0;boneCount<numBones;++boneCount)
	{
		AnimatedBone animBone;
		size_t boneNum;
		inputStream.read((char*)&boneNum,sizeof(size_t));
		animBone.BoneIndex = boneNum;
		size_t numKeys;
		inputStream.read((char*)&numKeys,sizeof(size_t));
		size_t testInt = numKeys;

		size_t nBoneNameLength;
		inputStream.read((char*)&nBoneNameLength,sizeof(size_t));
		char * Bonename = new char[nBoneNameLength];
		inputStream.read(Bonename,nBoneNameLength);
		animBone.m_szABoneName = Bonename;
		delete[] Bonename;


		if(testInt == 0)
		{
			animBone.vKeyFrames.resize(1);
			for(unsigned int irows = 0; irows < 4; irows++)
			{
				for(unsigned int icols = 0; icols < 4; icols++)
				{
					float temp; 
					inputStream.read((char*)&temp, sizeof(float));
					animBone.vKeyFrames[0].m_TransformFrame.GetWorldMatDX().m[irows][icols]= temp;
					animBone.vKeyFrames[0].m_TransformFrame.Update();
				}
			}
			m_vAnimatedBones.push_back(animBone);
			continue;
		}
		animBone.vKeyFrames.resize(numKeys);

		size_t keyNum=0;
		for(keyNum;keyNum<numKeys;keyNum++)
		{
			int currKey;
			inputStream.read((char*)&currKey,sizeof(size_t));
			//currKey-=1;
			double fKeyTime=0;
			inputStream.read((char*)&fKeyTime,sizeof(double));
			if(currKey != -1)
				animBone.vKeyFrames[currKey].m_fKeyTime = (float)fKeyTime;
			for(unsigned int irows = 0; irows < 4; irows++)
			{
				for(unsigned int icols = 0; icols < 4; icols++)
				{
					float temp; 
					inputStream.read((char*)&temp, sizeof(float));
					if(currKey != -1)
						animBone.vKeyFrames[currKey].m_TransformFrame.GetWorldMatDX().m[irows][icols]= temp;
				}
			}
			if(currKey != -1)
			{
				animBone.vKeyFrames[currKey].m_TransformFrame.SetBoneIndex(boneNum);
				animBone.vKeyFrames[currKey].m_TransformFrame.Update();
			}
			//}
		}

		m_vAnimatedBones.push_back(animBone);
	}

	D3DXMATRIX temp;
	int boneCount = m_vAnimatedBones.size();
	int vertCount;
	for(int i=0;i<boneCount;++i)
	{
		vertCount = m_vAnimatedBones[i].vKeyFrames.size();
		for(int j=0;j<vertCount;++j)
		{
			temp= m_vAnimatedBones[i].vKeyFrames[j].m_TransformFrame.GetWorldMatDX();
			D3DXMatrixDecompose(&m_vAnimatedBones[i].vKeyFrames[j].m_fQuatTransform.scale,&m_vAnimatedBones[i].vKeyFrames[j].m_fQuatTransform.rot,&m_vAnimatedBones[i].vKeyFrames[j].m_fQuatTransform.translate,&temp);
		}
	}

	inputStream.close();
	inputStream.clear();
}
CInterpolator::CInterpolator() : m_pAnimation(0)
{
	m_fAnimSpeed=1.0f;
	m_bIsDone=false; 
	m_fCurrentTime = 0.0f;
	m_bIsLooping = false;
	m_fTimeRemainingInAnim = 0.0f;
}

void CInterpolator::SetAnimation(const CAnimation *inAnimation)
{
	if(!inAnimation)
		return;
	m_pAnimation = inAnimation;

	unsigned int nAnimatedBonesSize = m_pAnimation->m_vAnimatedBones.size();
	unsigned int nPrevBoneSize = m_vBoneFrames.size();
	for(unsigned int i = 0; i < nPrevBoneSize; ++i)
		m_vBoneFrames.pop_front();
	m_vBoneFrames.clear();
	m_vBoneFrames.resize( nAnimatedBonesSize );
	for(unsigned int nFrame = 0; nFrame < nAnimatedBonesSize; nFrame++ )
	{
		m_vBoneFrames[nFrame] = m_pAnimation->m_vAnimatedBones[nFrame].vKeyFrames[0].m_TransformFrame;
	}
	SetTime(0.0f);
	m_bIsDone = false;
}

void CInterpolator::Process(void)
{
	if(!m_pAnimation)
		return;
	float aniDuration = const_cast<CAnimation*>(m_pAnimation)->GetDuration();

	if(m_bIsDone)
		return;

	if(m_fCurrentTime < FLT_EPSILON)
	{
		m_fCurrentTime = aniDuration;
	}
	if(m_fCurrentTime > aniDuration-FLT_EPSILON)
	{
		m_fCurrentTime = FLT_EPSILON;
		if(!m_bIsLooping)
		{
			m_bIsDone = true;
			m_fCurrentTime = aniDuration;
		}
	}
	//int frame=0;
	CAnimation* tempAni = const_cast<CAnimation*>(m_pAnimation);
	int nAnimatedBoneSize = tempAni->GetAnimatedBones().size();
	m_vBoneFrames.resize(nAnimatedBoneSize);

	int keyFrame =-1;
	int keyFrame2 =-1;

	float currFrameTime ;
	float nextFrameTime;
	float tweenTime;
	Bone temp;
	float keyTime;
	for(int currentFrame=0;currentFrame<nAnimatedBoneSize;++currentFrame)
	{
		int totalKeyFrameCount = (int)tempAni->GetAnimatedBones()[currentFrame].vKeyFrames.size();

		for(int keyFrameIndex=1;keyFrameIndex<totalKeyFrameCount;++keyFrameIndex)
		{
			if(keyFrame == -1)
			{
				int keyFrameNext = keyFrameIndex+1;
				if(keyFrameNext>=totalKeyFrameCount)
				{
					if(totalKeyFrameCount > 1)
						keyFrameNext = 1;
					else
						keyFrameNext = 0;
				}
				currFrameTime = tempAni->GetAnimatedBones()[currentFrame].vKeyFrames[keyFrameIndex].m_fKeyTime;
				nextFrameTime = tempAni->GetAnimatedBones()[currentFrame].vKeyFrames[keyFrameNext].m_fKeyTime;
				if(nextFrameTime > currFrameTime)
					tweenTime = nextFrameTime - currFrameTime;
				else
					tweenTime = nextFrameTime;
				if(m_fCurrentTime < currFrameTime+tweenTime)
				{
					if(tweenTime < FLT_EPSILON && tweenTime > -FLT_EPSILON)
						tweenTime = FLT_EPSILON;
					float fCFrameTime = (m_fCurrentTime-currFrameTime);
					if( fCFrameTime < FLT_EPSILON && fCFrameTime > -FLT_EPSILON)
						fCFrameTime = FLT_EPSILON;
					keyTime = fCFrameTime/tweenTime;
					//D3DXMATRIX transCurr = currFrame.m_TransformFrame.GetWorldMatDX();
					decompsedMat transCurr = tempAni->GetAnimatedBones()[currentFrame].vKeyFrames[keyFrameIndex].m_fQuatTransform;
					//D3DXMATRIX transNext = m_NextFrame.m_TransformFrame.GetWorldMatDX();
					decompsedMat transNext= tempAni->GetAnimatedBones()[currentFrame].vKeyFrames[keyFrameNext].m_fQuatTransform;
					temp.GetWorldMatDX() = interpolate(transCurr,transNext,keyTime);
					keyFrame = keyFrameIndex;
					keyFrame2 = keyFrameNext;

					m_vBoneFrames[currentFrame] = temp;
					break;
				}
				else
					continue;
			}
			else
			{
				if(keyFrame2 < totalKeyFrameCount)
				{
					//currFrame = tempAni->GetAnimatedBones()[currentFrame].vKeyFrames[keyFrame];
					//m_NextFrame = tempAni->GetAnimatedBones()[currentFrame].vKeyFrames[keyFrame2];
					//float keyTime = (m_fCurrentTime-currFrame.m_fKeyTime)/tweenTime;
					//D3DXMATRIX transCurr = currFrame.m_TransformFrame.GetWorldMatDX();
					decompsedMat transCurr = tempAni->GetAnimatedBones()[currentFrame].vKeyFrames[keyFrame].m_fQuatTransform;
					//D3DXMATRIX transNext = m_NextFrame.m_TransformFrame.GetWorldMatDX();
					decompsedMat transNext= tempAni->GetAnimatedBones()[currentFrame].vKeyFrames[keyFrame2].m_fQuatTransform;
					temp.GetWorldMatDX() = interpolate(transCurr,transNext,keyTime);
					m_vBoneFrames[currentFrame] = temp;
				}
				else
				{
					decompsedMat transCurr = tempAni->GetAnimatedBones()[currentFrame].vKeyFrames[keyFrame].m_fQuatTransform;
					//D3DXMATRIX transNext = m_NextFrame.m_TransformFrame.GetWorldMatDX();
					decompsedMat transNext= tempAni->GetAnimatedBones()[currentFrame].vKeyFrames[1].m_fQuatTransform;
					temp.GetWorldMatDX() = interpolate(transCurr,transNext,keyTime);
					m_vBoneFrames[currentFrame] = temp;
				}
				break;
			}
		}

	}


}

AnimStateMachine::AnimStateMachine(void)
{
	//m_pEntity = NULL;
	m_pCurrentState = NULL;
	m_pPreviousState = NULL;
	fTimeInCurrentState = 0.0;
	Init();
}

AnimStateMachine::~AnimStateMachine(void)
{
	m_vAnimSpeeds.clear();
}

void AnimStateMachine::Update(float fTime)
{
	fTimeInCurrentState+=fTime;
	m_pCurrentState->Execute(this,fTime);
}

void AnimStateMachine::ChangeState(AState * pNewState,bool canRepeat)
{
	if(!canRepeat)
	{
		if(m_pCurrentState == pNewState)
			return;
	}
	m_tAnimObject.m_Interpolator.SetAnimSpeed(1.0f);
	m_pPreviousState = m_pCurrentState;
	if(m_pCurrentState)
		m_pCurrentState->Exit(this);
	m_pCurrentState = pNewState;
	if(m_pCurrentState)
		m_pCurrentState->Enter(this);
	fTimeInCurrentState = 0.0;
}

void AnimStateMachine::RevertToPreviousState()
{
	ChangeState(m_pPreviousState);
}

void AnimStateMachine::Init()
{
	//float animSpeed;
	//m_vAnimSpeeds.push_back(animSpeed);
}

float AnimStateMachine::GetAnimSpeed(int id)
{
	if(id < (int)m_vAnimSpeeds.size())
		return m_vAnimSpeeds[id];
	else
		return 1.0f;
}

AState* AnimStateMachine::GetState(const char* name)
{
	//if(strcmp("NAME",name) ==0)
	//	return THESTATE::GetState();

	return NULL;
}

bool AnimStateMachine::CheckStates(const AState *pAnim1,const AState *pAnim2)
{
	bool isValidChange = false;
	if(GetCurrentState() != pAnim1)
		isValidChange = true;
	if(pAnim2)
	{
		if(GetCurrentState() != pAnim2)
			isValidChange = true;
		else
			isValidChange = false;
	}
	return isValidChange;
}