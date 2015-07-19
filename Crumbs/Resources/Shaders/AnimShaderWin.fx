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
uniform float4x3 g_BoneMatricies[MAX_BONE_MATRICES];

struct VS_INPUT
{
   float3 Pos : POSITION0;
   float2 UV : TEXCOORD0;
   float3 BlendWeights : BLENDWEIGHT;
   float3 BlendIndices : BLENDINDICES;
   float3 PosOne : POSITION1;
   float3 PosTwo : POSITION2;
}; 

struct VS_OUTPUT
{
   float4 Pos : POSITION0;
   float2 UV : TEXCOORD0;
};


VS_OUTPUT mainVS(VS_INPUT In)
{
   VS_OUTPUT Out = (VS_OUTPUT)0;
 
   float4 vWorldPos         = float4(0,0,0,1.0f);
   float4 PosZero            = float4(In.Pos,1.0f);
   float4 PosOne            = float4(In.PosOne,1.0f);
   float4 PosTwo            = float4(In.PosTwo,1.0f);
   vWorldPos += In.BlendWeights.x * float4(mul(PosZero, g_BoneMatricies[In.BlendIndices.x]),1.0f);
   vWorldPos += In.BlendWeights.y * float4(mul(PosOne, g_BoneMatricies[In.BlendIndices.y]),1.0f);
   vWorldPos += In.BlendWeights.z * float4(mul(PosTwo, g_BoneMatricies[In.BlendIndices.z]),1.0f);
   vWorldPos.w = 1.0f;
 
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
};

float4 mainPS(PS_INPUT In) : COLOR
{
   float4 finalColor = tex2D( s2D_TextureCoords, In.Texcoord );
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