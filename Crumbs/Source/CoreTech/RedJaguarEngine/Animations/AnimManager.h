#ifndef ANIMMANAGER_H
#define ANIMMANAGER_H

#include "D3DX9.h"
#include "../Asset/AssetManager.h"
#include "../../../Gameplay/GameAnim/AState.h"
#include "../../../Gameplay/Objects/Mesh.h"
#include "../../../Gameplay/Objects/AnimMesh.h"
#include <deque>
#include <String>
using std::deque;
using std::string;

class AnimStateMachine;

struct KeyFrame
{
	float m_fKeyTime;
	Bone m_TransformFrame;
	decompsedMat m_fQuatTransform;
};

struct AnimatedBone
{
	int BoneIndex;
	std::deque<KeyFrame> vKeyFrames;
	std::string m_szABoneName;
	~AnimatedBone() { vKeyFrames.clear(); }
};

class CAnimation
{	
	friend class CInterpolator;
	friend class AnimationManager;
private:
	/***********************************************
	* Data Members
	************************************************/
	float m_fDuration;
	std::string m_szName;
	std::deque< AnimatedBone > m_vAnimatedBones;
	float m_fPreHitTime;
	float m_fHitTime;
	float m_fPostHitTime;
public:
	CAnimation(void);
	~CAnimation(void){ m_vAnimatedBones.clear(); }
	void LoadFromXML(std::string fileName);
	void LoadFromBinary(std::string fileName);
	std::deque< AnimatedBone > &GetAnimatedBones(void){ return m_vAnimatedBones; }
	float GetDuration(void){ return m_fDuration; }
	void SetPreHitTime(float time){m_fPreHitTime = time;}
	void SetHitTime(float time) {m_fHitTime = time;}
	void SetPostHitTime(float time) {m_fPostHitTime = time;}
	float GetPreHitTime() const {return m_fPreHitTime;}
	float GetHitTime() const {return m_fHitTime;}
	float GetPostHitTime() const {return m_fPostHitTime;}
};

class CInterpolator
{
private:
	const CAnimation *m_pAnimation;
	std::deque<Bone> m_vBoneFrames;
	float m_fCurrentTime;
	float m_fAnimSpeed;
	bool m_bIsLooping;
	float m_fTimeRemainingInAnim;
	bool m_bIsDone;
public:
	CInterpolator(void);
	~CInterpolator(void)
	{
		unsigned int nPrevBoneSize = m_vBoneFrames.size();
		for(unsigned int i = 0; i < nPrevBoneSize; ++i)
			m_vBoneFrames.pop_front();
		m_vBoneFrames.clear();
	}
	void SetAnimation(const CAnimation *inAnimation);
	void AddTime(float t) { m_fCurrentTime += (t*m_fAnimSpeed); }
	void SetTime(float t) { m_fCurrentTime = t; }
	void Process(void);
	float GetTime() {return m_fCurrentTime;}
	std::deque<Bone> &GetCurrentBoneFrames(void){ return m_vBoneFrames; }
	void SetAnimSpeed(float fSpeed){m_fAnimSpeed=fSpeed;}
	void ResetAnimSpeed(){m_fAnimSpeed=1.0f;}
	void SetIsLooping(bool bLooping){m_bIsLooping=bLooping;}
	bool GetIsLooping(){return m_bIsLooping;}
	bool GetIsDone(){return m_bIsDone;}
	float GetTimeRemaining(){return (m_pAnimation->m_fDuration-m_fCurrentTime);}
	const CAnimation* GetCurrentAnimation(){return m_pAnimation;}
	float GetAnimSpeed() {return m_fAnimSpeed;}
};

struct AnimObject
{
	CInterpolator m_Interpolator;
	CInterpolator m_BlendToInterp;
	std::deque<D3DXMATRIX> m_vBlendedFrames;
	~AnimObject(void)
	{ 
		unsigned int nPrevBlendSize = m_vBlendedFrames.size();
		for(unsigned int i = 0; i < nPrevBlendSize; ++i)
			m_vBlendedFrames.pop_front();
		m_vBlendedFrames.clear(); 
	}
	//AnimationBlender m_Blender;
};

class AnimStateMachine
{
public:
	AnimStateMachine(void);
	//AnimStateMachine(Entity * pEntity);
	std::deque<float> m_vAnimSpeeds;
public:
	~AnimStateMachine(void);
private:
	//Entity * m_pEntity;
	AState * m_pCurrentState;
	AState * m_pPreviousState;
	float fTimeInCurrentState;
	AnimObject m_tAnimObject;
public:	
	void SetCurrentState(AState * pState)
	{
		m_pCurrentState = pState;
	}
	void SetPreviousState(AState * pState)
	{
		m_pPreviousState = pState;
	}
	void Update(float fTime);
	void ChangeState(AState * pNewState,bool canRepeat = false);
	void RevertToPreviousState();
	AState * GetCurrentState()
	{
		return m_pCurrentState;
	}
	AState* GetPreviousState()
	{
		return m_pPreviousState;
	}
	AnimObject& GetAnimObject(){return m_tAnimObject;}
	//AnimationBlender& GetBlender() {return m_tAnimObject.m_Blender;}
	void Init();
	float GetAnimSpeed(int id);
	static AState* GetState(const char* name);
	bool CheckStates(const AState* pAnim1,const AState* pAnim2 = NULL);
};

class AnimationManager
{
private:
	AnimationManager(void);
	AnimationManager(const AnimationManager &) {}
	const AnimationManager & operator=(const AnimationManager &) {}
	~AnimationManager(void);
	std::deque<CAnimation*> m_vAnimations;
	unsigned int m_nAnimationSize;
public:
	static AnimationManager * GetInstanceRender();
	bool AddAnimation(std::string szName);
	bool AddAnimation(CAnimation *pAnimation);
	CAnimation * RemoveAnimation(std::string szName);
	CAnimation * GetAnimation(std::string szName)
	{
		for(unsigned int iAnim = 0; iAnim < m_vAnimations.size(); iAnim++)
		{
			if(szName == m_vAnimations[iAnim]->m_szName)
				return m_vAnimations[iAnim];
		}

		return NULL;
	}
	int GetAnimationIndex(std::string szName)
	{
		for(unsigned int iAnim = 0; iAnim < m_vAnimations.size(); iAnim++)
		{
			if(szName == m_vAnimations[iAnim]->m_szName)
				return iAnim;
		}

		return -1;
	}
	CAnimation *GetAnimation(unsigned int uiIndex)
	{
		if( uiIndex >= 0 || uiIndex < m_vAnimations.size() )
			return m_vAnimations[uiIndex];
		return NULL;
	}
	void Shutdown();
	void Init();
};
#endif