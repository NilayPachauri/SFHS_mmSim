#ifndef MAZECELL_H
#define MAZECELL_H


class mazeCell
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

    mazeCell();
    bool isAdjacent(mazeCell *other);
};

#endif // MAZECELL_H
