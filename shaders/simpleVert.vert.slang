import push;

struct VertInput {
    float2 position : POSITION;
    float3 color : COLOR;
};

struct VertOutput {
    float4 pos : SV_Position;
    float3 fragColor : COLOR;
};

[shader("vertex")]
VertOutput main(VertInput input) {
    VertOutput output;
    // Transform * Position + Offset
    output.pos = float4(mul(push.transform, input.position) + push.offset, 0.0, 1.0);
    output.fragColor = input.color;
    return output;
}