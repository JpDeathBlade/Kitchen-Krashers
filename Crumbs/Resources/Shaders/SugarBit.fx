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
// Effects
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string Crumbs_Effects_Pass_0_Model : ModelData = "..\\..\\..\\Program Files\\AMD\\RenderMonkey 1.81\\Examples\\Media\\Models\\ElephantBody.3ds";

float f_Time : Time0_X;
float f_Bounce
<
   string UIName = "f_Bounce";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 25.00 );
float4x4 m4x4_ViewProjection : ViewProjection;

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float2 Texcoord : TEXCOORD0;
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float2 Texcoord : TEXCOORD0; 
};

VS_OUTPUT Crumbs_Effects_Pass_0_Vertex_Shader_mainVS( VS_INPUT Input )
{
   VS_OUTPUT Output;
   
   float offset = sin(f_Time * 0.1) * f_Bounce;

   float rot = 0.1;
   float4x4 finalMatrix = float4x4(1.0,0.0,0.0,0.0,
                        0.0,1.0,0.0,0.0,
                        0.0,0.0,1.0,0.0,
                        0.0,0.0,0.0,1.0);
   finalMatrix[0][0] = cos((f_Time * rot));
   finalMatrix[0][2] = -sin((f_Time * rot));
   finalMatrix[2][0] = sin((f_Time * rot));
   finalMatrix[2][2] = cos((f_Time * rot));
   
   finalMatrix = mul(finalMatrix, m4x4_ViewProjection);
   Output.Position = mul( Input.Position, finalMatrix );
   
   Output.Position.y += offset;
   Output.Texcoord = Input.Texcoord;

   return( Output );
   
}




texture s2D_TextureCoords_Tex
<
   string ResourceName = "..\\..\\..\\Program Files\\AMD\\RenderMonkey 1.81\\Examples\\Media\\Textures\\Coin.dds";
>;
sampler2D      s2D_TextureCoords = sampler_state
{
   Texture = (s2D_TextureCoords_Tex);
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   MINFILTER = LINEAR;
   MAGFILTER = LINEAR;
};

struct PS_INPUT 
{
   float2 Texcoord : TEXCOORD0;
};

float4 Crumbs_Effects_Pass_0_Pixel_Shader_mainPS( PS_INPUT Input ) : COLOR0
{
   float4 finalColor = {0.0, 0.0, 0.0, 1.0};

   finalColor = tex2D( s2D_TextureCoords, Input.Texcoord );
   
   return finalColor;
}




//--------------------------------------------------------------//
// Technique Section for Effect Workspace.Crumbs.Effects
//--------------------------------------------------------------//
technique ShaderTechnique
{
   pass Pass_0
   {
      CULLMODE = NONE;

      VertexShader = compile vs_2_0 Crumbs_Effects_Pass_0_Vertex_Shader_mainVS();
      PixelShader = compile ps_2_0 Crumbs_Effects_Pass_0_Pixel_Shader_mainPS();
   }

}

