#include "mazecell.h"

mazeCell::mazeCell()
{
    wallLeft = true;
    wallRight = true;
    wallBottom= true;
    wallTop= true;
    amount = -2;
}

void mazeCell::setLeftWall(bool val)  {
    wallLeft = val;
}

void mazeCell::setTopWall(bool val)   {
    wallTop = val;
}

void mazeCell::setRightWall(bool val) {
    wallRight = val;
}

void mazeCell::setBottomWall(bool val)  {
    wallBottom = val;
}
