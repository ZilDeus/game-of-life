#include <grid.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vertexarray.h>
#include <array.h>
#include <framebuffer.h>
#include <shader.h>
#include <texture.h>
#include <math.h>
#include <stdio.h>
#include <grid.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

float deltaTime;
double currTime,prevTime;

float clamp(float f,float a,float b)
{
  return f<a?a:b<f?b:f;
}
char testProc(struct Grid* grid,int x,int y)
{
  char isAlive = GridGetCell(grid,x,y);
  int count = 0;
  count+=GridGetCell(grid, x+1,y);
  count+=GridGetCell(grid, x-1,y);
  count+=GridGetCell(grid, x,y+1);
  count+=GridGetCell(grid, x,y-1);
  count+=GridGetCell(grid, x+1,y+1);
  count+=GridGetCell(grid, x-1,y+1);
  count+=GridGetCell(grid, x+1,y-1);
  count+=GridGetCell(grid, x-1,y-1);
  if(!isAlive)
  {
    return count==3;
  }
  else {
    return (count==2||count==3)?1:0;
  }
}
int main()
{
  if(!glfwInit())
    return -1;
  currTime = glfwGetTime();
  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  glfwWindowHint(GLFW_DECORATED,GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);
  glfwWindowHint(GLFW_AUTO_ICONIFY,GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  GLFWwindow* win;
  win = glfwCreateWindow(600,600, "game of life",NULL,NULL);
  glfwMakeContextCurrent(win);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  struct Shader *shader = Shader("./../res/default.vs","./../res/default.fs");
  struct VertexArray *gridVa,*borderVa;
  gridVa = VertexArray();
  borderVa = VertexArray();
  int cellsPerRow = 20;
  struct Grid* grid = Grid(cellsPerRow);
  GridSetCellsProc(grid, testProc);
  VertexArrayAddBuffer(gridVa, 0,grid->cellsMap->data,grid->cellsMap->size*grid->cellsMap->count,grid->cellsMap->size, 0);
  VertexArrayFormatBufferElement(gridVa, 0, TYPE_FLOAT, 2);
  VertexArrayAddBuffer(gridVa, 1,grid->cellsElements,grid->cellsElementsCount * sizeof(unsigned int),sizeof(unsigned int), 0);
  VertexArraySetElementBuffer(gridVa,1);
  float scale = 1.0f;
  VertexArrayAddBuffer(borderVa, 0,grid->borderMap->data,grid->borderMap->size*grid->borderMap->count,grid->borderMap->size, 0);
  VertexArrayAddBuffer(borderVa, 0,grid->borderMap->data,grid->borderMap->size*grid->borderMap->count,grid->borderMap->size, 0);
  VertexArrayFormatBufferElement(borderVa, 0, TYPE_FLOAT, 2);
  float aspect = 5.0f/4.0f;
  float offsetx = 0.0f , offsety = 0.0f;
  int limit = 5;
  char mDown=0;
  glfwSetInputMode(win,GLFW_STICKY_KEYS,GLFW_FALSE);
  while (!glfwWindowShouldClose(win)){
    prevTime = currTime;
    GridUpdate(grid);
    VertexArrayUpdateBuffer(gridVa, 1, grid->cellsElementsCount*sizeof(unsigned int), grid->cellsElements);
    VertexArraySetElementBuffer(gridVa,1);
    int width,height;
    glfwGetWindowSize(win, &width,&height);
    aspect = (float)height/(float)width;
    glViewport(0,0,width,height);
    deltaTime = glfwGetTime() - currTime;
    currTime = glfwGetTime();
    scale = clamp(scale,aspect,10.f);
    if(glfwGetKey(win,GLFW_KEY_A))
      offsetx-=deltaTime;
    if(glfwGetKey(win,GLFW_KEY_D))
      offsetx+=deltaTime;
    if(glfwGetKey(win,GLFW_KEY_W))
      offsety+=deltaTime;
    if (glfwGetKey(win,GLFW_KEY_S) == GLFW_RELEASE)
        mDown = 1;
    if (glfwGetKey(win,GLFW_KEY_S))
    {
      GridCallProc(grid);
      offsety-=deltaTime;
      Sleep(100);
    }
    if (glfwGetKey(win,GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(win,GLFW_TRUE);

    if((glfwGetMouseButton(win,GLFW_MOUSE_BUTTON_2) == GLFW_PRESS||glfwGetMouseButton(win,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS))
    {
      double_t mx_d,my_d;
      glfwGetCursorPos(win,&mx_d,&my_d);
      int x = ((mx_d/ width)*cellsPerRow);
      int y = ((my_d/height)*cellsPerRow);
      GridSetCell(grid,x,y,glfwGetMouseButton(win,GLFW_MOUSE_BUTTON_2) == GLFW_PRESS?0:1);
    }
    glfwSwapBuffers(win);
    glClear(GL_COLOR_BUFFER_BIT);
    VertexArrayBind(gridVa);
    UseShader(shader);
    glUniform1f(1,aspect);
    glDrawElements(GL_TRIANGLES,grid->cellsElementsCount*6,GL_UNSIGNED_INT,0);
    VertexArrayBind(borderVa);
    glDrawArrays(GL_LINES,0,grid->borderMap->count);
    glfwPollEvents();
  }
  glfwDestroyWindow(win);
  glfwTerminate();
  return 0;
}
