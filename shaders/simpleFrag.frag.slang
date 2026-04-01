import push;

// struct FragInput {
//     float3 fragColor : COLOR;
// };

struct FragOutput {
    float4 color : SV_Target;
};

[shader("fragment")]
FragOutput main() {
    FragOutput output;
    output.color = float4(push.color, 1.0);
    return output;
}