#version 450

layout(set=0,binding = 0) uniform UniformBufferObject {
    mat4 proj;
    mat4 view; 
} vp;

layout(push_constant) uniform PushConstants {
    mat4 model; 
} mesh;

// per vertex
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

// per instance
layout(location = 4) in vec4 modelC0;
layout(location = 5) in vec4 modelC1;
layout(location = 6) in vec4 modelC2;
layout(location = 7) in vec4 modelC3;
layout(location = 8) in vec2 texCoordOffset;

// Use the attributes to construct a mat4
mat4 model = mat4(modelC0, modelC1, modelC2, modelC3);

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec2 fragTexCoord;

void main() {
    gl_Position = vp.proj * vp.view * model * vec4(inPosition,1);
    vec4 tNormal = model * vec4(inNormal,0);
    fragNormal = normalize(tNormal.xyz);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    // 32 pixels width and height of cell in texture atlas.
    //fragTexCoord = (texCoordOffset + inTexCoord)/32.0f;
}