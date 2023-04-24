#include <grid.h>
#include <array.h>
#include <stdlib.h>
#include <string.h>

struct Grid* Grid(int i)
{
  struct Grid *grid = malloc(sizeof (struct Grid));
  grid->cellsPerSide = i;
  grid->cellsMap = Array(sizeof(float)*2);
  float p[2];
  for (int y = 0; y<=i; y++) {
    for (int x = 0; x<=i; x++) {
      p[0] = (x/(float)i);
      p[1] = 1.0f - (y/(float)i);
      ArrayPush(grid->cellsMap,p);
    }
  }
  grid->activeCells= malloc(sizeof(unsigned int)*grid->cellsPerSide*grid->cellsPerSide);
  for (int y=0;y<grid->cellsPerSide;y++)
  {
    for (int x=0;x<grid->cellsPerSide;x++)
    {
      grid->activeCells[(y*grid->cellsPerSide)+x] = 0;
    }
  }
  grid->borderMap = Array(sizeof(float)*2);
  for (int y=0;y<=grid->cellsPerSide;y++)
  {
    ArrayPush(grid->borderMap,ArrayGetElement(grid->cellsMap,(y*(i+1))));
    ArrayPush(grid->borderMap,ArrayGetElement(grid->cellsMap,(y*(i+1))+i));
  }
  for (int x=0;x<=grid->cellsPerSide;x++)
  {
    ArrayPush(grid->borderMap,ArrayGetElement(grid->cellsMap,x));
    ArrayPush(grid->borderMap,ArrayGetElement(grid->cellsMap,(i*(i+1))+x));
  }
  grid->changed = 0;
  grid->cellsElements = malloc(sizeof(unsigned int)*grid->cellsPerSide*grid->cellsPerSide*6);
  grid->cellsElementsCount = 0;
  grid->cellUpdateProc = 0;
  return grid;
}
void GridUpdate(struct Grid* grid)
{
  int gridElementsCounter = 0;
  grid->changed = 0;
  grid->cellsElementsCount = 0;
  for (int i =0; i<grid->cellsPerSide*grid->cellsPerSide;i++) {
    if(grid->activeCells[i])
    {
      int xIndex = ((i/grid->cellsPerSide) * (grid->cellsPerSide+1)) + i%grid->cellsPerSide;
      int yIndex = xIndex+(grid->cellsPerSide+1);
      unsigned int cell[4] = {xIndex,xIndex+1,yIndex,yIndex+1};
      grid->cellsElements[gridElementsCounter*6 + 0] = cell[0];
      grid->cellsElements[gridElementsCounter*6 + 1] = cell[3];
      grid->cellsElements[gridElementsCounter*6 + 2] = cell[1];
      grid->cellsElements[gridElementsCounter*6 + 3] = cell[0];
      grid->cellsElements[gridElementsCounter*6 + 4] = cell[2];
      grid->cellsElements[gridElementsCounter*6 + 5] = cell[3];
      gridElementsCounter++;
    }
  }
  grid->cellsElementsCount = gridElementsCounter*6;
}
void GridCallProc(struct Grid* grid)
{
  char newGrid[grid->cellsPerSide*grid->cellsPerSide];
  for (int y=0;y<grid->cellsPerSide;y++)
  {
    for (int x=0;x<grid->cellsPerSide;x++)
    {
      newGrid[(y*grid->cellsPerSide)+x] = grid->cellUpdateProc(grid,x,y);
    }
  }
  memcpy(grid->activeCells,newGrid,grid->cellsPerSide*grid->cellsPerSide*sizeof(char));
}
char GridGetCell(struct Grid* grid,unsigned x,unsigned y)
{
  if((0>x||x>=grid->cellsPerSide)||(0>y||y>=grid->cellsPerSide))
    return 0;
  return grid->activeCells[(y*grid->cellsPerSide)+x];
}
void GridSetCellsProc(struct Grid* grid,char (*cellProc)(struct Grid*,int,int))
{
  grid->cellUpdateProc = cellProc;
}
void GridSetCell(struct Grid* grid,unsigned x,unsigned y,char value)
{
  if(0>x||x>grid->cellsPerSide||0>y||y>grid->cellsPerSide)
    return;
  grid->activeCells[(y*grid->cellsPerSide)+x] = value==0?0:1;
  grid->changed = 1;
}
void GridFree(struct Grid* grid)
{
  free(grid->activeCells);
  free(grid->cellsElements);
  ArrayFree(grid->cellsMap);
  ArrayFree(grid->borderMap);
  free(grid);
}
