#ifndef CELL_H
#define CELL_H

#include <QVector>

class Cell
{
public:

    Cell();

    bool getVisited();
    bool getWallLeft();
    bool getWallTop();
    bool getWallRight();
    bool getWallBottom();
    int getX();
    int getY();

    void setVisited(bool visited);
    void setWallLeft(bool wallLeft);
    void setWallTop(bool wallTop);
    void setWallRight(bool wallRight);
    void setWallBottom(bool wallBottom);
    void setX(int x);
    void setY(int y);

    void removeFirstAdjacentUnexploredCell();
    void addAdjacentUnexploredCell(Cell unexploredCell);
    QVector <Cell> getAdjacentUnexploredCell();

    bool equals(Cell other);

private:

    bool visited;
    bool wallLeft;
    bool wallTop;
    bool wallRight;
    bool wallBottom;

    int x;
    int y;

    QVector <Cell> adjacentUnexploredCells;
};

#endif // CELL_H
