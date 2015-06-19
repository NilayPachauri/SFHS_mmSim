#ifndef MAZECELL_H
#define MAZECELL_H


class mazeCell
{
public:
    bool wallLeft, wallTop, wallRight, wallBottom;
    int amount;
    mazeCell();
    void setLeftWall(bool val);
    void setTopWall(bool val);
    void setRightWall(bool val);
    void setBottomWall(bool val);
};

#endif // MAZECELL_H
