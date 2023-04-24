#version 460 core
layout (location = 0 ) uniform  vec3 color;
out vec4 FragColor;
void main()
{
  //if(Out_pos.x > aspect && Out_pos.x< -aspect)
  //  FragColor = vec4(1,0,0,1);
  //else
  //  FragColor = vec4(0,1,0,1);
  FragColor = vec4(color,1);
}
