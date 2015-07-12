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

bool mazeCell::isAdjacent(mazeCell* other)   {
    if (((this->x - 1) == other->x) && (this->y == other->y))   {
        return true;
    }
    else if ((this->x == other->x) && ((this->y - 1) == other->y))  {
        return true;
    }
    else if (((this->x + 1) == other->x) && (this->y == other->y))  {
        return true;
    }
    else if ((this->x == other->x) && ((this->y + 1) == other->y))  {
        return true;
    }
    else    {
        return false;
    }
}
