// fragment.slang
struct FragOutput {
    float4 color : SV_Target;
};

[shader("fragment")]
FragOutput main() {
    FragOutput output;
    output.color = float4(0.6, 0.0, 0.9, 1.0);
    return output;
}