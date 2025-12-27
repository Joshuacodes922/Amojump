struct VSInput {
    [[vk::location(0)]] float2 inPosition;
    [[vk::location(1)]] float3 inColor;
    [[vk::location(2)]] float2 inTexCoord;
};

struct VSOutput {
    float3 color;
    float4 pos : SV_Position;
    float2 fragTexCoord;
};

struct UniformBuffer {
    float4x4 view;
    float4x4 proj;
};

ConstantBuffer<UniformBuffer> ubo; 

struct PushConstants {
    float4x4 model;
};

[[vk::push_constant]] PushConstants pc;

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;
    output.pos = mul(ubo.proj, mul(ubo.view, mul(pc.model, float4(input.inPosition, 0.0, 1.0))));
    output.color = input.inColor;
    output.fragTexCoord = input.inTexCoord;
    return output;
}

Sampler2D texture;

[shader("fragment")]
float4 fragMain(VSOutput inVert) : SV_Target {
    return texture.Sample(inVert.fragTexCoord);
}
