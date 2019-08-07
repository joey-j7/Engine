#version 320 es

#ifdef GL_GOOGLE_include_directive
#extension GL_GOOGLE_include_directive : require
#endif

#include "defines.h"

precision mediump float;

void main()
{
    vec2 v2TexCoord = vec2((VERT_ID << 1) & 2, VERT_ID & 2);
	gl_Position = vec4(v2TexCoord.x * 2.0 - 1.0, -v2TexCoord.y * 2.0 + 1.0, 0, 1.0);
}