#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vertexarray.h>
#include <array.h>
#include <framebuffer.h>
#include <shader.h>
#include <texture.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float deltaTime;
double currTime;

struct Point{
  float x,y;
};
struct Grid{
  int cellsPerRow;
  struct Array* cells;
};
struct Array* CreateCells(int i)
{
  struct Array* cells = Array(sizeof(struct Point));
  struct Point p;
  for (int y = 0; y<=i; y++) {
    for (int x = 0; x<=i; x++) {
      p.x = (x/(float)i);
      p.y = 1.0f - (y/(float)i);
      ArrayPush(cells,&p);
    }
  }
  return cells;
}
float clamp(float f,float a,float b)
{
  return f<a?a:b<f?b:f;
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
  win = glfwCreateWindow(480,480, "game of life",NULL,NULL);
  glfwMakeContextCurrent(win);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  struct Shader *shader = Shader("./../res/default.vs","./../res/default.fs");
  struct VertexArray *gridVa,*borderVa;
  gridVa = VertexArray();
  borderVa = VertexArray();
  int cellsPerRow = 10;
  struct Array* cells = CreateCells(cellsPerRow);
  VertexArrayAddBuffer(gridVa, 0,cells->data,cells->size*cells->count,cells->size, 0);
  VertexArrayFormatBufferElement(gridVa, 0, TYPE_FLOAT, 2);
  char activeSquares[cellsPerRow*cellsPerRow];
  for (int y=0;y<cellsPerRow;y++)
  {
    for (int x=0;x<cellsPerRow;x++)
    {
      activeSquares[(y*cellsPerRow)+x] = 0;
      if (x==y) {
      activeSquares[(y*cellsPerRow)+x] = 1;
      }
    }
  }

  //struct Array *gridElements;
  //gridElements = Array(sizeof(unsigned int));
  //for (int i =0; i<cellsPerRow*cellsPerRow;i++) {
  //  if(activeSquares[i])
  //  {
  //    int xIndex = ((i/cellsPerRow) * (cellsPerRow+1)) + i%cellsPerRow;
  //    int yIndex = xIndex+(cellsPerRow+1);
  //    unsigned int cell[4] = {xIndex,xIndex+1,yIndex,yIndex+1};
  //    ArrayPush(gridElements,&cell[0]);
  //    ArrayPush(gridElements,&cell[3]);
  //    ArrayPush(gridElements,&cell[1]);
  //    ArrayPush(gridElements,&cell[0]);
  //    ArrayPush(gridElements,&cell[2]);
  //    ArrayPush(gridElements,&cell[3]);
  //  }
  //}
  unsigned int gridElements[cellsPerRow*cellsPerRow*6];
  //for (int i =0; i<cellsPerRow*cellsPerRow;i++) {
  //  if(activeSquares[i])
  //  {
  //    int xIndex = ((i/cellsPerRow) * (cellsPerRow+1)) + i%cellsPerRow;
  //    int yIndex = xIndex+(cellsPerRow+1);
  //    unsigned int cell[4] = {xIndex,xIndex+1,yIndex,yIndex+1};
  //    gridElements[i*6 + 0] = cell[0];
  //    gridElements[i*6 + 1] = cell[3];
  //    gridElements[i*6 + 2] = cell[1];
  //    gridElements[i*6 + 3] = cell[0];
  //    gridElements[i*6 + 4] = cell[2];
  //    gridElements[i*6 + 5] = cell[3];
  //  }
  //  else {
  //    gridElements[i*6 + 0] = 0;
  //    gridElements[i*6 + 1] = 0;
  //    gridElements[i*6 + 2] = 0;
  //    gridElements[i*6 + 3] = 0;
  //    gridElements[i*6 + 4] = 0;
  //    gridElements[i*6 + 5] = 0;
  //  }
  //}

  //VertexArrayAddBuffer(gridVa, 1,gridElements->data,gridElements->size*gridElements->count,gridElements->size, 0);
  VertexArrayAddBuffer(gridVa, 1,gridElements,sizeof(gridElements),sizeof(unsigned int), 0);
  VertexArraySetElementBuffer(gridVa,1);
  float scale = 1.0f;
  struct Array* borderElements;
  borderElements = Array(sizeof(struct Point));
  for (int y=0;y<=cellsPerRow;y++)
  {
    ArrayPush(borderElements,ArrayGetElement(cells,(y*(cellsPerRow+1))));
    ArrayPush(borderElements,ArrayGetElement(cells,(y*(cellsPerRow+1))+cellsPerRow));
  }
  for (int x=0;x<=cellsPerRow;x++)
  {
    ArrayPush(borderElements,ArrayGetElement(cells,x));
    ArrayPush(borderElements,ArrayGetElement(cells,(cellsPerRow*(cellsPerRow+1))+x));
  }
  VertexArrayAddBuffer(borderVa, 0,borderElements->data,borderElements->size*borderElements->count,borderElements->size, 0);
  VertexArrayAddBuffer(borderVa, 0,borderElements->data,borderElements->size*borderElements->count,borderElements->size, 0);
  VertexArrayFormatBufferElement(borderVa, 0, TYPE_FLOAT, 2);
  float aspect = 5.0f/4.0f;
  float offsetx = 0.0f , offsety = 0.0f;
  int limit = 5;
  char mDown=0;
  char changed = 1;
  while (!glfwWindowShouldClose(win)){
    if(changed)
    {
      changed = 0;
      for (int i =0; i<cellsPerRow*cellsPerRow;i++) {
        if(activeSquares[i])
        {
          int xIndex = ((i/cellsPerRow) * (cellsPerRow+1)) + i%cellsPerRow;
          int yIndex = xIndex+(cellsPerRow+1);
          unsigned int cell[4] = {xIndex,xIndex+1,yIndex,yIndex+1};
          gridElements[i*6 + 0] = cell[0];
          gridElements[i*6 + 1] = cell[3];
          gridElements[i*6 + 2] = cell[1];
          gridElements[i*6 + 3] = cell[0];
          gridElements[i*6 + 4] = cell[2];
          gridElements[i*6 + 5] = cell[3];
        }
        else {
          gridElements[i*6 + 0] = 0;
          gridElements[i*6 + 1] = 0;
          gridElements[i*6 + 2] = 0;
          gridElements[i*6 + 3] = 0;
          gridElements[i*6 + 4] = 0;
          gridElements[i*6 + 5] = 0;
        }
      }
      VertexArrayUpdateBuffer(gridVa, 1, sizeof(gridElements), gridElements);
      VertexArraySetElementBuffer(gridVa,1);
    }
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
    if (glfwGetKey(win,GLFW_KEY_S))
      offsety-=deltaTime;
    if (glfwGetKey(win,GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(win,GLFW_TRUE);

    if(glfwGetMouseButton(win,GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
      mDown = 0;
    if(glfwGetMouseButton(win,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !mDown)
    {
      mDown = 1;
      double_t mx_d,my_d;
      glfwGetCursorPos(win,&mx_d,&my_d);
      int x = ((mx_d/ width)*cellsPerRow);
      int y = ((my_d/height)*cellsPerRow);
      printf("Mouse:%d,%d\nSize:%d,%d\nIndex:%d,%d\naspect:%f\n-----------------\n",(int)mx_d,(int)my_d,width,height,x+1,y+1,aspect);
      changed = 1;
      activeSquares[(y*cellsPerRow)+x] = 1;
    }
    glfwSwapBuffers(win);
    glClear(GL_COLOR_BUFFER_BIT);
    VertexArrayBind(gridVa);
    UseShader(shader);
    glUniform1f(1,aspect);
    glDrawElements(GL_TRIANGLES,cellsPerRow*cellsPerRow*6,GL_UNSIGNED_INT,0);
    VertexArrayBind(borderVa);
    glDrawArrays(GL_LINES,0,borderElements->count);
    glfwPollEvents();
  }
  glfwDestroyWindow(win);
  glfwTerminate();
  return 0;
}
