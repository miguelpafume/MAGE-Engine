struct GlobalUbo {
    float4x4 projectionViewMatrix;
    float4 directionToLight;
};

[[vk::binding(0, 0)]]
ConstantBuffer<GlobalUbo> ubo;