//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
    float4 DiffuseMtrl;
    float4 DiffuseLight;
    float3 LightVecW;
    float p1;
    float4 AmbientMtrl;
    float4 AmbientLight;
    float4 SpecularMtrl;
    float4 SpecularLight;
    float3 EyePosW;
    float p2;
    float SpecularPower;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float3 PosW : POSITION;
};

//------------------------------------------------------------------------------------
// Vertex Shader - Implements Gouraud Shading using Diffuse lighting only
//------------------------------------------------------------------------------------

VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.Pos = mul(Pos, World);
    
    // Compute the vector from the vertex to the eye position.
    // output.Pos is currently the position in world space
    output.PosW = normalize(EyePosW - output.Pos.xyz);

    // Apply View and Projection transformations
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    // Convert normal from local space to world space
    // OLD COMMENT: Convert from local space to world space
    // W component of vector is 0 as vectors cannot be translated
    float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
    normalW = normalize(normalW);
    output.Norm = normalW;
    
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    float4 f = float4(0.0f, 0.0f, 0.0f, 1.0f);

    // Compute Colour
    float3 diffuse = DiffuseMtrl * DiffuseLight;
    float3 ambient = AmbientMtrl * AmbientLight;
    float3 specular = SpecularMtrl * SpecularLight;
    
    // Compute the reflection vector.
    float3 r = reflect(-LightVecW, input.Norm);

    // Determine how much (if any) specular light makes it into the eye.
    float specularAmount = pow(max(dot(r, input.PosW), 0.0f), SpecularPower);
    float diffuseAmount = max(dot(LightVecW, input.Norm), 0.0f);
    
    // Calculate Diffuse and Ambient Lighting
    f.rgb = diffuseAmount * diffuse.rgb + ambient + specularAmount * specular.rgb;
    f.a = DiffuseMtrl.a;
    return f;
}
