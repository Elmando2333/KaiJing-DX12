//BasicVS.hlsl 最基本的顶点着色器


//常量缓冲区，这个每帧更新一次
cbuffer PassConstants : register(b0)
{
    float4x4 gView; //视图矩阵
    float4x4 gProj; //投影矩阵
    float4x4 gViewProj;//视图投影矩阵
    float gEyePos;//这是相机的位置
    float pad1;
}

//常量缓冲区：这个每个对象有一个
cbuffer ObjectConstants : register(b1)
{
    float4x4 gWorld;//师姐矩阵
    float4x4 gWorldInvTranspose;
}


//shuru
struct VertexIn
{
    float3 PosL : POSITIONT;//局部局部局部
    float4 Color : COLOR;
};

struct VertexOut
{
    float PosH : SV_POSITION;
    float4 Color : COLOR;
};


VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    
    //先转换到世界空间
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    
    
    //换到齐次
    vout.PosH = mul(posW, gViewProj);
    
    //传递颜色
    vout.Color = vin.Color;
    
    
    return vout;
}