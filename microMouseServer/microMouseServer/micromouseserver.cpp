#include "micromouseserver.h"
#include "ui_micromouseserver.h"
#include "mazeConst.h"
#include "mazegui.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include <mazecell.h>


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
    ui->txt_debug->append("Maze loaded");
    mazeFile.flush();
    inFile.close();

    //draw maze and mouse
    this->maze->drawMaze(this->mazeData);
    this->maze->drawMouse(QPoint(1,1),dUP);
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

void microMouseServer::printUI(const char *mesg)
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

mazeCell myData[20][20];    //need array here so all functions can have access to it without passing in a parameter

int exitXPosition;
int exitYPosition;

std::vector<int> listOfPaths;

int direction2 = 0;
int xPosition2 = 0;
int yPosition2 = 0;

bool finishedPossibilities;



void microMouseServer::moveLeft()   {   //if we have a moveForward, why not left right and back
    turnLeft();
    direction = direction + 3;
    realMoveForward();

    return;
}

void microMouseServer::realMoveForward()    {
    moveForward();
    switch (abs(direction % 4))  {
    case 0:
        yPosition = yPosition + 1;
        break;
    case 1:
        xPosition = xPosition + 1;
        break;
    case 2:
        yPosition = yPosition - 1;
        break;
    case 3:
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

//_aiCallTimer->stop();
//ui->txt_status->append("stuff");

void microMouseServer::moveBack()   {
    turnRight();
    turnRight();
    direction = direction + 2;
    realMoveForward();
    return;
}

int microMouseServer::leftAmount()  {
    switch (direction % 4)  {
    case 0:
        return isWallLeft() ? -3 : myData[xPosition-1][yPosition].amount;
        break;
    case 1:
        return isWallLeft() ? -3 : myData[xPosition][yPosition+1].amount;
        break;
    case 2:
        return isWallLeft() ? -3 : myData[xPosition+1][yPosition].amount;
        break;
    case 3:
        return isWallLeft() ? -3 : myData[xPosition][yPosition-1].amount;
        break;
    }
}

int microMouseServer::topAmount()  {
    switch (direction % 4)  {
    case 0:
        return isWallForward() ? -3 : myData[xPosition][yPosition+1].amount;
        break;
    case 1:
        return isWallForward() ? -3 : myData[xPosition+1][yPosition].amount;
        break;
    case 2:
        return isWallForward() ? -3 : myData[xPosition][yPosition-1].amount;
        break;
    case 3:
        return isWallForward() ? -3 : myData[xPosition-1][yPosition].amount;
        break;
    }
}

int microMouseServer::rightAmount()  {
    switch (direction % 4)  {
    case 0:
        return isWallRight() ? -3 : myData[xPosition+1][yPosition].amount;
        break;
    case 1:
        return isWallRight() ? -3 : myData[xPosition][yPosition-1].amount;
        break;
    case 2:
        return isWallRight() ? -3 : myData[xPosition-1][yPosition].amount;
        break;
    case 3:
        return isWallRight() ? -3 : myData[xPosition][yPosition+1].amount;
        break;
    }
}

int microMouseServer::backAmount()  {
    switch (direction % 4)  {
    case 0:
        return myData[xPosition][yPosition].wallBottom ? -3 : myData[xPosition][yPosition-1].amount;
        break;
    case 1:
        return myData[xPosition][yPosition].wallLeft ? -3 : myData[xPosition-1][yPosition].amount;
        break;
    case 2:
        return myData[xPosition][yPosition].wallTop ? -3 : myData[xPosition][yPosition+1].amount;
        break;
    case 3:
        return myData[xPosition][yPosition].wallRight ? -3 : myData[xPosition+1][yPosition].amount;
        break;
    }
}

int microMouseServer::numberOfWalls()   {
    int numOfWalls = 0;

    if (leftAmount() < 0)   {
        numOfWalls++;
    }

    if (topAmount() < 0)    {
        numOfWalls++;
    }

    if (rightAmount() < 0)  {
        numOfWalls++;
    }

    if (backAmount() < 0)   {
        numOfWalls++;
    }

    return numOfWalls;
}

void microMouseServer::deadEnd()    {
    if (numberOfWalls() == 3 && !(xPosition == 0 && yPosition == 0))   {
        myData[xPosition][yPosition].amount = -1;
    }
    else    {
        myData[xPosition][yPosition].amount++;
    }
    return;
}

void microMouseServer::assignAccessible()   {
    switch (direction % 4)  {
    case 0:
        if (leftAmount() == -2) {
            myData[xPosition-1][yPosition].amount = 0;
        }
        if (topAmount() == -2)  {
            myData[xPosition][yPosition+1].amount = 0;
        }
        if (rightAmount() == -2)    {
            myData[xPosition+1][yPosition].amount = 0;
        }
        if (backAmount() == -2) {
            myData[xPosition][yPosition-1].amount = 0;
        }
        if (xPosition == 0 && yPosition == 0 && myData[xPosition][yPosition].amount == -2)  {
            myData[xPosition][yPosition].amount = 0;
        }
        break;
    case 1:
        if (leftAmount() == -2) {
            myData[xPosition][yPosition+1].amount = 0;
        }
        if (topAmount() == -2)  {
            myData[xPosition+1][yPosition].amount = 0;
        }
        if (rightAmount() == -2)    {
            myData[xPosition][yPosition-1].amount = 0;
        }
        if (backAmount() == -2) {
            myData[xPosition-1][yPosition].amount == 0;
        }
        break;
    case 2:
        if (leftAmount() == -2) {
            myData[xPosition+1][yPosition].amount = 0;
        }
        if (topAmount() == -2)  {
            myData[xPosition][yPosition-1].amount = 0;
        }
        if (rightAmount() == -2)    {
            myData[xPosition-1][yPosition].amount = 0;
        }
        if (backAmount() == -2) {
            myData[xPosition][yPosition+1].amount = 0;
        }
        break;
    case 3:
        if (leftAmount() == -2) {
            myData[xPosition][yPosition-1].amount = 0;
        }
        if (topAmount() == -2)  {
            myData[xPosition-1][yPosition].amount = 0;
        }
        if (rightAmount() == -2)    {
            myData[xPosition][yPosition+1].amount = 0;
        }
        if (backAmount() == -2) {
            myData[xPosition+1][yPosition].amount = 0;
        }
        break;
    }
    return;
}

void microMouseServer::assignWalls() { //assigns the walls based on the direction
    switch (direction % 4)  {
    case 0:
        myData[xPosition][yPosition].wallLeft = isWallLeft();
        myData[xPosition][yPosition].wallTop = isWallForward();
        myData[xPosition][yPosition].wallRight = isWallRight();
        if (xPosition == 0 && yPosition == 0)   {
            myData[xPosition][yPosition].wallBottom = true;
        }
        else    {
            myData[xPosition][yPosition].wallBottom = false;
        }
        break;
    case 1:
        myData[xPosition][yPosition].wallLeft = false;
        myData[xPosition][yPosition].wallTop = isWallLeft();
        myData[xPosition][yPosition].wallRight = isWallForward();
        myData[xPosition][yPosition].wallBottom = isWallRight();
        break;
    case 2:
        myData[xPosition][yPosition].wallLeft = isWallRight();
        myData[xPosition][yPosition].wallTop = false;
        myData[xPosition][yPosition].wallRight = isWallLeft();
        myData[xPosition][yPosition].wallBottom = isWallForward();
        break;
    case 3:
        myData[xPosition][yPosition].wallLeft = isWallForward();
        myData[xPosition][yPosition].wallTop = isWallRight();
        myData[xPosition][yPosition].wallRight = false;
        myData[xPosition][yPosition].wallBottom = isWallLeft();
        break;
    }
    /*
    if (direction % 4 == 0)  {  //checks to see if the mouse is facing forward
        myData[xPosition][yPosition].wallLeft = isWallLeft();
        myData[xPosition][yPosition].wallTop = isWallForward();
        myData[xPosition][yPosition].wallRight = isWallRight();
        if (xPosition == 0 && yPosition == 0)   {
            myData[xPosition][yPosition].wallBottom = true;
        }
        else    {
            myData[xPosition][yPosition].wallBottom = false;
        }
    }
    else if ((direction - 1) % 4 == 0)   { //right
        myData[xPosition][yPosition].wallLeft = false;
        myData[xPosition][yPosition].wallTop = isWallLeft();
        myData[xPosition][yPosition].wallRight = isWallForward();
        myData[xPosition][yPosition].wallBottom = isWallRight();
    }
    else if ((direction - 2) % 4 == 0)   { //backwards
        myData[xPosition][yPosition].wallLeft = isWallRight();
        myData[xPosition][yPosition].wallTop = false;
        myData[xPosition][yPosition].wallRight = isWallLeft();
        myData[xPosition][yPosition].wallBottom = isWallForward();
    }
    else if ((direction + 1) % 4 == 0)   { //left
        myData[xPosition][yPosition].wallLeft = isWallForward();
        myData[xPosition][yPosition].wallTop = isWallRight();
        myData[xPosition][yPosition].wallRight = false;
        myData[xPosition][yPosition].wallBottom = isWallLeft();
    }
    */
/*
    if (!myData[xPosition][yPosition].wallLeft && myData[xPosition-1][yPosition].amount == -2) {
        myData[xPosition-1][yPosition].amount = 0;
    }

    if (!myData[xPosition][yPosition].wallTop && myData[xPosition][yPosition+1].amount == -2) {
        myData[xPosition][yPosition+1].amount = 0;
    }

    if (!myData[xPosition][yPosition].wallRight && myData[xPosition+1][yPosition].amount == -2) {
        myData[xPosition+1][yPosition].amount = 0;
    }

    if (!myData[xPosition][yPosition].wallBottom && myData[xPosition][yPosition-1].amount == -2) {
        myData[xPosition][yPosition-1].amount = 0;
    }
    /*
/*
    if (xPosition == 0 && yPosition == 0)   {
        if (myData[xPosition][yPosition].amount == -2)  {
            myData[xPosition][yPosition].amount = 1;
        }
        else    {
            myData[xPosition][yPosition].amount = myData[xPosition][yPosition].amount + 1;
        }
    }
    else if (isWallLeft() && isWallForward() && isWallRight())   {  //checking to see if this is the dead end
        myData[xPosition][yPosition].amount = -1;
    }
    else if (((isWallForward() && isWallLeft()) || (isWallForward() && isWallRight()) || (isWallLeft() && isWallRight())) && (direction % 4 == 0) && (myData[xPosition][yPosition-1].amount == -1))    {
        myData[xPosition][yPosition].amount = -1;   //checks to see if this is the path leading to a dead end when facing forward
    }
    else if (((isWallForward() && isWallLeft()) || (isWallForward() && isWallRight()) || (isWallLeft() && isWallRight())) && ((direction - 1) % 4 == 0) && (myData[xPosition-1][yPosition].amount == -1))    {
        myData[xPosition][yPosition].amount = -1;   //right
    }
    else if (((isWallForward() && isWallLeft()) || (isWallForward() && isWallRight()) || (isWallLeft() && isWallRight())) && ((direction - 2) % 4 == 0) && (myData[xPosition][yPosition+1].amount == -1))    {
        myData[xPosition][yPosition].amount = -1;   //bottom
    }
    else if (((isWallForward() && isWallLeft()) || (isWallForward() && isWallRight()) || (isWallLeft() && isWallRight())) && ((direction + 1) % 4 == 0) && (myData[xPosition+1][yPosition].amount == -1))    {
        myData[xPosition][yPosition].amount = -1;   //left
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && (((myData[xPosition-1][yPosition].amount == -1) && (myData[xPosition][yPosition+1].amount == -1) && (myData[xPosition+1][yPosition].amount == -1) && (myData[xPosition][yPosition-1].amount != -1)) || ((myData[xPosition-1][yPosition].amount == -1) && (myData[xPosition][yPosition+1].amount == -1) && (myData[xPosition+1][yPosition].amount != -1) && (myData[xPosition][yPosition-1].amount == -1)) || ((myData[xPosition-1][yPosition].amount == -1) && (myData[xPosition][yPosition+1].amount != -1) && (myData[xPosition+1][yPosition].amount == -1) && (myData[xPosition][yPosition-1].amount == -1)) || ((myData[xPosition-1][yPosition].amount != -1) && (myData[xPosition][yPosition+1].amount == -1) && (myData[xPosition+1][yPosition].amount == -1) && (myData[xPosition][yPosition-1].amount == -1))))  {
        myData[xPosition][yPosition].amount = -1;   //checking to see if an all of the intersections paths except one lead to dead ends
    }
    else if ((myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((myData[xPosition][yPosition+1].amount == -1 && myData[xPosition+1][yPosition].amount == -1 && myData[xPosition][yPosition-1].amount != -1) || (myData[xPosition][yPosition+1].amount == -1 && myData[xPosition+1][yPosition].amount != -1 && myData[xPosition][yPosition-1].amount == -1) || (myData[xPosition][yPosition+1].amount != -1 && myData[xPosition+1][yPosition].amount == -1 && myData[xPosition][yPosition-1].amount == -1))) {
        myData[xPosition][yPosition].amount = -1;   //checking to see if all the routes of a t-junction with no wall on the top, right and bottom except one lead to dead ends
    }
    else if ((!myData[xPosition][yPosition].wallLeft && myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((myData[xPosition-1][yPosition].amount == -1 && myData[xPosition][yPosition-1].amount == -1 && myData[xPosition+1][yPosition].amount != -1) || (myData[xPosition-1][yPosition].amount == -1 && myData[xPosition][yPosition-1].amount != -1 && myData[xPosition+1][yPosition].amount == -1) || (myData[xPosition-1][yPosition].amount != -1 && myData[xPosition][yPosition-1].amount == -1 && myData[xPosition+1][yPosition].amount == -1))) {
        myData[xPosition][yPosition].amount = -1;   //walls on left, down, right
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((myData[xPosition][yPosition+1].amount == -1 && myData[xPosition-1][yPosition].amount == -1 && myData[xPosition][yPosition-1].amount != -1) || (myData[xPosition][yPosition+1].amount == -1 && myData[xPosition-1][yPosition].amount != -1 && myData[xPosition][yPosition-1].amount == -1) || (myData[xPosition][yPosition+1].amount != -1 && myData[xPosition-1][yPosition].amount == -1 && myData[xPosition][yPosition-1].amount == -1))) {
        myData[xPosition][yPosition].amount = -1;   //walls on top, left, bottom
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && myData[xPosition][yPosition].wallBottom) && ((myData[xPosition-1][yPosition].amount == -1 && myData[xPosition][yPosition+1].amount == -1 && myData[xPosition+1][yPosition].amount != -1) || (myData[xPosition-1][yPosition].amount == -1 && myData[xPosition][yPosition+1].amount != -1 && myData[xPosition+1][yPosition].amount == -1) || (myData[xPosition-1][yPosition].amount != -1 && myData[xPosition][yPosition+1].amount == -1 && myData[xPosition+1][yPosition].amount == -1))) {
        myData[xPosition][yPosition].amount = -1;   //walls on left, top, right
    }
    else    {
        myData[xPosition][yPosition].amount = myData[xPosition][yPosition].amount + 1;
    }   */
}

void microMouseServer::leftHandRule()   {

    int valTop = topAmount();
    int valRight = rightAmount();
    int valLeft = leftAmount();

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

  /*  if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && (direction % 4 == 0) && ((myData[xPosition][yPosition+1].amount < myData[xPosition-1][yPosition].amount)))  {
        yPosition = yPosition + 1;  //checks to see if at intersection with facing top and if the left path has been travelled but the top path has not
        moveForward();
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction - 1) % 4 == 0) && ((myData[xPosition+1][yPosition].amount < myData[xPosition][yPosition+1].amount)))   {
        xPosition = xPosition + 1;  //facing right
        moveForward();
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction - 2) % 4 == 0) && ((myData[xPosition][yPosition-1].amount < myData[xPosition+1][yPosition].amount)))   {
        yPosition = yPosition - 1;  //facing backwards
        moveForward();
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction + 1) % 4 == 0) && ((myData[xPosition-1][yPosition].amount < myData[xPosition][yPosition-1].amount)))   {
        xPosition = xPosition - 1;  //facing left
        moveForward();
    }

    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && (direction % 4 == 0) && ((myData[xPosition+1][yPosition].amount < myData[xPosition-1][yPosition].amount) && (myData[xPosition+1][yPosition].amount < myData[xPosition][yPosition+1].amount)))  {
        yPosition = yPosition + 1;  //checks to see if at intersection with facing top and if the left path and top path have been travelled but the right path has not
        moveForward();
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction - 1) % 4 == 0) && ((myData[xPosition][yPosition-1].amount < myData[xPosition][yPosition+1].amount) && (myData[xPosition][yPosition-1].amount < myData[xPosition+1][yPosition].amount)))    {
        xPosition = xPosition + 1;  //facing right
        moveForward();
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction - 2) % 4 == 0) && ((myData[xPosition-1][yPosition].amount < myData[xPosition+1][yPosition].amount) && (myData[xPosition-1][yPosition].amount < myData[xPosition][yPosition-1].amount)))    {
        yPosition = yPosition - 1;  //facing backwards
        moveForward();
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction + 1) % 4 == 0) && ((myData[xPosition][yPosition+1].amount < myData[xPosition][yPosition-1].amount) && (myData[xPosition][yPosition+1].amount < myData[xPosition-1][yPosition].amount)))    {
        xPosition = xPosition - 1;  //facing left
        moveForward();
    }

    else if ((myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && (direction % 4 == 0) && ((myData[xPosition+1][yPosition].amount < myData[xPosition][yPosition+1].amount)))  {
        xPosition = xPosition + 1;  //checks to see if at a t-junction with facing top and if the forward path has been travelled but the right has not
        moveRight();
        direction = direction + 1;
    }
    else if ((!myData[xPosition][yPosition].wallLeft && myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction - 1) % 4 == 0) && ((myData[xPosition][yPosition-1].amount < myData[xPosition+1][yPosition].amount)))    {
        yPosition = yPosition - 1;  //facing right
        moveRight();
        direction = direction + 1;
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction - 2) % 4 == 0) && ((myData[xPosition-1][yPosition].amount < myData[xPosition][yPosition-1].amount)))    {
        xPosition = xPosition - 1;  //facing backwards
        moveRight();
        direction = direction + 1;
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && myData[xPosition][yPosition].wallBottom) && ((direction + 1) % 4 == 0) && ((myData[xPosition][yPosition+1].amount < myData[xPosition-1][yPosition].amount)))   {
        yPosition = yPosition + 1;  //facing left
        moveRight();
        direction = direction + 1;
    }

    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction) % 4 == 0) && ((myData[xPosition][yPosition+1].amount < myData[xPosition-1][yPosition].amount)))   {
        yPosition = yPosition + 1;  //t-junction w/facing top with left path travelled but not straight path
        moveForward();
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && myData[xPosition][yPosition].wallBottom) && ((direction - 1) % 4 == 0) && ((myData[xPosition+1][yPosition].amount < myData[xPosition][yPosition+1].amount)))    {
        xPosition = xPosition + 1;  //facing right
        moveForward();
    }
    else if ((myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction - 1) % 4 == 0) && ((myData[xPosition][yPosition-1].amount < myData[xPosition+1][yPosition].amount)))   {
        yPosition = yPosition - 1;  //facing backwards
        moveForward();
    }
    else if ((!myData[xPosition][yPosition].wallLeft && myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction + 1) % 4 == 0) && ((myData[xPosition-1][yPosition].amount < myData[xPosition][yPosition-1].amount)))   {
        xPosition = xPosition - 1;  //facing left
        moveForward();
    }

    else if ((!myData[xPosition][yPosition].wallLeft && myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction) % 4 == 0) && ((myData[xPosition+1][yPosition].amount < myData[xPosition-1][yPosition].amount)))   {
        xPosition = xPosition + 1;  //t-junction w/facing top with left path travelled but not right path
        moveRight();
        direction = direction + 1;
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction - 1) % 4 == 0) && ((myData[xPosition][yPosition-1].amount < myData[xPosition][yPosition+1].amount)))   {
        yPosition = yPosition - 1;  //facing right
        moveRight();
        direction = direction + 1;
    }
    else if ((!myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && myData[xPosition][yPosition].wallBottom) && ((direction - 2) % 4 == 0) && ((myData[xPosition-1][yPosition].amount < myData[xPosition+1][yPosition].amount)))   {
        xPosition = xPosition - 1;  //facing bottom
        moveRight();
        direction = direction + 1;
    }
    else if ((myData[xPosition][yPosition].wallLeft && !myData[xPosition][yPosition].wallTop && !myData[xPosition][yPosition].wallRight && !myData[xPosition][yPosition].wallBottom) && ((direction + 1) % 4 == 0) && ((myData[xPosition][yPosition+1].amount < myData[xPosition][yPosition-1].amount)))   {
        yPosition = yPosition + 1;  //facing left
        moveRight();
        direction = direction + 1;
    }

    else if (!isWallLeft() && direction % 4 == 0 && myData[xPosition-1][yPosition].amount != -1) {
        xPosition = xPosition - 1;
        moveLeft();
        direction = direction - 1;
    }
    else if (!isWallLeft() && (direction - 1) % 4 == 0 && myData[xPosition][yPosition+1].amount != -1)  {
        yPosition = yPosition + 1;
        moveLeft();
        direction = direction - 1;
    }
    else if (!isWallLeft() && (direction + 2) % 4 == 0 && myData[xPosition+1][yPosition].amount != -1)  {
        xPosition = xPosition + 1;
        moveLeft();
        direction = direction - 1;
    }
    else if (!isWallLeft() && (direction + 1) % 4 == 0 && myData[xPosition][yPosition-1].amount != -1)  {
        yPosition = yPosition - 1;
        moveLeft();
        direction = direction - 1;
    }
    else if (!isWallForward() && direction % 4 == 0 && myData[xPosition][yPosition+1].amount != -1) {
        yPosition = yPosition + 1;
        moveForward();
    }
    else if (!isWallForward() && (direction - 1) % 4 == 0 && myData[xPosition+1][yPosition].amount != -1)  {
        xPosition = xPosition + 1;
        moveForward();
    }
    else if (!isWallForward() && (direction + 2) % 4 == 0 && myData[xPosition][yPosition-1].amount != -1)  {
        yPosition = yPosition - 1;
        moveForward();
    }
    else if (!isWallForward() && (direction + 1) % 4 == 0 && myData[xPosition-1][yPosition].amount != -1)  {
        xPosition = xPosition - 1;
        moveForward();
    }
    else if (!isWallRight() && direction % 4 == 0 && myData[xPosition+1][yPosition].amount != -1) {
        xPosition = xPosition + 1;
        moveRight();
        direction = direction + 1;
    }
    else if (!isWallRight() && (direction - 1) % 4 == 0 && myData[xPosition][yPosition-1].amount != -1)  {
        yPosition = yPosition - 1;
        moveRight();
        direction = direction + 1;
    }
    else if (!isWallRight() && (direction + 2) % 4 == 0 && myData[xPosition-1][yPosition].amount != -1)  {
        xPosition = xPosition - 1;
        moveRight();
        direction = direction + 1;
    }
    else if (!isWallRight() && (direction + 1) % 4 == 0 && myData[xPosition][yPosition+1].amount != -1)  {
        yPosition = yPosition + 1;
        moveRight();
        direction = direction + 1;
    }
    else if (direction % 4 == 0 && myData[xPosition][yPosition-1].amount != -1) {
        yPosition = yPosition - 1;
        moveBack();
        direction = direction - 2;
    }
    else if ((direction - 1) % 4 == 0 && myData[xPosition-1][yPosition].amount != -1)  {
        xPosition = xPosition - 1;
        moveBack();
        direction = direction - 2;
    }
    else if ((direction + 2) % 4 == 0 && myData[xPosition][yPosition+1].amount != -1)  {
        yPosition = yPosition + 1;
        moveBack();
        direction = direction - 2;
    }
    else if ((direction + 1) % 4 == 0 && myData[xPosition+1][yPosition].amount != -1)  {
        xPosition = xPosition + 1;
        moveBack();
        direction = direction - 2;
    }   */
}

void microMouseServer::checkExit(int xValue, int yValue)  { //if entry is from (side relative to the user) (which of two boxes entered in from)
    if (myData[xValue][yValue].wallLeft && !myData[xValue][yValue].wallTop && !myData[xValue][yValue].wallRight && !myData[xValue][yValue].wallBottom&& myData[xValue][yValue+1].wallLeft && myData[xValue][yValue+1].wallTop && !myData[xValue][yValue+1].wallRight && !myData[xValue][yValue+1].wallBottom&& !myData[xValue+1][yValue+1].wallLeft && myData[xValue+1][yValue+1].wallTop && myData[xValue+1][yValue+1].wallRight && !myData[xValue+1][yValue+1].wallBottom&& !myData[xValue+1][yValue].wallLeft && !myData[xValue+1][yValue].wallTop && myData[xValue+1][yValue].wallRight && myData[xValue+1][yValue].wallBottom)    {
        win = true; //if entry is from bottom left
        exitXPosition = xValue;
        exitYPosition = yValue;
    }
    else if (!myData[xValue][yValue].wallLeft && myData[xValue][yValue].wallTop && !myData[xValue][yValue].wallRight && !myData[xValue][yValue].wallBottom&& !myData[xValue+1][yValue].wallLeft && myData[xValue+1][yValue].wallTop && myData[xValue+1][yValue].wallRight && !myData[xValue+1][yValue].wallBottom&& !myData[xValue+1][yValue-1].wallLeft && !myData[xValue+1][yValue-1].wallTop && myData[xValue+1][yValue-1].wallRight && myData[xValue+1][yValue-1].wallBottom&& myData[xValue][yValue-1].wallLeft && !myData[xValue][yValue-1].wallTop && !myData[xValue][yValue-1].wallRight && myData[xValue][yValue-1].wallBottom)    {
        win = true; //if entry is from left top
        exitXPosition = xValue;
        exitYPosition = yValue;
    }
    else if (!myData[xValue][yValue].wallLeft && !myData[xValue][yValue].wallTop && myData[xValue][yValue].wallRight && !myData[xValue][yValue].wallBottom&& !myData[xValue][yValue-1].wallLeft && !myData[xValue][yValue-1].wallTop && myData[xValue][yValue-1].wallRight && myData[xValue][yValue-1].wallBottom&& myData[xValue-1][yValue-1].wallLeft && !myData[xValue-1][yValue-1].wallTop && !myData[xValue-1][yValue-1].wallRight && myData[xValue-1][yValue-1].wallBottom&& myData[xValue-1][yValue].wallLeft && myData[xValue-1][yValue].wallTop && !myData[xValue-1][yValue].wallRight && !myData[xValue-1][yValue].wallBottom) {
        win = true; //if entry is from top right
        exitXPosition = xValue;
        exitYPosition = yValue;
    }
    else if (!myData[xValue][yValue].wallLeft && !myData[xValue][yValue].wallTop && !myData[xValue][yValue].wallRight && myData[xValue][yValue].wallBottom&& myData[xValue-1][yValue].wallLeft && !myData[xValue-1][yValue].wallTop && !myData[xValue-1][yValue].wallRight && myData[xValue-1][yValue].wallBottom&& myData[xValue-1][yValue+1].wallLeft && myData[xValue-1][yValue+1].wallTop && !myData[xValue-1][yValue+1].wallRight && !myData[xValue-1][yValue+1].wallBottom&& !myData[xValue][yValue+1].wallLeft && myData[xValue][yValue+1].wallTop && myData[xValue][yValue+1].wallRight && !myData[xValue][yValue+1].wallBottom)    {
        win = true; //if entry is from right bottom
        exitXPosition = xValue;
        exitYPosition = yValue;
    }
    else if (!myData[xValue][yValue].wallLeft && !myData[xValue][yValue].wallTop && myData[xValue][yValue].wallRight && !myData[xValue][yValue].wallBottom&& !myData[xValue][yValue+1].wallLeft && myData[xValue][yValue+1].wallTop && myData[xValue][yValue+1].wallRight && !myData[xValue][yValue+1].wallBottom&& myData[xValue-1][yValue+1].wallLeft && myData[xValue-1][yValue+1].wallTop && !myData[xValue-1][yValue+1].wallRight && !myData[xValue-1][yValue+1].wallBottom&& myData[xValue-1][yValue].wallLeft && !myData[xValue-1][yValue].wallTop && !myData[xValue-1][yValue].wallRight && myData[xValue-1][yValue].wallBottom) {
        win = true; //if entry is from bottom right
        exitXPosition = xValue;
        exitYPosition = yValue;
    }
    else if (!myData[xValue][yValue].wallLeft && !myData[xValue][yValue].wallTop && !myData[xValue][yValue].wallRight && myData[xValue][yValue].wallBottom&& !myData[xValue+1][yValue].wallLeft && !myData[xValue+1][yValue].wallTop && myData[xValue+1][yValue].wallRight && myData[xValue+1][yValue].wallBottom&& !myData[xValue+1][yValue+1].wallLeft && myData[xValue+1][yValue+1].wallTop && myData[xValue+1][yValue+1].wallRight && !myData[xValue+1][yValue+1].wallBottom&& myData[xValue][yValue+1].wallLeft && myData[xValue][yValue+1].wallTop && !myData[xValue][yValue+1].wallRight && !myData[xValue][yValue+1].wallBottom)    {
        win = true; //if entry is from left bottom
        exitXPosition = xValue;
        exitYPosition = yValue;
    }
    else if (myData[xValue][yValue].wallLeft && !myData[xValue][yValue].wallTop && !myData[xValue][yValue].wallRight && !myData[xValue][yValue].wallBottom&& myData[xValue][yValue-1].wallLeft && !myData[xValue][yValue-1].wallTop && !myData[xValue][yValue-1].wallRight && myData[xValue][yValue-1].wallBottom&& !myData[xValue+1][yValue-1].wallLeft && !myData[xValue+1][yValue-1].wallTop && myData[xValue+1][yValue-1].wallRight && myData[xValue+1][yValue-1].wallBottom&& !myData[xValue+1][yValue].wallLeft && myData[xValue+1][yValue].wallTop && myData[xValue+1][yValue].wallRight && !myData[xValue+1][yValue].wallBottom) {
        win = true; //if entry is from top left
        exitXPosition = xValue;
        exitYPosition = yValue;
    }
    else if (!myData[xValue][yValue].wallLeft && myData[xValue][yValue].wallTop && !myData[xValue][yValue].wallRight && !myData[xValue][yValue].wallBottom&& myData[xValue-1][yValue].wallLeft && myData[xValue-1][yValue].wallTop && !myData[xValue-1][yValue].wallRight && !myData[xValue-1][yValue].wallBottom&& myData[xValue-1][yValue-1].wallLeft && !myData[xValue-1][yValue-1].wallTop && !myData[xValue-1][yValue-1].wallRight && myData[xValue-1][yValue-1].wallBottom&& !myData[xValue][yValue-1].wallLeft && !myData[xValue][yValue-1].wallTop && myData[xValue][yValue-1].wallRight && myData[xValue][yValue-1].wallBottom)    {
        win = true; //if entry is from right top
        exitXPosition = xValue;
        exitYPosition = yValue;
    }

    if (myData[xValue][yValue].amount == 0) {
        populated = false;
    }
}

void microMouseServer::firstRun()   {
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

/*

void microMouseServer::shortPathFinder()    {

    while (!finishedPossibilities)  {

        if (!myData[xPosition2][yPosition2].wallLeft && !myData[xPosition2][yPosition2].wallTop && !!myData[xPosition2][yPosition2].wallRight && direction == 0)    {
            listOfPaths.insert(listOfPaths.size(),0);
        }

        //checking left when facing
        if (!myData[xPosition2][yPosition2].wallLeft && direction2 % 4 == 0 && myData[xPosition2-1][yPosition2].amount != -1) {
            xPosition2 = xPosition2 - 1;    //top
            direction2 = direction2 - 1;
        }
        else if (!myData[xPosition2][yPosition2].wallTop && (direction2 - 1) % 4 == 0 && myData[xPosition2][yPosition2+1].amount != -1)  {
            yPosition2 = yPosition2 + 1;    //right
            direction2 = direction2 - 1;
        }
        else if (!myData[xPosition2][yPosition2].wallRight && (direction2 + 2) % 4 == 0 && myData[xPosition2+1][yPosition2].amount != -1)  {
            xPosition2 = xPosition2 + 1;    //bottom
            direction2 = direction2 - 1;
        }
        else if (!myData[xPosition2][yPosition2].wallBottom && (direction2 + 1) % 4 == 0 && myData[xPosition2][yPosition2-1].amount != -1)  {
            yPosition2 = yPosition2 - 1;    //left
            direction2 = direction2 - 1;
        }

        //checking top when facing
        else if (!myData[xPosition2][yPosition2].wallTop && direction2 % 4 == 0 && myData[xPosition2][yPosition2+1].amount != -1) {
            yPosition2 = yPosition2 + 1;    //top
        }
        else if (!myData[xPosition2][yPosition2].wallRight && (direction2 - 1) % 4 == 0 && myData[xPosition2+1][yPosition2].amount != -1)  {
            xPosition2 = xPosition2 + 1;    //right
        }
        else if (!myData[xPosition2][yPosition2].wallBottom && (direction2 + 2) % 4 == 0 && myData[xPosition2][yPosition2-1].amount != -1)  {
            yPosition2 = yPosition2 - 1;    //bottom
        }
        else if (!myData[xPosition2][yPosition2].wallLeft && (direction2 + 1) % 4 == 0 && myData[xPosition2-1][yPosition2].amount != -1)  {
            xPosition2 = xPosition2 - 1;    //left
        }

        //checking right when facing
        else if (!myData[xPosition2][yPosition2].wallRight && direction2 % 4 == 0 && myData[xPosition2+1][yPosition2].amount != -1) {
            xPosition2 = xPosition2 + 1;    //top
            direction2 = direction2 + 1;
        }
        else if (!myData[xPosition2][yPosition2].wallBottom && (direction2 - 1) % 4 == 0 && myData[xPosition2][yPosition2-1].amount != -1)  {
            yPosition2 = yPosition2 - 1;    //right
            direction2 = direction2 + 1;
        }
        else if (!myData[xPosition2][yPosition2].wallLeft && (direction2 + 2) % 4 == 0 && myData[xPosition2-1][yPosition2].amount != -1)  {
            xPosition2 = xPosition2 - 1;    //bottom
            direction2 = direction2 + 1;
        }
        else if (!myData[xPosition2][yPosition2].wallTop && (direction2 + 1) % 4 == 0 && myData[xPosition2][yPosition2+1].amount != -1)  {
            yPosition2 = yPosition2 + 1;    //left
            direction2 = direction2 + 1;
        }

        //checking bottom when facing
        else if (!myData[xPosition2][yPosition2].wallBottom && direction2 % 4 == 0 && myData[xPosition2][yPosition2-1].amount != -1) {
            yPosition2 = yPosition2 - 1;    //top
            direction2 = direction2 - 2;
        }
        else if (!myData[xPosition2][yPosition2].wallLeft && (direction2 - 1) % 4 == 0 && myData[xPosition2-1][yPosition2].amount != -1)  {
            xPosition2 = xPosition2 - 1;    //right
            direction2 = direction2 - 2;
        }
        else if (!myData[xPosition2][yPosition2].wallTop && (direction2 + 2) % 4 == 0 && myData[xPosition2][yPosition2+1].amount != -1)  {
            yPosition2 = yPosition2 + 1;    //bottom
            direction2 = direction2 - 2;
        }
        else if (!myData[xPosition2][yPosition2].wallRight && (direction2 + 1) % 4 == 0 && myData[xPosition2+1][yPosition2].amount != -1)  {
            xPosition2 = xPosition2 + 1;    //left
            direction2 = direction2 - 2;
        }



        finishedPossibilities == true;
        for (int k = 0; k < listOfPaths.size(); i++)    {
            if (listOfPaths.at(k) != 2) {
                finishedPossibilities = false;
            }
        }
    }
}

*/

void microMouseServer::studentAI()
{

    if (!populated) {
        firstRun();
    }
   /* else    {
        ui->txt_status->append("I have found the exit");
        ui->txt_status->append("It is located at (" + QString::number(exitXPosition) + "," + QString::number(exitYPosition) + ")");
    } */

    if (populated && xPosition == 0 && yPosition == 0)   {
        ui->txt_status->append("I have found the exit");
        ui->txt_status->append("It is located at (" + QString::number(exitXPosition) + "," + QString::number(exitYPosition) + ")");
    }

   /* if (win == true)    {
        ui->txt_status->append("I have found the exit");
        ui->txt_status->append("It is located at (" + QString::number(exitXPosition) + "," + QString::number(exitYPosition) + ")");
    }

    if (myData[exitXPosition][exitYPosition].amount == 3)   {
        _aiCallTimer->stop();
    }*/
}
