#include "State_Loading.h"
#include "State_PlayerMenu.h"
#include "../Asset/AssetManager.h"
#include "..\\Input\\CInputManager.h"
#include "..\\..\\..\\RJ_Config.h"
#include "..\\..\\RJ_GameMain.h"
#include "../Text/font_defines.h"

#include "State_Config.h"

State_Loading::State_Loading(void) : Font("t_Crumbs_font.dds","Resources/Fonts/Crumbs_font.dat")
{
	tSC = NULL; 
	m_vBanana = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vPotato = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vPopsicle = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vApple = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vFish = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vAnthony = D3DXVECTOR2(512.0f, 384.0f);
	uberClocked = 0.5;
}

void State_Loading::OnPush(void)
{
	if(tSC == NULL)
		tSC = tStateConfig::GetInstanceRender();
}

void State_Loading::OnEnter(void)
{
	m_vBanana = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vPotato = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vPopsicle = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vApple = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vFish = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * 0.5f) * 300.0f), 384.0f - (sinf(D3DX_PI * 0.5f) * 250.0f));
	m_vAnthony = D3DXVECTOR2(512.0f, 384.0f);
	uberClocked = 0.5;
}

void State_Loading::OnExit(void)
{

}
bool State_Loading::Input(double _fDelta)
{
	uberClocked += _fDelta;
	if(!tSC->m_bIsLoading && uberClocked >= 3.5f)
		RJ_MAIN_GAME->PopState();
	if(uberClocked >= 2.1f)
		m_vBanana = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * ((float)uberClocked-1.6f))*300.0f), 384.0f - (sinf(D3DX_PI * ((float)uberClocked-1.6f)) * 250.0f));
	if(uberClocked >= 1.7f)
		m_vPotato = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * ((float)uberClocked-1.2f))*300.0f), 384.0f - (sinf(D3DX_PI * ((float)uberClocked-1.2f)) * 250.0f));
	if(uberClocked >= 1.3f)
		m_vApple = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * ((float)uberClocked-0.8f))*300.0f), 384.0f - (sinf(D3DX_PI * ((float)uberClocked-0.8f)) * 250.0f));
	if(uberClocked >= 0.9f)
		m_vPopsicle = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * ((float)uberClocked-0.4f))*300.0f), 384.0f - (sinf(D3DX_PI * ((float)uberClocked-0.4f)) * 250.0f));
	m_vFish = D3DXVECTOR2(512.0f + (cosf(D3DX_PI * ((float)uberClocked))*300.0f), 384.0f - (sinf(D3DX_PI * ((float)uberClocked)) * 250.0f));
	return true;
}

bool State_Loading::InputUnder(double _fDelta)
{
	return true;
}

bool State_Loading::Update(double _fDelta)
{
	return true;
}

bool State_Loading::UpdateUnder(double _fDelta)
{
	return true;
}

bool State_Loading::Render(double _fDelta)
{
	CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nLoading,0.0f,0.0f,2.0f,1.5f);

	//Draw Anthony
	RECT anthony;
	anthony.bottom = 330;
	anthony.top = 160;
	anthony.left = 850;
	anthony.right = 1020;
	CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nLoading,m_vAnthony.x - (anthony.right - anthony.left), 
		m_vAnthony.y - (anthony.bottom - anthony.top),1.5f,1.5f, &anthony);

	//Draw Banana
	RECT banana;
	banana.top = 20;
	banana.left = 540;
	banana.bottom = 120;
	banana.right = 670;
	CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nLoading,m_vBanana.x - (banana.right - banana.left), 
		m_vBanana.y - (banana.bottom - banana.top),1.5f,1.5f, &banana);

	//Draw Potato
	RECT potato;
	potato.top = 35;
	potato.left = 735;
	potato.bottom = 120;
	potato.right = 860;
	CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nLoading,m_vPotato.x - (potato.right - potato.left), 
		m_vPotato.y - (potato.bottom - potato.top),1.5f,1.5f, &potato);

	//Draw Apple
	RECT apple;
	apple.top = 200;
	apple.left = 530;
	apple.bottom = 315;
	apple.right = 630;
	CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nLoading,m_vApple.x - (apple.right - apple.left), 
		m_vApple.y - (apple.bottom - apple.top),1.5f,1.5f, &apple);

	//Draw Popsicle
	RECT popsicle;
	popsicle.top = 5;
	popsicle.left = 935;
	popsicle.bottom = 155;
	popsicle.right = 1010;
	CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nLoading,m_vPopsicle.x - (popsicle.right - popsicle.left), 
		m_vPopsicle.y - (popsicle.bottom - popsicle.top),1.5f,1.5f, &popsicle);

	//Draw Fish
	RECT fish;
	fish.top = 210;
	fish.left = 675;
	fish.bottom = 315;
	fish.right = 835;
	CAssetManager::getInstanceRender()->DrawFlatSprite(tSC->m_nLoading,m_vFish.x - (fish.right - fish.left), 
		m_vFish.y - (fish.bottom - fish.top),1.5f,1.5f, &fish);

	//static int counter = 0;
	if((int)uberClocked%3 < 1)
		Font.DrawString("LOADING !",20,700);
	else if(((int)uberClocked%3) - 1 < 1)
		Font.DrawString("LOADING !!",20,700);
	else
		Font.DrawString("LOADING !!!",20,700);
	return true;
}

bool State_Loading::RenderUnder(double _fDelta)
{
	return true;
}