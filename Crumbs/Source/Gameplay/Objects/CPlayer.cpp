#include "CPlayer.h"
#include "CStone.h"
#include "PowerUp.h"
#include "../../CoreTech/Utilities/RJ_MathLibrary.hpp"
#include "../../CoreTech/RedJaguarEngine/Event/EventDefinitions.h"
#include "../../CoreTech/RedJaguarEngine/Event/EventManager.h"
#include "../../CoreTech/RedJaguarEngine/Animations/AnimManager.h"
#include "Mesh.h"
#include "AnimMesh.h"
#include "../Gameplay.h"
#include "../../CoreTech/RedJaguarEngine/Scene/RJ_SceneManager.h"

CPlayer::CPlayer(unsigned int PlayerNum, D3DXVECTOR4 _color)
{
	m_uiPlayerNumber = PlayerNum;
	m_uiNumTurnsUsed = 0;
	m_vColor = _color;
	rotation = 1.0f;
	timer = 1.0f;
	for (unsigned int _stone = 0; _stone < 5; ++_stone)
	{
		m_stones[_stone] = new CStone(m_vColor,this);
	}
	CherryPOW = NULL;
	CurrStone = 0;
	m_uiTotalScore = 0;
	m_bEventFC = false;
	m_bCherry = false;
	m_bUsedCherry = false;
	volatile tCreateAnimLink* tLink = new tCreateAnimLink(EM_CREATELINK, this->GetGlobalID(), RJ_Config::RJ_ANIM_ANTHONY); 
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CREATELINK,(tBaseEventData*)tLink);
	tLink = NULL;
	volatile tCreateAnimLink* tBLink = new tCreateAnimLink(EM_CREATELINK, this->GetGlobalID(), RJ_Config::RJ_ANIM_BROOM); 
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CREATELINK,(tBaseEventData*)tBLink);
	tBLink = NULL;
	volatile tCreateAnimLink* tGLink = new tCreateAnimLink(EM_CREATELINK, this->GetGlobalID(), RJ_Config::RJ_ANIM_GUMMY); 
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CREATELINK,(tBaseEventData*)tGLink);
	tGLink = NULL;
	volatile tCreateAnimLink* tTLink = new tCreateAnimLink(EM_CREATELINK, this->GetGlobalID(), RJ_Config::RJ_ANIM_TOOTHPICK); 
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CREATELINK,(tBaseEventData*)tTLink);
	tTLink = NULL;
}
CPlayer::~CPlayer()
{
	/*for (unsigned int _stone = 0; _stone < 5; ++_stone)
	{
		delete m_stones[_stone];
		m_stones[_stone] = NULL;
	}*/
	volatile tBreakAnimLink* tLink = new tBreakAnimLink(EM_BREAKLINK, this->GetGlobalID(), RJ_Config::RJ_ANIM_ANTHONY); 
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_BREAKLINK,(tBaseEventData*)tLink);
	tLink = NULL;
	volatile tBreakAnimLink* tBLink = new tBreakAnimLink(EM_BREAKLINK, this->GetGlobalID(), RJ_Config::RJ_ANIM_BROOM); 
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_BREAKLINK,(tBaseEventData*)tBLink);
	tBLink = NULL;
	volatile tBreakAnimLink* tGLink = new tBreakAnimLink(EM_BREAKLINK, this->GetGlobalID(), RJ_Config::RJ_ANIM_GUMMY); 
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_BREAKLINK,(tBaseEventData*)tGLink);
	tGLink = NULL;
	volatile tBreakAnimLink* tTLink = new tBreakAnimLink(EM_BREAKLINK, this->GetGlobalID(), RJ_Config::RJ_ANIM_TOOTHPICK); 
	CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_BREAKLINK,(tBaseEventData*)tTLink);
	tTLink = NULL;
}
void CPlayer::Reset(void)
{
	m_uiNumTurnsUsed = 0;
	m_bCherry = false;
	CurrStone = 0;
	m_uiTotalScore = 0;
	rotation = 1.0f;
	timer = 1.0f;
	m_vPowerUps.clear();
	CherryPOW = NULL;
	for(unsigned int _stone = 0; _stone < 5; ++_stone)
	{
		m_stones[_stone]->Reset();
	}
}
void CPlayer::updateScore()
{
	m_uiTotalScore = 0;
	for(unsigned int stone = 0; stone < 5; stone++)
	{
		m_uiTotalScore += m_stones[stone]->getTotalScore();
	}
}
void CPlayer::UsedTurn(void)
{
	if (m_uiNumTurnsUsed < 5)
	{
		++m_uiNumTurnsUsed;
		CurrStone = m_uiNumTurnsUsed;
		if (CurrStone == 5)
		{
			CurrStone = 4;
		}
	}
}
bool CPlayer::TurnOver(double fDelta)
{
	m_stones[CurrStone]->GetVolume().SetIsActive(true);
	D3DXVECTOR3 _vel = m_stones[CurrStone]->getVelocity();
	if ((_vel.x == 0 && _vel.z == 0) && (m_stones[CurrStone]->GetCanStop() == true) )
	{
		static float m_fTimeElapsed = 0;
		m_fTimeElapsed += (float)fDelta;
		if(m_fTimeElapsed >= 2.0f)
		{
			m_fTimeElapsed = 0;
			return true;
		}
	}
	return false;
}
void CPlayer::Render(double fDelta)
{
	//if ( CGameplay::GetInstanceUpdate()->GetLaunched() )
	//{
		if(CGameplay::GetInstanceUpdate()->GetCurrPlayer()->getPlayerNumber() == this->m_uiPlayerNumber)
		{
			if(!m_stones[CurrStone]->GetIsLaunched())
			{
				rotation = m_stones[CurrStone]->getLaunchAngle() * D3DX_PI;
				timer = 1.0f;
			}
			else if(timer > 0.0f)
				timer -= (float)fDelta;
			else if(rotation > D3DX_PI + 0.01f)
				rotation -= 1.0f * (float)fDelta;
			else if(rotation < D3DX_PI - 0.01f)
				rotation += 1.0f * (float)fDelta;
			else
				rotation = D3DX_PI;
			D3DXMATRIX _startmat = SceneManager::getInstanceUpdate()->GetGummyStart();

			if(SceneManager::getInstanceUpdate()->getCurrLevel() != 1)
			{
				D3DXMATRIX _pizza;

				D3DXMatrixRotationY( &_pizza, (-rotation) + D3DX_PI);

				if(!m_stones[CurrStone]->GetIsLaunched())
					m_mStonePos = m_stones[CurrStone]->GetWorldMat();

				_pizza = (m_mStonePos * _pizza);

				_pizza._41 = m_mStonePos._41;
				_pizza._42 = m_mStonePos._42;
				_pizza._43 = m_mStonePos._43;

				RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_pizza);
				CAssetManager::getInstanceRender()->GetMesh( "SM_PizzaCutter" )->Render(fDelta);


				D3DXMATRIX _rot1, _trans2, _rot2, _final;
				D3DXMatrixIdentity(&_rot1);
				D3DXMatrixIdentity(&_trans2);
				D3DXMatrixIdentity(&_rot2);
				D3DXMatrixIdentity(&_final);

				D3DXMatrixRotationY(&_rot1, (-rotation) + (D3DX_PI * 0.5f));

				_trans2._41 = m_mStonePos._41 - _startmat._41;
				_trans2._42 = m_mStonePos._42 - _startmat._42;
				_trans2._43 = m_mStonePos._43 - _startmat._43;

				D3DXMatrixRotationY(&_rot2, D3DX_PI * 0.5f);

				_final = (m_mStonePos * _final);
				_final = (_rot1 * _final);
				_final = (_trans2 * _final);
				_final = (_rot2 * _final);

				RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_final);
			}
			else
				RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &_startmat);
			CAssetManager::getInstanceRender()->GetMesh( "SM_WormGum" )->Render(fDelta);
			AnimLink* AniLink = CAssetManager::getInstanceRender()->GetAnimLink(GetGlobalID(), RJ_Config::RJ_ANIM_GUMMY);
			if(AniLink)
			{
				AnimStateMachine* GummyStateMachine = AniLink->m_AStateMachine;
				CAssetManager::getInstanceRender()->GetAnimMesh( "GummyWorm" )->Render(fDelta,GummyStateMachine);
				GummyStateMachine = NULL;
			}
			AniLink = NULL;

			AnimLink* ATniLink = CAssetManager::getInstanceRender()->GetAnimLink(GetGlobalID(), RJ_Config::RJ_ANIM_TOOTHPICK);
			if(ATniLink)
			{
				AnimStateMachine* GummyStateMachine = ATniLink->m_AStateMachine;
				CAssetManager::getInstanceRender()->GetAnimMesh( "toothPicks" )->Render(fDelta,GummyStateMachine);
				GummyStateMachine = NULL;
			}
			ATniLink = NULL;
		}
	//}
}
void CPlayer::addPowerup(CPowerUp* _powerup, CStone* _stone)
{
	_powerup->SetStone(_stone);
	_powerup->SetPlayer(this);
	m_vPowerUps.push_back(_powerup);
}
void CPlayer::GetPowerUps()
{
	for (unsigned int _stone = 0; _stone < 5; ++_stone)
	{
		if(m_stones[_stone]->GetPOWDone())
		{
			std::vector<CPowerUp*> temp;
			temp = m_stones[_stone]->GetPowerUps();
			unsigned int _size = temp.size();
			for (unsigned int i = 0; i < _size; ++i)
			{
				temp[i]->SetStone(m_stones[_stone]);
				if(((CPowerUp*)temp[i])->GetPOWType() == CHERRIES)
					m_bCherry = true;
				temp[i]->SetPlayer(this);
				m_vPowerUps.push_back(temp[i]); 
			}
			m_stones[_stone]->EmptyPowerUps();
		}
	}
}
void CPlayer::RemovePOWfromCherryShot()
{
	std::vector<CPowerUp*>::iterator iter;
	for(iter = m_vPowerUps.begin(); iter != m_vPowerUps.end(); ++iter)
	{
		if((*iter)->GetStone() == m_stones[CurrStone] && (*iter)->GetPOWType() != CHERRIES)
		{
			CPowerUp* temp;
			temp = (*iter);
			m_vPowerUps.erase(iter);
			if(m_vPowerUps.empty())
				break;
			else
			{
				iter = m_vPowerUps.begin();
			}
		}
	}
}
void CPlayer::handlePowerups(double fDelta)
{
	bool bcurrStone;
	if (!m_vPowerUps.empty())
	{
		std::vector<CPowerUp*>::iterator iter;
		int i = -1;
		for(iter = m_vPowerUps.begin(); iter != m_vPowerUps.end(); ++iter)
		{
			++i;
			//if((*iter)->GetPOWType() == CHERRIES)
			//	continue;
			if((*iter)->GetStone() == m_stones[CurrStone])
				bcurrStone = true;
			else
				bcurrStone = false;
			if((*iter)->HandlePowerUp(fDelta, bcurrStone))
			{
				if(m_vPowerUps[i]->GetPOWType() == CHERRIES)
				{
					delete m_vPowerUps[i];
					CherryPOW = NULL;
				}
				else
					delete m_vPowerUps[i];
				m_vPowerUps.erase(iter);
				if(m_vPowerUps.empty())
					break;
				else
				{
					iter = m_vPowerUps.begin();
					i = 0;
				}
			}
		}	
	}
}
bool CPlayer::handleCherry(double fDelta)
{
	if(CherryPOW != NULL)
	{
		if(CherryPOW->HandleCherryShot(fDelta, this))
		{
// 			delete CherryPOW;
// 			CherryPOW = NULL;
			/*for (unsigned int POW = 0; POW < m_vPowerUps.size(); ++POW)
			{
				m_vPowerUps.pop_back();
			}*/
			return true;
		}
	}


	return false;
}
