#include "cell.h"

Cell::Cell()
{
    visited = false;
    wallLeft = true;
    wallTop = true;
    wallRight = true;
    wallBottom = true;

    x = 0;
    y = 0;


//    QVector <Cell> adjacentUnexploredCells = QVector <Cell> ();
}

bool Cell::getVisited() {
    return visited;
}

bool Cell::getWallLeft()     {
    return wallLeft;
}

bool Cell::getWallTop() {
    return wallTop;
}

bool Cell::getWallRight()   {
    return wallRight;
}

bool Cell::getWallBottom() {
    return wallBottom;
}

int Cell::getX() {
    return x;
}

int Cell::getY() {
    return y;
}

void Cell::setVisited(bool visited) {
    this->visited = visited;
}

void Cell::setWallLeft(bool wallLeft) {
    this->wallLeft = wallLeft;
}

void Cell::setWallTop(bool wallTop) {
    this->wallTop = wallTop;
}

void Cell::setWallRight(bool wallRight) {
    this->wallRight = wallRight;
}

void Cell::setWallBottom(bool wallBottom) {
    this->wallBottom = wallBottom;
}

void Cell::setX (int x)  {
    this->x = x;
}

void Cell::setY(int y) {
    this->y = y;
}

void Cell::addAdjacentUnexploredCell(Cell unexploredCell)  {
    this->adjacentUnexploredCells.append(unexploredCell);
}

void Cell::removeFirstAdjacentUnexploredCell()  {
    this->adjacentUnexploredCells.removeFirst();
}

QVector<Cell> Cell::getAdjacentUnexploredCell() {
    return this->adjacentUnexploredCells;
}

bool Cell::equals(Cell other)   {
    return (this->getX() == other.getX()) && (this->getY() == other.getY()) ? true : false;
}
