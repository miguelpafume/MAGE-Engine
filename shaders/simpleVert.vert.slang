struct VertInput {
    float2 position : POSITION;
};

struct VertOutput {
    float4 pos : SV_Position;
};

[shader("vertex")]
VertOutput main(VertInput input) {
    VertOutput output;
    output.pos = float4(input.position, 0.0, 1.0);
    return output;
}