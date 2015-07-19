#include "CSpectator.h"
#include "../../CoreTech/RedJaguarEngine/Event/EventDefinitions.h"
#include "../../CoreTech/RedJaguarEngine/Event/EventManager.h"
#include "../../Coretech/RedjaguarEngine/Asset/AssetManager.h"
#include "AnimMesh.h"
#include "../GameAnim/SpectatorAnimStates.h"
#include <Windows.h>

CSpectator::CSpectator(int nGlobalID)
{
	GlobalID = nGlobalID;
	volatile tCreateAnimLink* tLink = new tCreateAnimLink(EM_CREATELINK, GlobalID, RJ_Config::RJ_SPECTATOR); 
	CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_CREATELINK,(tBaseEventData*)tLink);
	tLink = NULL;
	m_bSitting = false;
}
CSpectator::~CSpectator()
{
	volatile tBreakAnimLink* tLink = new tBreakAnimLink(EM_BREAKLINK, GlobalID, RJ_Config::RJ_SPECTATOR); 
	CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_BREAKLINK,(tBaseEventData*)tLink);
	tLink = NULL;
}
void CSpectator::Initialize(int nCheerAnimID)
{
	m_nCheerAnimID = nCheerAnimID;
	if(!m_bSitting)
	{
		if(m_nCheerAnimID == 0)
		{
			volatile tAnimStateChange* tAChange = new tAnimStateChange(EM_ANIMSTATECHANGE, GlobalID,
				RJ_Config::RJ_SPECTATOR,RJ_Config::SPEC_ANIM_CHEER1,false); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ANIMSTATECHANGE,(tBaseEventData*)tAChange);
			tAChange = NULL;
		}
		else if(m_nCheerAnimID == 1)
		{
			volatile tAnimStateChange* tAChange = new tAnimStateChange(EM_ANIMSTATECHANGE, GlobalID,
				RJ_Config::RJ_SPECTATOR,RJ_Config::SPEC_ANIM_CHEER2,false); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ANIMSTATECHANGE,(tBaseEventData*)tAChange);
			tAChange = NULL;
		}
		else if(m_nCheerAnimID == 2)
		{
			volatile tAnimStateChange* tAChange = new tAnimStateChange(EM_ANIMSTATECHANGE, GlobalID,
				RJ_Config::RJ_SPECTATOR,RJ_Config::SPEC_ANIM_CHEER3,false); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ANIMSTATECHANGE,(tBaseEventData*)tAChange);
			tAChange = NULL;
		}
		else if(m_nCheerAnimID == 3)
		{
			volatile tAnimStateChange* tAChange = new tAnimStateChange(EM_ANIMSTATECHANGE, GlobalID,
				RJ_Config::RJ_SPECTATOR,RJ_Config::SPEC_ANIM_CHEER4,false); 
			CEventManager::GetInstanceThreaded()->SendEventRenderLocal(ET_ANIMSTATECHANGE,(tBaseEventData*)tAChange);
			tAChange = NULL;
		}
	}
}
void CSpectator::Shutdown()
{

}
void CSpectator::Reset(void)
{

}
void CSpectator::Render(double fDelta)
{
	//D3DXMATRIX _world = GetWorldMat();
	//RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_world);

	AnimSpectator* AniLink = (AnimSpectator*)CAssetManager::getInstanceRender()->GetAnimLink(GlobalID, RJ_Config::RJ_SPECTATOR);
	if(AniLink)
	{
		AnimStateMachine* AnthonyStateMachine = AniLink->m_AStateMachine;
		CAssetManager::getInstanceRender()->GetAnimMesh( "AnthonyBody" )->Render(fDelta,AnthonyStateMachine, this);
		AnthonyStateMachine = NULL;
	}
	AniLink = NULL;
}
void CSpectator::Update(double fDelta)
{

}
void CSpectator::AddAntCheer(D3DXMATRIX mLoc)
{
	CheerAnt AddAnt;
	AddAnt.mLoc = mLoc;
	AddAnt.nColor = rand()%4;
	m_dCheerDeck.push_back(AddAnt);
}
void CSpectator::ClearAntCheers()
{
	while(!m_dCheerDeck.empty())
		m_dCheerDeck.pop_front();
}