#include "RJ_Vertex.h"
#include "RJ_Renderer.h"

IDirect3DVertexDeclaration9* _BaseVertex::VertexDecl = 0;
IDirect3DVertexDeclaration9* _ColorVertex::VertexDecl = 0;
IDirect3DVertexDeclaration9* _EffectVertex::VertexDecl = 0;
IDirect3DVertexDeclaration9* _AnimatedVertex::VertexDecl = 0;
IDirect3DVertexDeclaration9* _FontVertex::VertexDecl = 0;

void InitializeAllVertexDeclarations()
{
	D3DVERTEXELEMENT9 VertexElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		{0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};	
	RJ_RENDERER->getDirect3DDevice()->CreateVertexDeclaration(VertexElements, &_BaseVertex::VertexDecl);

	D3DVERTEXELEMENT9 VertexColorElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};	
	RJ_RENDERER->getDirect3DDevice()->CreateVertexDeclaration(VertexColorElements, &_ColorVertex::VertexDecl);

	D3DVERTEXELEMENT9 VertexEffectElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		{0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};	
	RJ_RENDERER->getDirect3DDevice()->CreateVertexDeclaration(VertexEffectElements, &_FontVertex::VertexDecl);

	D3DVERTEXELEMENT9 VertexFontElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};	
	RJ_RENDERER->getDirect3DDevice()->CreateVertexDeclaration(VertexFontElements, &_EffectVertex::VertexDecl);


	D3DVERTEXELEMENT9 VertexAnimatedElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
		{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
		{0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1},
		{0, 56, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 2},
		D3DDECL_END()
	};	
	RJ_RENDERER->getDirect3DDevice()->CreateVertexDeclaration(VertexAnimatedElements, &_AnimatedVertex::VertexDecl);
}

void ShutdownAllVertexDeclarations()
{
	if(_BaseVertex::VertexDecl)
	{
		_BaseVertex::VertexDecl->Release();
		_BaseVertex::VertexDecl = 0;
	}

	if(_FontVertex::VertexDecl)
	{
		_FontVertex::VertexDecl->Release();
		_FontVertex::VertexDecl = 0;
	}

	if(_ColorVertex::VertexDecl)
	{
		_ColorVertex::VertexDecl->Release();
		_ColorVertex::VertexDecl = 0;
	}

	if(_EffectVertex::VertexDecl)
	{
		_EffectVertex::VertexDecl->Release();
		_EffectVertex::VertexDecl = 0;
	}

	if(_AnimatedVertex::VertexDecl)
	{
		_AnimatedVertex::VertexDecl->Release();
		_AnimatedVertex::VertexDecl = 0;
	}
}