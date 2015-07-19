#include "PowerUp.h"
#include "CPlayer.h"
#include "CStone.h"
#include "../../CoreTech/RedJaguarEngine/Event/EventDefinitions.h"
#include "../../CoreTech/RedJaguarEngine/Event/EventManager.h"
#include <time.h>
#include "../Gameplay.h"

CPowerUp::CPowerUp(void) : CSolidSphere()
{
	m_ePOWType = WHIP_CREAM;
	m_bFC = false;
	m_dTime = 0;
}
CPowerUp::CPowerUp(char* szName, D3DXVECTOR3 vPosition, POWERUP_TYPE ePOWType, float fRadius, float fSpringValue,
				   MATERIAL_TYPE eMatType, unsigned int uiID, bool bActive) : CSolidSphere(szName, 
				   vPosition, fRadius, fSpringValue, eMatType, uiID, bActive)
{
	m_ePOWType = ePOWType;
	m_bFC = false;
	m_dTime = 0;
}


CPowerUp::~CPowerUp(void)
{
}

bool CPowerUp::HandlePowerUp(double fDelta, bool currStone)
{
	// send SETPOW event
	if(currStone && this->GetStone()->GetGameplay()->GetCurrPlayer() == Player)
	{
		volatile tPowerUpData* tPD = new tPowerUpData(EM_SETPOW, (int)m_ePOWType);
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_SETPOW,(tBaseEventData*)tPD);
		tPD = NULL;
	}

	Player->SetFC(false);
	m_dTime += fDelta;
	//if the Power Up is the Fortune Cookie
	if (m_ePOWType == FORTUNE_COOKIE)
	{
		m_dTime = 0.0;
		m_bFC = true;
		Player->SetFC(true);

		if(Player->GetCherry() == false)
		{
			//Random if player does not already have the Cherry Power Up
			m_ePOWType = (POWERUP_TYPE)(rand() % FORTUNE_COOKIE);
			if(m_ePOWType == CHERRIES)
			{
				Player->SetCherry(true);
				Player->SetCherryPOW(this);
			}
		}
		else
		{
			//Random if player already has the Cherry Power Up
			m_ePOWType = (POWERUP_TYPE)((rand() % WHIP_CREAM) + 1);
		}

	}
	else if(m_bFC == false || m_dTime >= 2.0)
	{
		m_bFC = false;
		m_dTime = 0.0;
		switch (m_ePOWType)
		{
		case TURKEY:
			{
				//if its not the current stone then they no longer have the cold turkey
				if(!currStone)
				{
					Stone->SetTurkey(false);
					return true;
				}
				else
				{
					Stone->SetTurkey(true);

					// tell HUD that we got the cold turkey
					volatile tSetTurkey* tST = new tSetTurkey(EM_SETTURKEY, true);
					CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_SETTURKEY, (tBaseEventData*)tST);
					tST = NULL;
				}
				break;
			}
		case WHIP_CREAM:
			{
				//increase the speed of the stone
				Stone->setSpeed(Stone->getSpeed() + 2);

				//play the motion line effect
				volatile tMotionLines* tPD = new tMotionLines(EM_MOTION, 2.0f);
				CEventManager::GetInstanceThreaded()->SendEventUpdateLocal(ET_MOTION,(tBaseEventData*)tPD);
				tPD = NULL;

				// tell HUD we got Whipped Cream
				volatile tPlayerGotWC* tWC = new tPlayerGotWC(EM_PLAYERGOTCREAM);
				CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_PLAYERGOTCREAM, (tBaseEventData*)tWC);
				tWC = NULL;

				//return true so the power up is not handled again
				return true;
				break;
			}

		case CHERRIES:
			{
				//ignore the cherry until the last shot and then remove it from the list to be handled else where
				if(Stone->GetOnCherryShot())
					return true;
				break;
			}

		case GUM:
			{
				Stone->SetGum(true);
				Stone->setTransparent(true);

				if (currStone)
				{
					// tell HUD we have gum
					volatile tSetGum* tSG = new tSetGum(EM_SETGUM);
					CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_SETGUM, (tBaseEventData*)tSG);
					tSG = NULL;
				}

				if (Stone->getSpeed() > 0.0f)
				{
					//if the stone is still moving then the counter for it breaking should not be increasing
					Stone->SetGumCollision(0);
				}
				else
				{
					if(Stone->GetGumCollision() >= 2)
					{
						//once it has been hit twice remove the gum
						Stone->SetGum(false);
						return true;
					}
				}
				break;
			}
		}
	}

	return false;
}

bool CPowerUp::HandleCherryShot(double fDelta, CPlayer* _player)
{
	//make sure the player has used all of their turns
	if(_player->GetTurns() > 4)
	{
		CStone* _stonesArr[5];
		_stonesArr[0] = _player->GetStone(0);
		_stonesArr[1] = _player->GetStone(1);
		_stonesArr[2] = _player->GetStone(2);
		_stonesArr[3] = _player->GetStone(3);
		_stonesArr[4] = _player->GetStone(4);

		//find the stone with the lowest score
		if(_stonesArr[0]->getTotalScore() <= _stonesArr[1]->getTotalScore() && _stonesArr[0]->getTotalScore() <= _stonesArr[2]->getTotalScore() &&
			_stonesArr[0]->getTotalScore() <= _stonesArr[3]->getTotalScore() && _stonesArr[0]->getTotalScore() <= _stonesArr[4]->getTotalScore())
			_player->SetCurrStone(0);
		else if(_stonesArr[1]->getTotalScore() <= _stonesArr[0]->getTotalScore() && _stonesArr[1]->getTotalScore() <= _stonesArr[2]->getTotalScore() &&
			_stonesArr[1]->getTotalScore() <= _stonesArr[3]->getTotalScore() && _stonesArr[1]->getTotalScore() <= _stonesArr[4]->getTotalScore())
			_player->SetCurrStone(1);
		else if(_stonesArr[2]->getTotalScore() <= _stonesArr[0]->getTotalScore() && _stonesArr[2]->getTotalScore() <= _stonesArr[1]->getTotalScore() &&
			_stonesArr[2]->getTotalScore() <= _stonesArr[3]->getTotalScore() && _stonesArr[2]->getTotalScore() <= _stonesArr[4]->getTotalScore())
			_player->SetCurrStone(2);
		else if(_stonesArr[3]->getTotalScore() <= _stonesArr[0]->getTotalScore() && _stonesArr[3]->getTotalScore() <= _stonesArr[1]->getTotalScore() &&
			_stonesArr[3]->getTotalScore() <= _stonesArr[2]->getTotalScore() && _stonesArr[3]->getTotalScore() <= _stonesArr[4]->getTotalScore())
			_player->SetCurrStone(3);
		else if(_stonesArr[4]->getTotalScore() <= _stonesArr[0]->getTotalScore() && _stonesArr[4]->getTotalScore() <= _stonesArr[1]->getTotalScore() &&
			_stonesArr[4]->getTotalScore() <= _stonesArr[3]->getTotalScore() && _stonesArr[4]->getTotalScore() <= _stonesArr[2]->getTotalScore())
			_player->SetCurrStone(4);

		//reset the stone with the lowest score and give the player another turn
		_player->GetCurrStone()->Reset();
		_player->SetNumTurns(4);

		
		_player->SetUsedCherry(true);
		_player->GetCurrStone()->SetOnCherryShot(true);
		// send event telling the HUD we are on our cherry shot
		volatile tCherryShot* tCS = new tCherryShot(EM_CHERRYSHOT);
		CEventManager::GetInstanceThreaded()->SendEventUpdateCrossToRender(ET_CHERRYSHOT,(tBaseEventData*)tCS);
		tCS = NULL;

		//return true to remove the cherry so it is not handled again
		return true;
	}
	return false;
}