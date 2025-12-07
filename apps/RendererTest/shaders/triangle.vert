#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragposition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV;


void main()
{
    gl_Position = vec4(inPosition, 1.0);
    fragposition = gl_Position.xyz;
    fragUV = inUV;
    fragNormal = inNormal;
}
