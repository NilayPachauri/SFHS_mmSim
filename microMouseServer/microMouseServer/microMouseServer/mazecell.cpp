#include "mazecell.h"

mazeCell::mazeCell()
{
    wallLeft = true;
    wallRight = true;
    wallBottom= true;
    wallTop= true;
    amount = -2;

    x = 0;
    y = 0;

    fCost = -10;
    gCost = -10;
    hCost = -10;
}
