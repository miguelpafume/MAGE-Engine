import push;

[[vk::binding(0, 1)]]
Sampler2D texSampler;

struct FragInput {
    float3 fragColor : COLOR;
    float2 fragTexCoord : TEXCOORD;
};

struct FragOutput {
    float4 color : SV_Target;
};

[shader("fragment")]
FragOutput main(FragInput input) {
    FragOutput output;
    output.color = texSampler.Sample(input.fragTexCoord) * float4(input.fragColor, 1.0);
    return output;
}