#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;


layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(normalize(abs(inNormal)), 1.0);
    // outColor = vec4(fs_inUV, 0.0, 1.0);
}
