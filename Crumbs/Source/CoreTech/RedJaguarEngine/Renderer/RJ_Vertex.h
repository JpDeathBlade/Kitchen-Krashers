#ifndef  REDJAGUAR_VERTEX_H_
#define  REDJAGUAR_VERTEX_H_

#include <d3dx9.h>

void InitializeAllVertexDeclarations();
void ShutdownAllVertexDeclarations();

struct _BaseVertex
{
	D3DXVECTOR4		position;
	D3DXVECTOR4		normal;
	D3DXVECTOR2		textureUV;
	D3DXVECTOR2		lightmapUV;
	D3DXVECTOR4		color;
	static IDirect3DVertexDeclaration9* VertexDecl;
};

struct _FontVertex
{
	D3DXVECTOR4		position;
	D3DXVECTOR4		color;
	D3DXVECTOR2		textureUV;
	static IDirect3DVertexDeclaration9* VertexDecl;
};

struct _ColorVertex
{
	D3DXVECTOR4 position;
	D3DCOLOR    color;
	static IDirect3DVertexDeclaration9* VertexDecl;
};

struct _EffectVertex
{
	D3DXVECTOR4		position;
	D3DXVECTOR2		textureUV;
	static IDirect3DVertexDeclaration9* VertexDecl;
};

struct _AnimatedVertex
{
	D3DXVECTOR3		position;
	D3DXVECTOR2		textureUV;
	D3DXVECTOR3		BlendWgt;
	D3DXVECTOR3		BlendInc;
	D3DXVECTOR3		positionone;
	D3DXVECTOR3		positiontwo;
	static IDirect3DVertexDeclaration9* VertexDecl;
};

#endif // end of REDJAGUAR_VERTEX_H_