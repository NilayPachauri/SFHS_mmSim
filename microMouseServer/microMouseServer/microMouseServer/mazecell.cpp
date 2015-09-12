#include "mazecell.h"

MazeCell::MazeCell()
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

bool MazeCell::isAdjacent(MazeCell* other,int amountLeft, int amountTop, int amountRight, int amountBottom) {
    if (((this->x - 1) == other->x) && (this->y == other->y) && (amountLeft >= 0))   {
        return true;
    }
    else if ((this->x == other->x) && ((this->y + 1) == other->y) && (amountTop >= 0))   {
        return true;
    }
    else if (((this->x + 1) == other->x) && (this->y == other->y) && (amountRight >= 0)) {
        return true;
    }
    else if ((this->x == other->x) && ((this->y - 1) == other->y) && (amountBottom >= 0))    {
        return true;
    }
    else    {
        return false;
    }
}
