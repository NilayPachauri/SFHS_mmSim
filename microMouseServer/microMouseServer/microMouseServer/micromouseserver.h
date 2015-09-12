#ifndef MICROMOUSESERVER_H
#define MICROMOUSESERVER_H
#include <cell.h>
#include "mazeConst.h"
#include "mazeBase.h"
#include "mazegui.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QLineF>
#include <QTimer>

#include <mazecell.h>



namespace Ui {
class microMouseServer;
}

class microMouseServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit microMouseServer(QWidget *parent = 0);
    ~microMouseServer();

private slots:
    void on_tabWidget_tabBarClicked(int index);
    void loadMaze();


    void generateMaze();

    void makeMaze(Cell map[][20], int x, int y, Cell exitCell);

    void saveMaze();
    void addLeftWall(QPoint cell);
    void addRightWall(QPoint cell);
    void addTopWall(QPoint cell);
    void addBottomWall(QPoint cell);
    void removeRightWall(QPoint cell);
    void removeLeftWall(QPoint cell);
    void removeTopWall(QPoint cell);
    void removeBottomWall(QPoint cell);
    void netComs();
    void connect2mouse();
    void startAI();
    void studentAI();



private:
    bool isWallLeft();
    bool isWallRight();
    bool isWallForward();
    bool moveForward();
    void turnLeft();
    void turnRight();
    void foundFinish();
    void printUI(const QString mesg);

    bool isWallBack();

    MazeCell& positionCurrent();
    MazeCell& positionLeft();
    MazeCell& positionTop();
    MazeCell& positionRight();
    MazeCell& positionBottom();

    int amountLeft();
    int amountTop();
    int amountRight();
    int amountBottom();

    void moveLeft();
    void realMoveForward();
    void moveRight();
    void moveBack();

    int numberOfWalls();
    void deadEnd();

    void assignPositions();
    void assignAccessible();
    void assignWalls();
    void leftHandRule();
    void checkExit(int xValue, int yValue);

    void firstRun();

    bool isWallLeft(int x, int y);
    bool isWallForward(int x, int y);
    bool isWallRight(int x, int y);
    bool isWallBack(int x, int y);
    bool isEnd(MazeCell *cell, int x, int y);

    MazeCell& positionCurrent(int x, int y);
    MazeCell& positionLeft(int x, int y);
    MazeCell& positionTop(int x, int y);
    MazeCell& positionRight(int x, int y);
    MazeCell& positionBottom(int x, int y);

    int amountLeft(int x, int y);
    int amountTop(int x, int y);
    int amountRight(int x, int y);
    int amountBottom(int x, int y);

    void zeroValues(int x, int y);
    void resetCosts();
    void setAdjacentCosts(int x, int y);

    void shellSort(QVector <MazeCell*> * array);
    void insertionSort(QVector <MazeCell*> * array);

    void removeChecked();
    void shortestPathDirections(int x, int y);

    void shortestPathFinder(int x, int y, int xEnd, int yEnd);

    void navigateShortestPath();
    bool atExit();

    void secondRun();

    void shortPathFinder();

    QTimer *_comTimer;
    QTimer *_aiCallTimer;
    static const int _mDelay = 100;
    Ui::microMouseServer *ui;
    mazeGui *maze;
    std::vector<QGraphicsLineItem*> backgroundGrid;
    struct baseMapNode mazeData[MAZE_WIDTH][MAZE_HEIGHT];
    void connectSignals();
    void initMaze();
};

#endif // MICROMOUSESERVER_H
