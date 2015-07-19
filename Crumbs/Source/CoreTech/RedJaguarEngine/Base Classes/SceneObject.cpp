#include "ISceneObject.h"
#include "../Asset/AssetManager.h"
#include "../Renderer/RJ_Renderer.h"
#include "../../../Gameplay/Objects/Mesh.h"

void ISceneObject::Render(double fDelta)
{
	RJ_RENDERER->getDirect3DDevice()->SetTransform(D3DTS_WORLD, &GetWorldMat());

	Mesh* pMesh = CAssetManager::getInstanceRender()->GetMesh( getName() );
	pMesh->setAdditive(m_fRandomAdditive);
	pMesh->Render(fDelta);
}