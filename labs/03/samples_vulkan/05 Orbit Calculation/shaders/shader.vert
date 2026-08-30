#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in float inSize;

layout(push_constant) uniform PushConstants
{
	float scaleX;
	float scaleY;
} pc;

layout(location = 0) out vec3 fragColor;

void main()
{
	gl_Position = vec4(inPosition.x * pc.scaleX, -inPosition.y * pc.scaleY, 0.0, 1.0);
	gl_PointSize = inSize;
	fragColor = inColor;
}
