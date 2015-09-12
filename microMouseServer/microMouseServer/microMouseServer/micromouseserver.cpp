#include "micromouseserver.h"
#include "cell.h"
#include "ui_micromouseserver.h"
#include "mazeConst.h"
#include "mazegui.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include <mazecell.h>

#include <tgmath.h>


microMouseServer::microMouseServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::microMouseServer)
{
    maze = new mazeGui;
    _comTimer = new QTimer(this);
    _aiCallTimer = new QTimer(this);
    ui->setupUi(this);
    connectSignals();

    ui->graphics->scale(1,-1);
    ui->graphics->setBackgroundBrush(QBrush(Qt::black));
    ui->graphics->setAutoFillBackground(true);
    ui->graphics->setScene(maze);

    this->initMaze();
    this->maze->drawGuideLines();
    this->maze->drawMaze(this->mazeData);

}


microMouseServer::~microMouseServer()
{
    delete ui;
    delete _comTimer;
    delete maze;
}


void microMouseServer::on_tabWidget_tabBarClicked(int index)
{
    index = index;
    //left for testing, does nothing in this rev, will remove in nex rev
}

void microMouseServer::connectSignals()
{
    //connect all signals

    connect(ui->menu_loadMaze, SIGNAL(triggered()), this, SLOT(loadMaze()));
    connect(ui->menu_generateMaze, SIGNAL(triggered()), this, SLOT(generateMaze()));
    connect(ui->menu_saveMaze, SIGNAL(triggered()), this, SLOT(saveMaze()));
    connect(ui->menu_connect2Mouse, SIGNAL(triggered()), this, SLOT(connect2mouse()));
    connect(ui->menu_startRun, SIGNAL(triggered()), this, SLOT(startAI()));

    connect(_comTimer, SIGNAL(timeout()), this, SLOT(netComs()));
    connect(_aiCallTimer, SIGNAL(timeout()), this, SLOT(studentAI()));

    connect(this->maze, SIGNAL(passTopWall(QPoint)), this, SLOT(addTopWall(QPoint)));
    connect(this->maze, SIGNAL(passBottomWall(QPoint)), this, SLOT(addBottomWall(QPoint)));
    connect(this->maze, SIGNAL(passLeftWall(QPoint)), this, SLOT(addLeftWall(QPoint)));
    connect(this->maze, SIGNAL(passRightWall(QPoint)), this, SLOT(addRightWall(QPoint)));
    connect(this->maze, SIGNAL(takeBottomWall(QPoint)),this,SLOT(removeBottomWall(QPoint)));
    connect(this->maze, SIGNAL(takeTopWall(QPoint)),this,SLOT(removeTopWall(QPoint)));
    connect(this->maze, SIGNAL(takeLeftWall(QPoint)),this,SLOT(removeLeftWall(QPoint)));
    connect(this->maze, SIGNAL(takeRightWall(QPoint)),this,SLOT(removeRightWall(QPoint)));
}
void microMouseServer::connect2mouse()
{
    //cut for mac compatibility
}

void microMouseServer::netComs()
{
    //cut for mac compatibility
}

void microMouseServer::loadMaze()
{
    //open file find window
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Maze File"), "./", tr("Maze Files (*.maz)"));
    QFile inFile(fileName);

    //if the file can't be opened throw error to ui
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->txt_debug->append("ERROR 202: file not found");
        return;
    }

    //read maze
    QTextStream mazeFile(&inFile);

    int largestX =0, largestY=0;
    int x, y, wallTop, wallBottom, wallLeft,wallRight;
    while(!mazeFile.atEnd())
    {
        mazeFile >> x;
        mazeFile >> y;
        mazeFile >> wallTop;
        mazeFile >> wallBottom;
        mazeFile >> wallLeft;
        mazeFile >> wallRight;

        //check formating
        if(x<0 || y<0 || wallTop<0 || wallBottom<0 || wallLeft<0 || wallRight<0 ||
                wallTop>1 || wallBottom>1 || wallLeft>1 || wallRight>1)
        {
            ui->txt_debug->append("ERROR 201: file formating error");
            return;
        }

        //check x boundary
        if(x > largestX)
        {
            if(x > MAZE_WIDTH)
            {
                ui->txt_debug->append("ERROR 204: maze file is wider than max maze size");
            }
            else
            {
                largestX = x;
            }
        }

        //check y boundary
        if(y > largestY)
        {
            if(y > MAZE_HEIGHT)
            {
                ui->txt_debug->append("ERROR 205: maze file is taller than max maze size");
            }
            else
            {
                largestY = y;
            }
        }

        baseMapNode *mover = &this->mazeData[x-1][y-1];
        //load data into maze
        mover->setXY(x,y);

        if(wallLeft)
        {
            mover->setWall(LEFT, NULL);
        }
        else
        {
            // mover->setWall(LEFT, &this->mazeData[x-1][y]);
        }

        if(wallRight)
        {
            mover->setWall(RIGHT, NULL);
        }
        else
        {
            //mover->setWall(RIGHT, &this->mazeData[x+1][y]);
        }
        if(wallTop)
        {
            mover->setWall(TOP, NULL);
        }
        else
        {
            //mover->setWall(TOP, &this->mazeData[x][y+1]);
        }

        if(wallBottom)
        {
            mover->setWall(BOTTOM, NULL);
        }
        else
        {
            // mover->setWall(BOTTOM, &this->mazeData[x][y-1]);
        }
    }
    //ui->txt_debug->append("Maze loaded");
    mazeFile.flush();
    inFile.close();

    //draw maze and mouse
    this->maze->drawMaze(this->mazeData);
    this->maze->drawMouse(QPoint(1,1),dUP);
}

//void microMouseServer::generateMaze(Cell map[20][20])

void microMouseServer::generateMaze()   {
    //generate a random maze

    int userInputX;
    int userInputY;

    srand(time(NULL));  //seeding the random generator so we can make it as random as possible, in this case its seeded based on the time

    int xStartingPosition = 0;  //setting the starting position
    int yStartingPosition = 0;
    int xExitPosition = rand()%MAZE_WIDTH-1;  //and exit position between 0 and 18 b/c the other open cells will be +1 x, +1 y, and +1 x and y
    int yExitPosition = rand()%MAZE_HEIGHT-1;  //hence it'll reach the bounds of the array

    Cell map[MAZE_WIDTH][MAZE_HEIGHT];  //initializing data structure which will hold the generated maze

    for (int i = 0; i < MAZE_WIDTH; i++)    {   //giving all the values of the map their correct x and y values
        for (int j = 0; j < MAZE_HEIGHT; j++)   {
            map[i][j].setX(i);
            map[i][j].setY(j);
        }
    }

    map[xExitPosition][yExitPosition].setWallTop(false);    //setting the inside of the square to be empty
    map[xExitPosition][yExitPosition].setWallRight(false);
    map[xExitPosition+1][yExitPosition].setWallLeft(false);
    map[xExitPosition+1][yExitPosition].setWallTop(false);
    map[xExitPosition+1][yExitPosition+1].setWallLeft(false);
    map[xExitPosition+1][yExitPosition+1].setWallBottom(false);
    map[xExitPosition][yExitPosition+1].setWallRight(false);
    map[xExitPosition][yExitPosition+1].setWallBottom(false);

    srand(time(NULL));

    makeMaze(map,xStartingPosition,yStartingPosition,map[xExitPosition][yExitPosition]);

//    for (int i = 0; i < MAZE_WIDTH; i++) {
//        for (int j = 0; j < MAZE_HEIGHT; j++)   {

//            if (map[i][j].getWallLeft())   {
//                addLeftWall(QPoint(i,j));
//            }
//            if (map[i][j].getWallTop())  {
//                addTopWall(QPoint(i,j));
//            }
//            if (map[i][j].getWallRight())    {
//                addRightWall(QPoint(i,j));
//            }
//            if (map[i][j].getWallBottom()) {
//                addBottomWall(QPoint(i,j));
//            }
//        }
//    }

//    baseMapNode* moverArray[MAZE_WIDTH][MAZE_HEIGHT]; //drawing what has been made

//    for (int i = 0; i < MAZE_WIDTH; i++)    {
//        for (int j = 0; j < MAZE_HEIGHT; j++)   {
//            moverArray[i][j] = &this->mazeData[i][j];
//        }
//    }

//    for (int i = 0; i < MAZE_WIDTH; i++) {
//        for (int j = 0; j < MAZE_HEIGHT; j++)   {

//            baseMapNode *mover = moverArray[i-1][j-1];
//            mover->setXY(i,j);  //problem when i call methods

//            if (map[i][j].getWallLeft())   {
//                mover->setWall(LEFT, NULL);
//            }
//            if (map[i][j].getWallTop())  {
//                mover->setWall(TOP, NULL);
//            }
//            if (map[i][j].getWallRight())    {
//                mover->setWall(RIGHT, NULL);
//            }

//            if (map[i][j].getWallBottom()) {
//                mover->setWall(BOTTOM, NULL);
//            }
//        }
//    }

    QFile generatedFile("/Users/Nilay/Desktop/Micromouse/test.maz");
    if(generatedFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        ui->txt_status->append("could not open file");
    }
    QTextStream nigga(&generatedFile);

    for(int i = 0; i < MAZE_WIDTH; i++)
    {
        for(int j = 0; j < MAZE_HEIGHT; j++)
        {


            int top = map[i][j].getWallTop();
            int bottom = map[i][j].getWallBottom();
            int left = map[i][j].getWallLeft();
            int right = map[i][j].getWallRight();

            int x = map[i][j].getX() + 1;
            int y = map[i][j].getY() + 1;

            nigga << x << " " << y << " " << top << " " << bottom << " " << left << " " << right << endl;
        }
    }
    nigga.flush();
    generatedFile.close();

    loadMaze();
    return;
}

void microMouseServer::makeMaze(Cell map[][20], int x, int y, Cell exitCell)    {
    srand(time(NULL));

    int removeLeft = rand()%2;  //randomizing whether the walls should be there or not
    int removeTop = rand()%2;
    int removeRight = rand()%2;
    int removeBottom = rand()%2;

    bool completed = false;

    map[x][y].setVisited(true);

    if (removeLeft == 0 && x > 0)   {   //if the random outcome is to remove the wall and there is a spot to the left
        map[x][y].setWallLeft(false);   //remove the left wall of the current cell
        map[x-1][y].setWallRight(false);    //remove the right wall of the left cell
    }
    if (removeTop == 0 && y < MAZE_HEIGHT -1)   {
        map[x][y].setWallTop(false);
        map[x][y+1].setWallBottom(false);
    }
    if (removeRight == 0 && x < MAZE_WIDTH - 1) {
        map[x][y].setWallRight(false);
        map[x+1][y].setWallLeft(false);
    }
    if (removeBottom == 0 && y > 0) {
        map[x][y].setWallBottom(false);
        map[x-1][y].setWallTop(false);
    }

    if (!map[x][y].getWallLeft()) {   //if theres no wall to my left
        if (map[x][y].getX() > 0) {   //and there is a spot to my left
            if (!map[x-1][y].getVisited())  {   //and that spot hasn't been visited
                map[x][y].addAdjacentUnexploredCell(map[x-1][y]);  //add the spot to my unexplored cells
            }
        }
    }
    if (!map[x][y].getWallTop()) {   //if theres no wall to my top
        if (map[x][y].getY() < MAZE_HEIGHT -1) {   //and there is a spot to my top
            if (!map[x-1][y].getVisited())  {   //and that spot hasn't been visited
                map[x][y].addAdjacentUnexploredCell(map[x][y+1]);  //add the spot to my unexplored cells
            }
        }
    }
    if (!map[x][y].getWallRight()) {   //if theres no wall to my Right
        if (map[x][y].getX() < MAZE_WIDTH -1) {   //and there is a spot to my Right
            if (!map[x+1][y].getVisited())  {   //and that spot hasn't been visited
                map[x][y].addAdjacentUnexploredCell(map[x+1][y]);  //add the spot to my unexplored cells
            }
        }
    }
    if (!map[x][y].getWallBottom()) {   //if theres no wall to my bottom
        if (map[x][y].getY() > 0) {   //and there is a spot to my bottom
            if (!map[x][y-1].getVisited())  {   //and that spot hasn't been visited
                map[x][y].addAdjacentUnexploredCell(map[x][y-1]);  //add the spot to my unexplored cells
            }
        }
    }

    while (!map[x][y].getAdjacentUnexploredCell().empty()) {    //while there are more spots to explore
        int adjacentX = map[x][y].getAdjacentUnexploredCell().first().getX();   //get the xCoordinate of the item to explore
        int adjacentY = map[x][y].getAdjacentUnexploredCell().first().getY();   //as well as the y
        map[x][y].removeFirstAdjacentUnexploredCell();  //remove it because we're going to explore it
        makeMaze(map,adjacentX,adjacentY,exitCell);  //explore it
    }

    while (!completed)  {   //while the entire maze has not been made
        for (int i = 0; i < MAZE_WIDTH; i++)    {   //iterate x
            for (int j = 0; j < MAZE_HEIGHT; j++)   {   //iterate y
                if (!map[i][j].getVisited())    {   //find a place which hasn't been visited yet
                    makeMaze(map,i,j,exitCell);  //explore it
                }
            }
        }

        completed = true;   //set completed to be true because we soon check if it is

        for (int i = 0; i < MAZE_WIDTH; i++)    {   //iterate x
            for (int j = 0; j < MAZE_HEIGHT; j++)   {   //iterate y
                if (!map[i][j].getVisited())    {   //if a place is found which is not visited
                    completed = false;  //then the maze has not been completed
                }
            }
        }
    }

    for (int i = 0; i < MAZE_WIDTH - 1; i++)    {
        for (int j = 0; j < MAZE_HEIGHT; j++)   {
            if (!(map[i][j].getWallTop() && map[i][j+1].getWallRight() && map[i+1][j+1].getWallBottom() && map[i+1][j+1].getWallLeft()) && !(map[x][y].equals(exitCell))) {
                int leftRandom = rand() % 2;
                int topRandom = rand() % 2;
                int rightRandom = rand() % 2;
                int bottomRandom = rand() % 2;

                if (leftRandom == 0)    {
                    map[i][j].setWallTop(true);
                    map[i][j+1].setWallBottom(true);
                }
                if (topRandom == 0)    {
                    map[i][j+1].setWallRight(true);
                    map[i+1][j+1].setWallLeft(true);
                }
                if (rightRandom == 0)    {
                    map[i+1][j+1].setWallBottom(true);
                    map[i+1][j].setWallTop(true);
                }
                if (bottomRandom == 0)    {
                    map[i+1][j].setWallLeft(true);
                    map[i][j].setWallRight(true);
                }

            }
        }
    }

    return;
}

void microMouseServer::saveMaze()
{
    //open file save window
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Select Maze File"), "", tr("Maze Files (*.maz)"));
    QFile inFile(fileName);

    //if file can't be opened throw error to UI
    if (!inFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        ui->txt_debug->append("ERROR 202: file not found");
        return;
    }
    else
    {
        QTextStream mazeFile(&inFile);

        for(int i = 0; i < MAZE_WIDTH; i++)
        {
            for(int j = 0; j < MAZE_HEIGHT; j++)
            {
                int top = this->mazeData[i][j].isWallTop();
                int bottom = this->mazeData[i][j].isWallBottom();
                int left = this->mazeData[i][j].isWallLeft();
                int right = this->mazeData[i][j].isWallRight();

                mazeFile << this->mazeData[i][j].posX() << " " << this->mazeData[i][j].posY() << " " << top << " " << bottom << " " << left << " " << right << endl;
            }
        }

        mazeFile.flush();

        inFile.close();
        ui->txt_debug->append("Maze Saved to File.");
    }
}

void microMouseServer::initMaze()
{
    //zero out data in maze, way slower than memset but safer. Will switch to memset if I have time to test
    for(int y =0; y < MAZE_HEIGHT; y++)
    {
        for(int x = 0; x < MAZE_WIDTH; x++)
        {
            baseMapNode *mover = &this->mazeData[x][y];
            mover->setXY(x+1,y+1);
            if(x == 0)
            {
                mover->setWall(LEFT, NULL);
            }
            else
            {
                mover->setWall(LEFT, &this->mazeData[x-1][y]);
            }
            if(x == MAZE_WIDTH-1)
            {
                mover->setWall(RIGHT, NULL);
            }
            else
            {
                mover->setWall(RIGHT, &this->mazeData[x+1][y]);
            }
            if(y == 0)
            {
                mover->setWall(BOTTOM, NULL);
            }
            else
            {
                mover->setWall(BOTTOM,  &this->mazeData[x][y-1]);
            }
            if(y == MAZE_HEIGHT-1)
            {
                mover->setWall(TOP, NULL);
            }
            else
            {
                mover->setWall(TOP,  &this->mazeData[x][y+1]);
            }
        }
    }
}


//--This is a terrible way of doing this in C++, I will fix in the next revision
void microMouseServer::removeRightWall(QPoint cell)
{
    if(cell.x() < MAZE_WIDTH -1)
    {
        this->mazeData[cell.x()][cell.y()].setWall(RIGHT, &this->mazeData[cell.x()+1][cell.y()]);
        this->mazeData[cell.x()+1][cell.y()].setWall(LEFT, &this->mazeData[cell.x()][cell.y()]);
    }
    this->maze->drawMaze(this->mazeData);
}

void microMouseServer::removeLeftWall(QPoint cell)
{
    if(cell.x() > 0)
    {
        this->mazeData[cell.x()][cell.y()].setWall(LEFT, &this->mazeData[cell.x()-1][cell.y()]);
        this->mazeData[cell.x()-1][cell.y()].setWall(RIGHT, &this->mazeData[cell.x()][cell.y()]);
    }
    this->maze->drawMaze(this->mazeData);
}

void microMouseServer::removeTopWall(QPoint cell)
{
    if(cell.y() < MAZE_HEIGHT -1)
    {
        this->mazeData[cell.x()][cell.y()].setWall(TOP, &this->mazeData[cell.x()][cell.y()+1]);
        this->mazeData[cell.x()][cell.y()+1].setWall(BOTTOM, &this->mazeData[cell.x()][cell.y()]);
    }
    this->maze->drawMaze(this->mazeData);
}

void microMouseServer::removeBottomWall(QPoint cell)
{
    if(cell.y() > 0)
    {
        this->mazeData[cell.x()][cell.y()].setWall(BOTTOM, &this->mazeData[cell.x()][cell.y()-1]);
        this->mazeData[cell.x()][cell.y()-1].setWall(TOP, &this->mazeData[cell.x()][cell.y()]);
    }
    this->maze->drawMaze(this->mazeData);
}

void microMouseServer::addLeftWall(QPoint cell)
{
    this->mazeData[cell.x()][cell.y()].setWall(LEFT, NULL);
    if(cell.x() > 0)this->mazeData[cell.x()-1][cell.y()].setWall(RIGHT,NULL);
    this->maze->drawMaze(this->mazeData);
}

void microMouseServer::addRightWall(QPoint cell)
{
    this->mazeData[cell.x()][cell.y()].setWall(RIGHT, NULL);
    if(cell.x() < MAZE_WIDTH )this->mazeData[cell.x()+1][cell.y()].setWall(LEFT, NULL);
    this->maze->drawMaze(this->mazeData);
}

void microMouseServer::addTopWall(QPoint cell)
{
    this->mazeData[cell.x()][cell.y()].setWall(TOP, NULL);
    if(cell.y() < MAZE_HEIGHT)this->mazeData[cell.x()][cell.y()+1].setWall(BOTTOM,NULL);
    this->maze->drawMaze(this->mazeData);
}

void microMouseServer::addBottomWall(QPoint cell)
{
    this->mazeData[cell.x()][cell.y()].setWall(BOTTOM, NULL);
    if(cell.y() > 0)this->mazeData[cell.x()][cell.y()-1].setWall(TOP,NULL);
    this->maze->drawMaze(this->mazeData);
}
//--up to here

void microMouseServer::startAI()
{
    QPoint newPos;
    newPos.setX(1);
    newPos.setY(1);
    this->maze->drawMouse(newPos, dUP);
    _aiCallTimer->start(MDELAY);
}

void microMouseServer::foundFinish()
{
    _aiCallTimer->stop();
    ui->txt_status->append("Found end of maze.");
}

void microMouseServer::printUI(const QString mesg)
{
    ui->txt_status->append(mesg);
}

bool microMouseServer::isWallForward()
{
    baseMapNode *mover = &this->mazeData[this->maze->mouseX()-1][this->maze->mouseY()-1];
    switch (this->maze->mouseDir()) {
    case dUP:
        return mover->isWallTop();
        break;
    case dDOWN:
        return mover->isWallBottom();
        break;
    case dLEFT:
        return mover->isWallLeft();
        break;
    case dRIGHT:
        return mover->isWallRight();
        break;
    }

    return true;
}
bool microMouseServer::isWallLeft()
{
    baseMapNode *mover = &this->mazeData[this->maze->mouseX()-1][this->maze->mouseY()-1];
    switch (this->maze->mouseDir()) {
    case dUP:
        return mover->isWallLeft();
        break;
    case dDOWN:
        return mover->isWallRight();
        break;
    case dLEFT:
        return mover->isWallBottom();
        break;
    case dRIGHT:
        return mover->isWallTop();
        break;
    }
    return true;
}
bool microMouseServer::isWallRight()
{
    baseMapNode *mover = &this->mazeData[this->maze->mouseX()-1][this->maze->mouseY()-1];
    switch (this->maze->mouseDir()) {
    case dUP:
        return mover->isWallRight();
        break;
    case dDOWN:
        return mover->isWallLeft();
        break;
    case dLEFT:
        return mover->isWallTop();
        break;
    case dRIGHT:
        return mover->isWallBottom();
        break;
    }
    return true;
}
bool microMouseServer::moveForward()
{
    bool hasMoved = false;
    QPoint newPos;
    newPos.setX(this->maze->mouseX());
    newPos.setY(this->maze->mouseY());
    baseMapNode *mover = &this->mazeData[this->maze->mouseX()-1][this->maze->mouseY()-1];
    switch (this->maze->mouseDir()) {
    case dUP:
        if(!mover->isWallTop())
        {
            newPos.setY(newPos.y()+1);
            this->maze->drawMouse(newPos, dUP);
            hasMoved = true;
        }
        break;
    case dDOWN:
        if(!mover->isWallBottom())
        {
            newPos.setY(newPos.y()-1);
            this->maze->drawMouse(newPos, dDOWN);
            hasMoved = true;
        }
        break;
    case dLEFT:
        if(!mover->isWallLeft())
        {
            newPos.setX(newPos.x()-1);
            this->maze->drawMouse(newPos, dLEFT);
            hasMoved = true;
        }
        break;
    case dRIGHT:
        if(!mover->isWallRight())
        {
            newPos.setX(newPos.x()+1);
            this->maze->drawMouse(newPos, dRIGHT);
            hasMoved = true;
        }
        break;
    }

    return hasMoved;
}
void microMouseServer::turnLeft()
{
    QPoint newPos;
    newPos.setX(this->maze->mouseX());
    newPos.setY(this->maze->mouseY());
    switch(this->maze->mouseDir()){
    case dUP:
        this->maze->drawMouse(newPos, dLEFT);
        break;
    case dDOWN:
        this->maze->drawMouse(newPos, dRIGHT);
        break;
    case dLEFT:
        this->maze->drawMouse(newPos, dDOWN);
        break;
    case dRIGHT:
        this->maze->drawMouse(newPos, dUP);
        break;
    }

}
void microMouseServer::turnRight()
{
    QPoint newPos;
    newPos.setX(this->maze->mouseX());
    newPos.setY(this->maze->mouseY());
    switch(this->maze->mouseDir()){
    case dUP:
        this->maze->drawMouse(newPos, dRIGHT);
        break;
    case dDOWN:
        this->maze->drawMouse(newPos, dLEFT);
        break;
    case dLEFT:
        this->maze->drawMouse(newPos, dUP);
        break;
    case dRIGHT:
        this->maze->drawMouse(newPos, dDOWN);
        break;
    }
}


//declaration of variables

int direction = 0;
int xPosition = 0;
int yPosition = 0;

bool win = false;   //checks to see if the mouse has reached the end
bool populated = false;
bool finishedMapping = false;
bool entered = false;

MazeCell myData[MAZE_WIDTH][MAZE_HEIGHT];    //need array here so all functions can have access to it without passing in a parameter

int exitXPosition = 0;
int exitYPosition = 0;

QVector <MazeCell*> * openset = new QVector <MazeCell*>;
QVector <MazeCell*> * closedset = new QVector <MazeCell*>;

QVector <MazeCell*> * shortestDirections = new QVector <MazeCell*>;

bool finishedPossibilities;

/* isWallBack Function
 *
 * NOTE WHEN POGACE SAYS THIS IS TRASH AND YOU DON'T NEED IT,
 * JUSTIFY WITH:
 *
 * 1) amountBottom() shortens from 15 lines to 1 line of code
 * 2) amountBottom() is consistent with the other three amount returning functions
 */

bool microMouseServer::isWallBack() {
    switch (direction % 4)  {
    case 0:
        return positionCurrent().wallBottom;
        break;
    case 1:
        return positionCurrent().wallLeft;
        break;
    case 2:
        return positionCurrent().wallTop;
        break;
    case 3:
        return positionCurrent().wallRight;
        break;
    default:
        return true;
    }
}


//moving functions

void microMouseServer::moveLeft()   {   //if we have a moveForward, why not left right and back
    turnLeft();
    direction = direction + 3;
    realMoveForward();

    return;
}

void microMouseServer::realMoveForward()    {
    moveForward();
    switch (direction % 4)  {
    case 0:
        direction = 0;
        yPosition = yPosition + 1;
        break;
    case 1:
        direction = 1;
        xPosition = xPosition + 1;
        break;
    case 2:
        direction = 2;
        yPosition = yPosition - 1;
        break;
    case 3:
        direction = 3;
        xPosition = xPosition - 1;
        break;
    }
}

void microMouseServer::moveRight()  {
    turnRight();
    direction = direction + 1;
    realMoveForward();

    return;
}

void microMouseServer::moveBack()   {
    turnRight();
    turnRight();
    direction = direction + 2;
    realMoveForward();
    return;
}

//end of moving functions


//start of maze returning functions

MazeCell& microMouseServer::positionCurrent()    {
    return myData[xPosition][yPosition];
}

MazeCell& microMouseServer::positionLeft()   {
    switch (direction % 4)  {
    case 0:
        return myData[xPosition-1][yPosition];
        break;
    case 1:
        return myData[xPosition][yPosition+1];
        break;
    case 2:
        return myData[xPosition+1][yPosition];
        break;
    case 3:
        return myData[xPosition][yPosition-1];
        break;
    default:
        return myData[xPosition][yPosition];
    }
}

MazeCell& microMouseServer::positionTop()    {
    switch (direction % 4)  {
    case 0:
        return myData[xPosition][yPosition+1];
        break;
    case 1:
        return myData[xPosition+1][yPosition];
        break;
    case 2:
        return myData[xPosition][yPosition-1];
        break;
    case 3:
        return myData[xPosition-1][yPosition];
        break;
    default:
        return myData[xPosition][yPosition];
    }
}

MazeCell& microMouseServer::positionRight()  {
    switch (direction % 4)  {
    case 0:
        return myData[xPosition+1][yPosition];
        break;
    case 1:
        return myData[xPosition][yPosition-1];
        break;
    case 2:
        return myData[xPosition-1][yPosition];
        break;
    case 3:
        return myData[xPosition][yPosition+1];
        break;
    default:
        return myData[xPosition][yPosition];
    }
}

MazeCell& microMouseServer::positionBottom() {
    switch (direction % 4)  {
    case 0:
        return myData[xPosition][yPosition-1];
        break;
    case 1:
        return myData[xPosition-1][yPosition];
        break;
    case 2:
        return myData[xPosition][yPosition+1];
        break;
    case 3:
        return myData[xPosition+1][yPosition];
        break;
    default:
        return myData[xPosition][yPosition];
    }
}

//end of maze returning functions


//start of amount returning functions

int microMouseServer::amountLeft()  {
    return isWallLeft() ? -3 : positionLeft().amount;
}

int microMouseServer::amountTop()  {
    return isWallForward() ? -3 : positionTop().amount;
}

int microMouseServer::amountRight()  {
    return isWallRight() ? -3 : positionRight().amount;
}

int microMouseServer::amountBottom()  {
    return isWallBack() ? -3 : positionBottom().amount;
}


//end of amount returning functions

int microMouseServer::numberOfWalls()   {
    int numOfWalls = 0;

    if (amountLeft() < 0)   {
        numOfWalls++;
    }

    if (amountTop() < 0)    {
        numOfWalls++;
    }

    if (amountRight() < 0)  {
        numOfWalls++;
    }

    if (amountBottom() < 0)   {
        numOfWalls++;
    }

    return numOfWalls;
}

void microMouseServer::deadEnd()    {
    if (numberOfWalls() == 3 && !(xPosition == 0 && yPosition == 0))   {
        positionCurrent().amount = -1;
    }
    else if (positionCurrent().amount == -2) {
        positionCurrent().amount = 1;
    }
    else    {
        positionCurrent().amount++;
    }
    return;
}

void microMouseServer::assignAccessible()   {
    if (amountLeft() == -2) {
        positionLeft().amount = 0;
    }
    if (amountTop() == -2)  {
        positionTop().amount = 0;
    }
    if (amountRight() == -2)    {
        positionRight().amount = 0;
    }
    if (amountBottom() == -2)   {
        positionBottom().amount = 0;
    }
    return;
}

void microMouseServer::assignPositions()    {
    for (int i = 0; i < 20; i++)    {
        for (int j = 0; j < 20; j++)    {
            myData[i][j].x = i;
            myData[i][j].y = j;
        }
    }
    return;
}

void microMouseServer::assignWalls() { //assigns the walls based on the direction
    switch (direction % 4)  {
    case 0:
        positionCurrent().wallLeft = isWallLeft();
        positionCurrent().wallTop = isWallForward();
        positionCurrent().wallRight = isWallRight();
        if (xPosition == 0 && yPosition == 0)   {
            positionCurrent().wallBottom = true;
        }
        else    {
            positionCurrent().wallBottom = false;
        }
        break;
    case 1:
        positionCurrent().wallLeft = false;
        positionCurrent().wallTop = isWallLeft();
        positionCurrent().wallRight = isWallForward();
        positionCurrent().wallBottom = isWallRight();
        break;
    case 2:
        positionCurrent().wallLeft = isWallRight();
        positionCurrent().wallTop = false;
        positionCurrent().wallRight = isWallLeft();
        positionCurrent().wallBottom = isWallForward();
        break;
    case 3:
        positionCurrent().wallLeft = isWallForward();
        positionCurrent().wallTop = isWallRight();
        positionCurrent().wallRight = false;
        positionCurrent().wallBottom = isWallLeft();
        break;
    }
}

void microMouseServer::leftHandRule()   {

    int valTop = amountTop();
    int valRight = amountRight();
    int valLeft = amountLeft();

    //1     &&             (2a        ||     2b)       &&             (3a          ||          3b)
    if ((valLeft >= 0) && ((valLeft <= valTop) || (valTop < 0)) && ((valLeft <= valRight) || (valRight < 0)))   {
        moveLeft();
        return;
    }
    else if ((valTop >= 0) && ((valTop <= valRight ) || (valRight < 0)))  {
        realMoveForward();
        return;
    }
    else if (valRight >= 0)  {
        moveRight();
        return;
    }
    else    {
        moveBack();
        return;
    }


}

void microMouseServer::checkExit(int xValue, int yValue)  { //if entry is from (side relative to the user) (which of two boxes entered in from)
    if (!myData[xValue][yValue].wallTop && !myData[xValue][yValue+1].wallRight && !myData[xValue+1][yValue+1].wallBottom && !myData[xValue+1][yValue].wallLeft) {
        win = true;
        exitXPosition = xValue;
        exitYPosition = yValue;
    }

    if (myData[xValue][yValue].amount == 0) {
        populated = false;
    }
}

void microMouseServer::firstRun()   {
    assignPositions();
    assignWalls();
    assignAccessible();
    deadEnd();

    leftHandRule();

    populated = true;
    for (int i = 0; i < 20; i++)    {
        for (int j = 0; j < 20; j++)    {
            checkExit(i,j);
        }
    }
}

bool microMouseServer::isWallLeft(int x, int y) {
    return myData[x][y].wallLeft;
}

bool microMouseServer::isWallForward(int x, int y)  {
    return myData[x][y].wallTop;
}

bool microMouseServer::isWallRight(int x, int y)    {
    return myData[x][y].wallRight;
}

bool microMouseServer::isWallBack(int x, int y) {
    return myData[x][y].wallBottom;
}

bool microMouseServer::isEnd(MazeCell* cell, int x, int y)   {
    if ((x == cell->x) && (y == cell->y))   {
        return true;
    }
    else    {
        return false;
    }
}


MazeCell& microMouseServer::positionCurrent(int x, int y)   {
    return myData[x][y];
}

MazeCell& microMouseServer::positionLeft(int x, int y)  {
    return myData[x-1][y];
}

MazeCell& microMouseServer::positionTop(int x, int y)   {
    return myData[x][y+1];
}

MazeCell& microMouseServer::positionRight(int x, int y) {
    return myData[x+1][y];
}

MazeCell& microMouseServer::positionBottom(int x, int y)    {
    return myData[x][y-1];
}


int microMouseServer::amountLeft(int x, int y)  {
    return isWallLeft(x,y) ? -3 : positionLeft(x,y).amount;
}

int microMouseServer::amountTop(int x, int y)   {
    return isWallForward(x,y) ? -3 : positionTop(x,y).amount;
}

int microMouseServer::amountRight(int x, int y) {
    return isWallRight(x,y) ? -3 : positionRight(x,y).amount;
}

int microMouseServer::amountBottom(int x, int y)    {
    return isWallBack(x,y) ? -3 : positionBottom(x,y).amount;
}


void microMouseServer::resetCosts() {
    for (int i = 0; i < MAZE_WIDTH; i++)    {
        for (int j = 0; j < MAZE_HEIGHT; j++)   {
            myData[i][j].fCost = -10;
            myData[i][j].gCost = -10;
            myData[i][j].hCost = -10;
        }
    }
}

void microMouseServer::setAdjacentCosts(int x, int y)   {

    bool added;

    if (amountLeft(x,y) > 0)    {   //if the position to the left is not a dead end and accessible
        if (positionLeft(x,y).gCost < 0)    {   //if the position has never been assigned a movement cost
            positionLeft(x,y).gCost = positionCurrent(x,y).gCost + 10;  //then make the cost an additional from the place just came
        }
        else    {
            positionLeft(x,y).gCost = std::min(positionCurrent(x,y).gCost+10,positionLeft(x,y).gCost);  //update the cost so it is the lesser one between the previous and the new one
        }
        positionLeft(x,y).hCost = sqrt(pow((x-1)-exitXPosition,2) + pow(y-exitYPosition,2))*10;   //assign the hereuistic cost depending on distance from exit
        positionLeft(x,y).fCost = positionLeft(x,y).gCost + positionLeft(x,y).hCost;    //assign the total cost depending on the movement and hereuistic costs

        added = false;  //added is false because the initial condition is it has not been added to either

        for (int i = 0; i < closedset->size(); i++)  {   //iterate through the closedset
            if (closedset->at(i) == &positionLeft(x,y))   {   //to check if this mazeCell reference has been added already
                added = true;   //if they are, then say that this object reference has already been added
            }
        }

        for (int i = 0; i < openset->size(); i++)    {   //iterate through the openset
            if (openset->at(i) == &positionLeft(x,y)) {   //for the same reason as above
                added = true;   //same as above
            }
        }

        if (!added) {   //if the object reference is not added
            openset->append(&positionLeft(x,y));  //then add it to the openset
        }
    }

    if (amountTop(x,y) > 0) {
        if (positionTop(x,y).gCost < 0)    {
            positionTop(x,y).gCost = positionCurrent(x,y).gCost + 10;
        }
        else    {
            positionTop(x,y).gCost = std::min(positionCurrent(x,y).gCost+10,positionTop(x,y).gCost);
        }
        positionTop(x,y).hCost = sqrt(pow(x-exitXPosition,2) + pow((y+1)-exitYPosition,2))*10;
        positionTop(x,y).fCost = positionTop(x,y).gCost + positionTop(x,y).hCost;

        added = false;

        for (int i = 0; i < closedset->size(); i++)  {
            if (closedset->at(i) == &positionTop(x,y))    {
                added = true;
            }
        }

        for (int i = 0; i < openset->size(); i++)    {
            if (openset->at(i) == &positionTop(x,y))  {
                added = true;
            }
        }

        if (!added) {
            openset->append(&positionTop(x,y));
        }
    }

    if (amountRight(x,y) > 0)    {
        if (positionRight(x,y).gCost < 0)  {
            positionRight(x,y).gCost = positionCurrent(x,y).gCost + 10;
        }
        else    {
            positionRight(x,y).gCost = std::min(positionCurrent(x,y).gCost+10,positionRight(x,y).gCost);
        }
        positionRight(x,y).hCost = sqrt(pow((x+1)-exitXPosition,2) + pow(y-exitYPosition,2))*10;
        positionRight(x,y).fCost = positionRight(x,y).gCost + positionRight(x,y).hCost;

        added = false;

        for (int i = 0; i < closedset->size(); i++)  {
            if (closedset->at(i) == &positionRight(x,y))  {
                added = true;
            }
        }

        for (int i = 0; i < openset->size(); i++)    {
            if (openset->at(i) == &positionRight(x,y))    {
                added = true;
            }
        }

        if (!added) {
            openset->append(&positionRight(x,y));
        }
    }

    if (amountBottom(x,y) > 0) {
        if (positionBottom(x,y).gCost < 0) {
            positionBottom(x,y).gCost = positionCurrent(x,y).gCost + 10;
        }
        else    {
            positionBottom(x,y).gCost = std::min(positionCurrent(x,y).gCost+10,positionBottom(x,y).gCost);
        }
        positionBottom(x,y).hCost = sqrt(pow(x-exitXPosition,2) + pow((y-1)-exitYPosition,2))*10;
        positionBottom(x,y).fCost = positionBottom(x,y).gCost + positionBottom(x,y).hCost;

        added = false;

        for (int i = 0; i < closedset->size(); i++)  {
            if (closedset->at(i) == &positionBottom(x,y)) {
                added = true;
            }
        }

        for (int i = 0; i < openset->size(); i++)    {
            if (openset->at(i) == &positionBottom(x,y))   {
                added = true;
            }
        }

        if (!added) {
            openset->append(&positionBottom(x,y));
        }
    }
    return;

}


void microMouseServer::shellSort(QVector <MazeCell*> * array)  {
    for (int gap = array->size()/2; gap > 0; gap /= 2)   {
        for (int i = gap; i < array->size(); i++)    {
            for (int j=i-gap; j>=0 && &array->at(j)->fCost > &array->at(j+gap)->fCost; j-=gap) {
                MazeCell* temp = array->at(j);
                array->replace(j,array->at(j+gap));
                array->replace(j+gap,temp);

            }
        }
    }
    return;
}

void microMouseServer::insertionSort(QVector <MazeCell*> *array) {
    for (int i = 1; i < array->size(); i++)   {
        int j = i;
        while (j > 0 && array->at(j - 1)->fCost > array->at(j)->fCost) {
            MazeCell *temp = array->at(j);
            array->replace(j,array->at(j-1));
            array->replace(j - 1, temp);
            j--;
        }
    }
}


void microMouseServer::removeChecked()  {
    closedset->append(openset->at(0));
    openset->remove(0);
    return;
}

void microMouseServer::zeroValues(int x, int y)  {
    if ((x == 0) && (y == 0))   {
        positionCurrent(x,y).gCost = 0;
        positionCurrent(x,y).hCost = sqrt(pow(x - exitXPosition,2) + pow(y - exitYPosition,2))*10;
        positionCurrent(x,y).fCost = positionCurrent(x,y).gCost + positionCurrent(x,y).hCost;
        closedset->append(&positionCurrent(x,y));
    }
    return;
}

void microMouseServer::shortestPathDirections(int x, int y) {
    QVector <MazeCell*> * possibilities = new QVector <MazeCell*>;

    for (int i = closedset->last()->gCost; i >= 0; i = i - 10)  {   //if the movement cost is one less than the last checked, then add it to the list of possibilities
        for (int j = 0; j < closedset->size(); j++) {
            if (closedset->at(j)->gCost == i)   {
                possibilities->append(closedset->at(j));
            }
        }

        for (int k = 0; k < possibilities->size(); k++) {   //if the possibility isn't adjacent to the last checked cell, remove it
            if (!shortestDirections->empty())   {
                if (!(shortestDirections->first()->isAdjacent(possibilities->at(k),amountLeft(shortestDirections->first()->x,shortestDirections->first()->y),amountTop(shortestDirections->first()->x,shortestDirections->first()->y),amountRight(shortestDirections->first()->x,shortestDirections->first()->y),amountBottom(shortestDirections->first()->x,shortestDirections->first()->y)))) {
                    possibilities->remove(k);
                    k--;
                }
            }
            else    {
                if (!(isEnd(possibilities->at(k),x,y)))    {   //when the vector is empty, the first value to populate it will be the exit
                    possibilities->remove(k);
                    k--;
                }
            }
        }

        shortestDirections->insert(0,possibilities->at(0));
        shortestDirections->first()->amount = 0;

        possibilities->clear();
    }
    return;
}

void microMouseServer::navigateShortestPath()   {
    positionCurrent().amount++;
    if (amountLeft() == 0)  {
        moveLeft();
    }
    else if (amountTop() == 0)  {
        realMoveForward();
    }
    else if (amountRight() == 0)    {
        moveRight();
    }
    else if (amountBottom() == 0)   {
        moveBack();
    }
    return;
}

bool microMouseServer::atExit() {
    return ((xPosition == exitXPosition) && (yPosition == exitYPosition)) ? true : false;
}


void microMouseServer::shortestPathFinder(int xStart, int yStart, int xEnd, int yEnd) {
    zeroValues(xStart,yStart);
    setAdjacentCosts(xStart,yStart);
    insertionSort(openset);
    removeChecked();
    if (closedset->last() == &myData[xEnd][yEnd]) {
        shortestPathDirections(xEnd,yEnd);
    }
    else    {
        shortestPathFinder(closedset->last()->x,closedset->last()->y,xEnd,yEnd);
    }
    return;
}


void microMouseServer::secondRun()  {

    if (shortestDirections->empty())    {
        shortestPathFinder(0,0,exitXPosition,exitYPosition);
    }
    navigateShortestPath();
    if (atExit())   {
        foundFinish();
    }
    return;
}


void microMouseServer::studentAI()
{
    //Cell map[MAZE_WIDTH][MAZE_HEIGHT];
    //generateMaze();

    if (populated && xPosition == 0 && yPosition == 0)  {
        finishedMapping = true;
    }

    if (!(populated)) {
        firstRun();
    }

    else if (!(finishedMapping))    {

        if (shortestDirections->empty())    {
            shortestPathFinder(xPosition,yPosition,0,0);
        }
        navigateShortestPath();
    }

    else    {
        if (!entered)   {
            openset->clear();
            closedset->clear();
            shortestDirections->clear();

            resetCosts();
            entered = true;
        }
        secondRun();
    }

    return;
}


