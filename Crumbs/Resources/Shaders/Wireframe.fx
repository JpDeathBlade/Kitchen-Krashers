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
// Position
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string Crumbs_Position_Pass_0_Model : ModelData = "..\\..\\..\\Program Files\\AMD\\RenderMonkey 1.81\\Examples\\Media\\Models\\Sphere.3ds";

float4x4 m4x4_ViewProjection : ViewProjection;

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float4 Color    : COLOR0;
   
};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float4 Color    : COLOR0;
   
};

VS_OUTPUT Crumbs_Position_Pass_0_Vertex_Shader_vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;

   Output.Position = mul( Input.Position, m4x4_ViewProjection );
   Output.Color = Input.Color;
   
   return( Output );
   
}




float4 v4_Color
<
   string UIName = "v4_Color";
   string UIWidget = "Direction";
   bool UIVisible =  true;
   float4 UIMin = float4( -10.00, -10.00, -10.00, -10.00 );
   float4 UIMax = float4( 10.00, 10.00, 10.00, 10.00 );
   bool Normalize =  false;
> = float4( 1.00, 0.00, 0.00, 1.00 );

struct PS_INPUT 
{
   float4 Position : POSITION0;
   float4 Color    : COLOR0;
   
};

float4 Crumbs_Position_Pass_0_Pixel_Shader_ps_main(PS_INPUT Input) : COLOR0
{   
   return(Input.Color);
   
}




//--------------------------------------------------------------//
// Technique Section for Effect Workspace.Crumbs.Position
//--------------------------------------------------------------//
technique Position
{
   pass Pass_0
   {
      VertexShader = compile vs_2_0 Crumbs_Position_Pass_0_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 Crumbs_Position_Pass_0_Pixel_Shader_ps_main();
      
      FillMode = Wireframe;
   }

}

