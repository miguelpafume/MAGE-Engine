struct Push {
    float2 offset;
    float3 color;
};

[[vk::push_constant]]
uniform Push push;