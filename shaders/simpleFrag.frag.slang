struct FragInput {
    float3 fragColor : COLOR;
};

struct FragOutput {
    float4 color : SV_Target;
};

[shader("fragment")]
FragOutput main(FragInput input) {
    FragOutput output;
    output.color = float4(input.fragColor, 1.0);
    return output;
}