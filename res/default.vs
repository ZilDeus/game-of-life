#version 460 core
layout (location = 0) in vec4 aPos;
layout (location = 1) uniform float Uaspect;
void main()
{
  vec2 pos = aPos.xy * 2 -1;
  //pos.x*=Uaspect;
	gl_Position = vec4(pos,1.0f,1.0f);
}
