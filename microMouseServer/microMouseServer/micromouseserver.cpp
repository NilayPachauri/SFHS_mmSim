#include "micromouseserver.h"
#include "ui_micromouseserver.h"
#include "mazeConst.h"
#include "mazegui.h"
#include "mazecell.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>


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
            mover->setWall(LEFT, &this->mazeData[x-1][y]);
        }

        if(wallRight)
        {
            mover->setWall(RIGHT, NULL);
        }
        else
        {
            mover->setWall(RIGHT, &this->mazeData[x+1][y]);
        }
        if(wallTop)
        {
            mover->setWall(TOP, NULL);
        }
        else
        {
            mover->setWall(TOP, &this->mazeData[x][y+1]);
        }

        if(wallBottom)
        {
            mover->setWall(BOTTOM, NULL);
        }
        else
        {
            mover->setWall(BOTTOM, &this->mazeData[x][y-1]);
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

int direction = 0;
int xPosition = 0;
int yPosition = 0;

bool win = false;   //checks to see if the mouse has reached the end
bool populated;

mazeCell myData[20][20];    //need array here so all functions can have access to it without passing in a parameter

int exitXPosition;
int exitYPosition;


void microMouseServer::moveLeft()   {   //if we have a moveForward, why not left right and back
    turnLeft();
    moveForward();
    return;
}

void microMouseServer::moveRight()  {
    turnRight();
    moveForward();
    return;
}

//_aiCallTimer->stop();
//ui->txt_status->append("stuff");

void microMouseServer::moveBack()   {
    turnRight();
    turnRight();
    moveForward();
    return;
}

void microMouseServer::assignWalls() { //assigns the walls based on the direction
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

    if (xPosition == 0 && yPosition == 0)   {
        if (myData[xPosition][yPosition].amount == -2)  {
            myData[xPosition][yPosition].amount = 1;
        }
        else    {
            myData[xPosition][yPosition].amount = myData[xPosition][yPosition].amount + 1;
        }
    }
    else if (isWallLeft() && isWallForward() && isWallRight())   {
        myData[xPosition][yPosition].amount = -1;
    }
    else if (((isWallForward() && isWallLeft()) || (isWallForward() && isWallRight()) || (isWallLeft() && isWallRight())) && (direction % 4 == 0) && (myData[xPosition][yPosition-1].amount == -1))    {
        myData[xPosition][yPosition].amount = -1;
    }
    else if (((isWallForward() && isWallLeft()) || (isWallForward() && isWallRight()) || (isWallLeft() && isWallRight())) && ((direction - 1) % 4 == 0) && (myData[xPosition-1][yPosition].amount == -1))    {
        myData[xPosition][yPosition].amount = -1;
    }
    else if (((isWallForward() && isWallLeft()) || (isWallForward() && isWallRight()) || (isWallLeft() && isWallRight())) && ((direction - 2) % 4 == 0) && (myData[xPosition][yPosition+1].amount == -1))    {
        myData[xPosition][yPosition].amount = -1;
    }
    else if (((isWallForward() && isWallLeft()) || (isWallForward() && isWallRight()) || (isWallLeft() && isWallRight())) && ((direction + 1) % 4 == 0) && (myData[xPosition+1][yPosition+1].amount == -1))    {
        myData[xPosition][yPosition].amount = -1;
    }
    else    {
        myData[xPosition][yPosition].amount = myData[xPosition][yPosition].amount + 1;
    }
}

void microMouseServer::leftHandRule()   {
    if(!isWallLeft())   {
        if (direction % 4 == 0 && myData[xPosition-1][yPosition].amount != -1) {
            xPosition = xPosition - 1;
            moveLeft();
        }
        else if ((direction - 1) % 4 == 0 && myData[xPosition][yPosition+1].amount != -1)  {
            yPosition = yPosition + 1;
            moveLeft();
        }
        else if ((direction + 2) % 4 == 0 && myData[xPosition+1][yPosition].amount != -1)  {
            xPosition = xPosition + 1;
            moveLeft();
        }
        else if ((direction + 1) % 4 == 0 && myData[xPosition][yPosition-1].amount != -1)  {
            yPosition = yPosition - 1;
            moveLeft();
        }
        direction = direction - 1;
    }
    else if(!isWallForward())   {
        if (direction % 4 == 0 && myData[xPosition][yPosition+1].amount != -1) {
            yPosition = yPosition + 1;
            moveForward();
        }
        else if ((direction - 1) % 4 == 0 && myData[xPosition+1][yPosition].amount != -1)  {
            xPosition = xPosition + 1;
            moveForward();
        }
        else if ((direction + 2) % 4 == 0 && myData[xPosition][yPosition-1].amount != -1)  {
            yPosition = yPosition - 1;
            moveForward();
        }
        else if ((direction + 1) % 4 == 0 && myData[xPosition-1][yPosition].amount != -1)  {
            xPosition = xPosition - 1;
            moveForward();
        }
    }
    else if(!isWallRight()) {
        if (direction % 4 == 0 && myData[xPosition+1][yPosition].amount != -1) {
            xPosition = xPosition + 1;
            moveRight();
        }
        else if ((direction - 1) % 4 == 0 && myData[xPosition][yPosition-1].amount != -1)  {
            yPosition = yPosition - 1;
            moveRight();
        }
        else if ((direction + 2) % 4 == 0 && myData[xPosition-1][yPosition].amount != -1)  {
            xPosition = xPosition - 1;
            moveRight();
        }
        else if ((direction + 1) % 4 == 0 && myData[xPosition][yPosition+1].amount != -1)  {
            yPosition = yPosition + 1;
            moveRight();
        }
        direction = direction + 1;
    }
    else    {
        if (direction % 4 == 0 && myData[xPosition][yPosition-1].amount != -1) {
            yPosition = yPosition - 1;
            moveBack();
        }
        else if ((direction - 1) % 4 == 0 && myData[xPosition-1][yPosition].amount != -1)  {
            xPosition = xPosition - 1;
            moveBack();
        }
        else if ((direction + 2) % 4 == 0 && myData[xPosition][yPosition+1].amount != -1)  {
            yPosition = yPosition + 1;
            moveBack();
        }
        else if ((direction + 1) % 4 == 0 && myData[xPosition+1][yPosition].amount != -1)  {
            xPosition = xPosition + 1;
            moveBack();
        }
        direction = direction - 2;
    }
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

void microMouseServer::studentAI()
{


    assignWalls();


    leftHandRule();

    populated = true;
    for (int i = 0; i < 20; i++)    {
        for (int j = 0; j < 20; j++)    {
            checkExit(i,j);
        }
    }

    if (win == true)    {
        ui->txt_status->append("I have found the exit");
        ui->txt_status->append("It is located at (" + QString::number(exitXPosition) + "," + QString::number(exitYPosition) + ")");
//           _aiCallTimer->stop();
    }

    if (populated)  {
        _aiCallTimer->stop();
    }
}
