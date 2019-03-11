#version 300 es
precision mediump float;

void main()
{
    vec2 v2TexCoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	gl_Position = vec4(v2TexCoord.x * 2.0 - 1.0, -v2TexCoord.y * 2.0 + 1.0, 0, 1.0);
}