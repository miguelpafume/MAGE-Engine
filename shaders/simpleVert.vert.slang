import push;
import ubo;

struct VertInput {
    float3 position : POSITION;
    float3 color : COLOR;
    float2 texCoord : TEXCOORD;
};

struct VertOutput {
    float4 pos : SV_Position;
    float3 fragColor : COLOR;
    float2 fragTexCoord : TEXCOORD;
};

[shader("vertex")]
VertOutput main(VertInput input) {
    VertOutput output;
    output.pos = mul(ubo.projectionViewMatrix, mul(push3d.modelMatrix, float4(input.position, 1.0)));
    output.fragColor = input.color;
    output.fragTexCoord = input.texCoord;
    return output;
}
