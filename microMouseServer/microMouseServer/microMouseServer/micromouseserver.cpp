#include "micromouseserver.h"
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

mazeCell myData[20][20];    //need array here so all functions can have access to it without passing in a parameter

int exitXPosition = 0;
int exitYPosition = 0;

QVector <mazeCell*> * openset = new QVector <mazeCell*>;
QVector <mazeCell*> * closedset = new QVector <mazeCell*>;

QVector <mazeCell*> * shortestDirections = new QVector <mazeCell*>;

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

//_aiCallTimer->stop();
//ui->txt_status->append("stuff");

void microMouseServer::moveBack()   {
    turnRight();
    turnRight();
    direction = direction + 2;
    realMoveForward();
    return;
}

//end of moving functions


//start of maze returning functions

mazeCell& microMouseServer::positionCurrent()    {
    return myData[xPosition][yPosition];
}

mazeCell& microMouseServer::positionLeft()   {
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
    }
}

mazeCell& microMouseServer::positionTop()    {
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
    }
}

mazeCell& microMouseServer::positionRight()  {
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
    }
}

mazeCell& microMouseServer::positionBottom() {
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
    }
}

/*
mazeCell& microMouseServer::exitPositionTopRight(int x, int y)  {
    switch (direction % 4)  {
    case 0:
        return myData[x+1][y+1];
        break;
    case 1:
        return myData[x+1][y-1];
        break;
    case 2:
        return myData[x-1][y-1];
        break;
    case 3:
        return myData[x-1][y+1];
        break;
    }
}
*/

//end of maze returning functions

//start of amount returning functions

int microMouseServer::amountLeft()  {
    return isWallLeft() ? -3 : positionLeft().amount;
/*
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
    */
}

int microMouseServer::amountTop()  {
    return isWallForward() ? -3 : positionTop().amount;
    /*
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
    */
}

int microMouseServer::amountRight()  {
    return isWallRight() ? -3 : positionRight().amount;
    /*
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
    */
}

int microMouseServer::amountBottom()  {
    return isWallBack() ? -3 : positionBottom().amount;
    /*
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
    */
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
    /*
    switch (direction % 4)  {
    case 0:
        if (amountLeft() == -2) {
            myData[xPosition-1][yPosition].amount = 0;
        }
        if (amountTop() == -2)  {
            myData[xPosition][yPosition+1].amount = 0;
        }
        if (amountRight() == -2)    {
            myData[xPosition+1][yPosition].amount = 0;
        }
        if (amountBottom() == -2) {
            myData[xPosition][yPosition-1].amount = 0;
        }
        if (xPosition == 0 && yPosition == 0 && myData[xPosition][yPosition].amount == -2)  {
            myData[xPosition][yPosition].amount = 0;
        }
        break;
    case 1:
        if (amountLeft() == -2) {
            myData[xPosition][yPosition+1].amount = 0;
        }
        if (amountTop() == -2)  {
            myData[xPosition+1][yPosition].amount = 0;
        }
        if (amountRight() == -2)    {
            myData[xPosition][yPosition-1].amount = 0;
        }
        if (amountBottom() == -2) {
            myData[xPosition-1][yPosition].amount == 0;
        }
        break;
    case 2:
        if (amountLeft() == -2) {
            myData[xPosition+1][yPosition].amount = 0;
        }
        if (amountTop() == -2)  {
            myData[xPosition][yPosition-1].amount = 0;
        }
        if (amountRight() == -2)    {
            myData[xPosition-1][yPosition].amount = 0;
        }
        if (amountBottom() == -2) {
            myData[xPosition][yPosition+1].amount = 0;
        }
        break;
    case 3:
        if (amountLeft() == -2) {
            myData[xPosition][yPosition-1].amount = 0;
        }
        if (amountTop() == -2)  {
            myData[xPosition-1][yPosition].amount = 0;
        }
        if (amountRight() == -2)    {
            myData[xPosition][yPosition+1].amount = 0;
        }
        if (amountBottom() == -2) {
            myData[xPosition+1][yPosition].amount = 0;
        }
        break;
    }
    */
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
    if (!myData[xValue][yValue].wallTop && !myData[xValue][yValue+1].wallRight && !myData[xValue+1][yValue+1].wallBottom && !myData[xValue+1][yValue].wallLeft) {
        win = true;
        exitXPosition = xValue;
        exitYPosition = yValue;
    }
    /* if ((positionCurrent(xValue,yValue).wallLeft && !positionCurrent(xValue,yValue).wallTop && !positionCurrent(xValue,yValue).wallRight && !positionCurrent(xValue,yValue).wallBottom) && (positionTop(xValue,yValue).wallLeft && positionTop(xValue,yValue).wallTop && !positionTop(xValue,yValue).wallRight && !positionTop(xValue,yValue).wallBottom) && (!positionTopRight(xValue,yValue).wallLeft && positionTopRight(xValue,yValue).wallTop && positionTopRight(xValue,yValue).wallRight && !positionTopRight(xValue,yValue).wallBottom) && (!positionRight(xValue,yValue).wallLeft && !positionRight(xValue,yValue).wallTop && positionRight(xValue,yValue).wallRight && positionRight(xValue,yValue).wallBottom))       {
        win = true;
        exitXPosition = xValue;
        exitYPosition = yValue;
    }
    else if ((!positionCurrent(xValue,yValue).wallLeft && !positionCurrent(xValue,yValue).wallTop && positionCurrent(xValue,yValue).wallRight && !positionCurrent(xValue,yValue).wallBottom) && (positionLeft(xValue,yValue).wallLeft && !positionLeft(xValue,yValue).wallTop && !positionLeft(xValue,yValue).wallRight && positionLeft(xValue,yValue).wallBottom) && (positionTopLeft(xValue,yValue).wallLeft && positionTopLeft(xValue,yValue).wallTop && !positionTopLeft(xValue,yValue).wallRight && positionTopRight(xValue,yValue).wallBottom) && (!positionTop(xValue,yValue).wallLeft && positionTop(xValue,yValue).wallTop && positionTop(xValue,yValue).wallRight && !positionTop(xValue,yValue).wallBottom)) {
        win = true;
        exitXPosition = xValue;
        exitYPosition = yValue;
    }*/
/*
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
    */
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

bool microMouseServer::isExit(mazeCell* cell)   {
    if ((exitXPosition == cell->x) && (exitYPosition == cell->y))   {
        return true;
    }
    else    {
        return false;
    }
}


mazeCell& microMouseServer::positionCurrent(int x, int y)   {
    return myData[x][y];
}

mazeCell& microMouseServer::positionLeft(int x, int y)  {
    return myData[x-1][y];
}

mazeCell& microMouseServer::positionTop(int x, int y)   {
    return myData[x][y+1];
}

mazeCell& microMouseServer::positionRight(int x, int y) {
    return myData[x+1][y];
}

mazeCell& microMouseServer::positionBottom(int x, int y)    {
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


void microMouseServer::shellSort(QVector <mazeCell*> * array)  {
    for (int gap = array->size()/2; gap > 0; gap /= 2)   {
        for (int i = gap; i < array->size(); i++)    {
            for (int j=i-gap; j>=0 && &array->at(j)->fCost < &array->at(j+gap)->fCost; j-=gap) {
                /*
                std::swap(array[j],array[j+gap]);
                */
                /*
                std::swap(array->at(j),array->at(j+gap)); //no viable overloaded '=', ERROR NOT EVEN IN THIS CLASS
                */

                mazeCell* temp = array->at(j);
                array->replace(j,array->at(j+gap));
                array->replace(j+gap,temp);

            }
        }
    }
    return;
}

void microMouseServer::insertionSort(QVector <mazeCell*> *array) {
      for (int i = 1; i < array->size(); i++)   {
            int j = i;
            while (j > 0 && array->at(j - 1) > array->at(j)) {
                  mazeCell *temp = array->at(j);
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
        positionCurrent(x,y).hCost = (abs(x - exitXPosition) + abs(y - exitYPosition))*10;
        positionCurrent(x,y).fCost = positionCurrent(x,y).gCost + positionCurrent(x,y).hCost;
        closedset->append(&positionCurrent(x,y));
    }
    return;
}

void microMouseServer::shortestPathDirections() {
    QVector <mazeCell*> * possibilities = new QVector <mazeCell*>;

    for (int i = closedset->last()->gCost; i >= 0; i = i - 10)  {   //if the movement cost is one less than the last checked, then add it to the list of possibilities
        for (int j = 0; j < closedset->size(); j++) {
            if (closedset->at(j)->gCost == i)   {
                possibilities->append(closedset->at(j));
            }
        }

        for (int k = 0; k < possibilities->size(); k++) {   //if the possibility isn't adjacent to the last checked cell, remove it
            if (!shortestDirections->empty())   {
                if (!(shortestDirections->first()->isAdjacent(possibilities->at(k))))   {
                    possibilities->remove(k);
                    k--;
                }
            }
            else    {
                if (!(isExit(possibilities->at(k))))    {   //when the vector is empty, the first value to populate it will be the exit
                    possibilities->remove(k);
                    k--;
                }
            }
        }

//        if (possibilities->size() > 1)  {
//            insertionSort(possibilities);
//        }

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


void microMouseServer::shortestPathFinder(int x, int y) {
    zeroValues(x,y);
    setAdjacentCosts(x,y);
    shellSort(openset);
    removeChecked();
    if (closedset->last() == &myData[exitXPosition][exitYPosition]) {
        shortestPathDirections();
    }
    else    {
        shortestPathFinder(closedset->last()->x,closedset->last()->y);
    }
    return;
}


void microMouseServer::secondRun()  {
    if (shortestDirections->empty())    {
        shortestPathFinder(0,0);
    }
    navigateShortestPath();
    if (atExit())   {
        foundFinish();
    }
    return;
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
    if (populated && xPosition == 0 && yPosition == 0)  {
        finishedMapping = true;
    }
    /*
    if (populated && (xPosition == 0 && yPosition == 0))    {
        atStart = true;
    }
    */
    /*
    if (!populated || !(xPosition == exitXPosition && yPosition == exitYPosition)) {
        firstRun();
    }
    if (!populated || !(xPosition == 0 && yPosition == 0)) {
        firstRun();
    }
    */
    if (!(finishedMapping)) {
        firstRun();
    }
   /* else    {
        ui->txt_status->append("I have found the exit");
        ui->txt_status->append("It is located at (" + QString::number(exitXPosition) + "," + QString::number(exitYPosition) + ")");
    } */
    else    {
        secondRun();
    }
    /*
    if (populated && xPosition == exitXPosition && yPosition == exitYPosition)   {
        ui->txt_status->append("I have found the exit");
        ui->txt_status->append("It is located at (" + QString::number(exitXPosition) + "," + QString::number(exitYPosition) + ")");
        foundFinish();
    }
    */
    /*
    if (win == true)    {
        printUI("I have found the exit");
        printUI(("It is located at (" + QString::number(exitXPosition) + "," + QString::number(exitYPosition) + ")"));
        foundFinish();
    }
    */
/*
    if (myData[exitXPosition][exitYPosition].amount == 3)   {
        _aiCallTimer->stop();
    }*/
    return;
}
