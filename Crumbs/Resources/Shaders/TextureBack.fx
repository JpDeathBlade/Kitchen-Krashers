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

float4x4 m4x4_ViewProjection : ViewProjection;

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float2 Texcoord : TEXCOORD0;
   float4 Color : COLOR0;
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float2 Texcoord : TEXCOORD0;
   float4 Color : COLOR0;
};

VS_OUTPUT Crumbs_ShaderTechnique_Pass_0_Vertex_Shader_ShaderVS( VS_INPUT Input )
{
   VS_OUTPUT Output;

   Output.Position = mul( Input.Position, m4x4_ViewProjection );
   Output.Texcoord = Input.Texcoord;
   Output.Color = Input.Color;

   return( Output );
   
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

float4 Crumbs_ShaderTechnique_Pass_0_Pixel_Shader_ShaderPS( PS_INPUT Input ) : COLOR0
{
   float4 finalColor = Input.Color;
	finalColor *= tex2D( s2D_TextureCoords, Input.Texcoord );
   
   return finalColor;
}
//--------------------------------------------------------------//
// Technique Section for Effect Workspace.Crumbs.ShaderTechnique
//--------------------------------------------------------------//
technique ShaderTechnique
{
   pass Pass_0
   {
      CULLMODE = CCW;
      SRCBLEND = SRCALPHA;
      ALPHABLENDENABLE = TRUE;
	  SHADEMODE = PHONG;
	  
      VertexShader = compile vs_2_0 Crumbs_ShaderTechnique_Pass_0_Vertex_Shader_ShaderVS();
      PixelShader = compile ps_2_0 Crumbs_ShaderTechnique_Pass_0_Pixel_Shader_ShaderPS();
   }

	pass Pass_1
   {
      ALPHABLENDENABLE = TRUE;
      CULLMODE = CW;
      SHADEMODE = PHONG;
      SRCBLEND = SRCALPHA;

      VertexShader = compile vs_2_0 Crumbs_ShaderTechnique_Pass_0_Vertex_Shader_ShaderVS();
      PixelShader = compile ps_2_0 Crumbs_ShaderTechnique_Pass_0_Pixel_Shader_ShaderPS();
   }
}

