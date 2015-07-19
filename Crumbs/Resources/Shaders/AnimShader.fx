//**************************************************************//
//  Effect File exported by RenderMonkey 1.6
//
//  - Although many improvements were made to RenderMonkey FX  
//    file export, there are still situations that may cause   
//    compilation problems once the file is exported, such as  
//    occasional naming conflicts for methods, since FX format 
//    does not support any notions of name spaces. You need to 
//    try to create workspaces in such a way as to minimize    
//    potential naming conflicts on export.                    
//    
//  - Note that to minimize resulting name collisions in the FX 
//    file, RenderMonkey will mangle names for passes, shaders  
//    and function names as necessary to reduce name conflicts. 
//**************************************************************//

//--------------------------------------------------------------//
// ShaderTechnique
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string Crumbs_ShaderTechnique_Pass_0_Model : ModelData = "..\\..\\..\\Program Files\\AMD\\RenderMonkey 1.81\\Examples\\Media\\Models\\Sphere.3ds";
// globals

uniform float4x4 g_mtxViewProj : ViewProjection;
uniform float4x4 g_mtxWorld : World;


#define MAX_BONE_MATRICES 49
uniform float4x4 g_BoneMatricies[MAX_BONE_MATRICES];

struct VS_INPUT
{
	float4 Pos : POSITION0;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR0;
	float4 BlendWeights : BLENDWEIGHT;
	float4 BlendIndices : BLENDINDICES;
}; 

struct VS_OUTPUT
{
	float4 Pos : POSITION0;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR0;
};


VS_OUTPUT mainVS(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
 
	// Skinning (4 weights per vertex)
	In.BlendWeights.w = 1.0 - (In.BlendWeights.x + In.BlendWeights.y + In.BlendWeights.z); // Calculate 4th blend weight since only 3 are sent down
 
    	// Calculate position
    	float4 vWorldPos = mul(In.Pos, g_BoneMatricies[In.BlendIndices.x]) * In.BlendWeights.x;
	vWorldPos += mul(In.Pos, g_BoneMatricies[In.BlendIndices.y]) * In.BlendWeights.y;
	vWorldPos += mul(In.Pos, g_BoneMatricies[In.BlendIndices.z]) * In.BlendWeights.z;
	vWorldPos += mul(In.Pos, g_BoneMatricies[In.BlendIndices.w]) * In.BlendWeights.w;
 
 
	Out.Pos = mul(vWorldPos, g_mtxViewProj); // Put position into projection space
	Out.UV = In.UV; // pass uv straight through
 
	return Out;
}

texture s2D_TextureCoords_Tex
<
   string ResourceName = "..\\..\\..\\Program Files\\AMD\\RenderMonkey 1.81\\Examples\\Media\\Textures\\Fieldstone.tga";
>;
sampler2D s2D_TextureCoords = sampler_state
{
   Texture = (s2D_TextureCoords_Tex);
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   MINFILTER = LINEAR;
   MAGFILTER = LINEAR;
};
 
struct PS_INPUT 
{
   float4 Position : POSITION0;
   float2 Texcoord : TEXCOORD0;
   float4 Color : COLOR0;
};

float4 mainPS(VS_OUTPUT In) : COLOR
{
   float4 finalColor = In.Color;
	finalColor *= tex2D( s2D_TextureCoords, Input.UV );
   
   return finalColor;
}
 
technique Normal
{
	pass P0
	{
	   CULLMODE = CW;
	   SHADEMODE = PHONG;
	   VertexShader = compile vs_2_0 mainVS();
	   PixelShader = compile ps_2_0 mainPS();
    	}
}