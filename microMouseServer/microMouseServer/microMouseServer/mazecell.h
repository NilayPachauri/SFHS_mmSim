#ifndef MAZECELL_H
#define MAZECELL_H


class MazeCell
{
public:
    bool wallLeft;
    bool wallTop;
    bool wallRight;
    bool wallBottom;
    int amount;

    int x;
    int y;

    int fCost;
    int gCost;
    int hCost;

    MazeCell();
    bool isAdjacent(MazeCell *other, int amountLeft, int amountTop, int amountRight, int amountBottom);
};

#endif // MAZECELL_H
