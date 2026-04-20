#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

// ================= GLOBAL =================
const int ROW = 20, COL = 10;
int grid[ROW][COL] = {0};

int blockX = 3, blockY = 0;
int currentShape, nextShape;
int rotation = 0;

int score = 0;
int speed = 400;

bool gameOver = false;
bool isPaused = false;

// ================= SHAPES =================
int shapes[7][4][4] =
{
    {{1,1,1,1},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,1,0},{1,0,0,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,1,0},{0,0,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}
};

float colors[7][3] =
{
    {0,1,1},{1,1,0},{1,0,1},{1,0.5,0},
    {0,0,1},{0,1,0},{1,0,0}
};

// ================= ROTATION =================
int getShape(int s,int r,int i,int j)
{
    switch(r%4)
    {
        case 0: return shapes[s][i][j];
        case 1: return shapes[s][3-j][i];
        case 2: return shapes[s][3-i][3-j];
        case 3: return shapes[s][j][3-i];
    }
    return 0;
}

// ================= DRAW CELL (WITH BORDER) =================
void drawCell(int x,int y)
{
    // ✅ save color before drawing border
    float r,g,b;
    glGetFloatv(GL_CURRENT_COLOR, new float[4]{r,g,b,1}); // won't work cleanly

    glBegin(GL_QUADS);
        glVertex2f(x,   y);
        glVertex2f(x+1, y);
        glVertex2f(x+1, y+1);
        glVertex2f(x,   y+1);
    glEnd();

    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x,   y);
        glVertex2f(x+1, y);
        glVertex2f(x+1, y+1);
        glVertex2f(x,   y+1);
    glEnd();
}

// ================= GRID =================
void drawGrid()
{
    for(int i=0;i<ROW;i++)
        for(int j=0;j<COL;j++)
            if(grid[i][j])
            {
                glColor3fv(colors[grid[i][j]-1]); // ✅ already inside loop, this was fine
                drawCell(j, ROW-i);
            }
}
// ================= CURRENT BLOCK =================
void drawBlock()
{
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            if(getShape(currentShape,rotation,i,j))
            {
                glColor3fv(colors[currentShape]); // ✅ set color INSIDE loop
                drawCell(blockX+j, ROW-(blockY+i));
            }
}

// ================= UI BORDER =================
void drawGameBorder()
{
    glColor3f(1,1,1);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0,0);
        glVertex2f(10,0);
        glVertex2f(10,20);
        glVertex2f(0,20);
    glEnd();
}

void drawUIPanel()
{
    glColor3f(1,1,1);
    glBegin(GL_LINE_LOOP);
        glVertex2f(11,0);
        glVertex2f(20,0);
        glVertex2f(20,20);
        glVertex2f(11,20);
    glEnd();
}

// ================= NEXT =================
void drawNext()
{
    glColor3f(1,1,1);
    glRasterPos2f(12,18);
    string t="Next:";
    for(char c:t) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,c);

    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            if(shapes[nextShape][i][j])
            {
                glColor3fv(colors[nextShape]);
                drawCell(13+j,15-i);
            }
}

// ================= SCORE =================
void drawScore()
{
    string s="Score: "+to_string(score);
    glColor3f(1,1,1);
    glRasterPos2f(12,11);

    for(char c:s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,c);
}

// ================= MIDPOINT CIRCLE =================
void drawCircle(int r)
{
    int x=0,y=r,p=1-r;

    glBegin(GL_POINTS);
    while(x<=y)
    {
        glVertex2i(x,y); glVertex2i(y,x);
        glVertex2i(-x,y); glVertex2i(-y,x);
        glVertex2i(-x,-y); glVertex2i(-y,-x);
        glVertex2i(x,-y); glVertex2i(y,-x);
        x++;
        if(p<0) p+=2*x+1;
        else { y--; p+=2*(x-y)+1; }
    }
    glEnd();
}

// ================= CLOCK =================
void drawClock()
{
    glPushMatrix();
    glTranslatef(15.5,5,0);
    glScalef(0.033f, 0.033f, 1); // ✅ scale so radius-3 fits in UI units

    glColor3f(1,1,1);
    drawCircle(90); // ✅ larger radius, scaled down

    glPopMatrix();

    // draw hands separately in world units
    glPushMatrix();
    glTranslatef(15.5,5,0);

    time_t t=time(0);
    tm *now=localtime(&t);

    float sec=now->tm_sec*6;
    float min=now->tm_min*6;
    float hr=(now->tm_hour%12)*30;

    glColor3f(1,1,1);
    glBegin(GL_LINES);
    glVertex2f(0,0);
    glVertex2f(1.5*sin(hr*M_PI/180), 1.5*cos(hr*M_PI/180));
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0,0);
    glVertex2f(2.0*sin(min*M_PI/180), 2.0*cos(min*M_PI/180));
    glEnd();

    glColor3f(1,0,0);
    glBegin(GL_LINES);
    glVertex2f(0,0);
    glVertex2f(2.5*sin(sec*M_PI/180), 2.5*cos(sec*M_PI/180));
    glEnd();

    glPopMatrix();
}
// ================= COLLISION =================
bool collision(int x,int y,int r)
{
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            if(getShape(currentShape,r,i,j))
            {
                int nx=x+j, ny=y+i;
                if(nx<0||nx>=COL||ny>=ROW) return true;
                if(ny>=0 && grid[ny][nx]) return true;
            }
    return false;
}

// ================= PLACE =================
void placeBlock()
{
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            if(getShape(currentShape,rotation,i,j))
                grid[blockY+i][blockX+j]=currentShape+1;
}

// ================= CLEAR =================
void clearLines()
{
    for(int i=0;i<ROW;i++)
    {
        bool full=true;
        for(int j=0;j<COL;j++)
            if(!grid[i][j]) full=false;

        if(full)
        {
            score+=100;
            for(int k=i;k>0;k--)
                for(int j=0;j<COL;j++)
                    grid[k][j]=grid[k-1][j];
        }
    }
}

// ================= NEW BLOCK =================
void newBlock()
{
    currentShape = nextShape;
    nextShape = rand()%7;

    blockX=3; blockY=0; rotation=0;

    if(collision(blockX,blockY,rotation))
        gameOver=true;
}

// ================= RESET =================
void resetGame()
{
    for(int i=0;i<ROW;i++)
        for(int j=0;j<COL;j++)
            grid[i][j]=0;

    score=0;
    speed=400;
    gameOver=false;
    isPaused=false;

    nextShape=rand()%7;
    newBlock();
}

// ================= DISPLAY =================
void display()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,20,0,20);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT);

    drawGameBorder();
    drawGrid();
    drawBlock();

    drawUIPanel();
    drawNext();
    drawScore();
    drawClock();

    if(gameOver)
    {
        glColor3f(1,0,0);
        glRasterPos2f(3,10);
        string g="GAME OVER";
        for(char c:g) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,c);
    }

    if(isPaused)
    {
        glColor3f(1,1,0);
        glRasterPos2f(3,12);
        string p="PAUSED";
        for(char c:p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,c);
    }

    glutSwapBuffers();
}

// ================= UPDATE =================
void update(int)
{
    if(!gameOver && !isPaused)
    {
        if(!collision(blockX,blockY+1,rotation))
            blockY++;
        else
        {
            placeBlock();
            clearLines();
            newBlock();
        }
    }

    glutPostRedisplay();
    glutTimerFunc(speed,update,0);
}

// ================= KEYBOARD =================
void keyboard(unsigned char key,int,int)
{
    if(key=='p'){ isPaused=!isPaused; return; }
    if(key=='r'){ resetGame(); return; }

    if(isPaused || gameOver) return;

    if(key=='a'&&!collision(blockX-1,blockY,rotation)) blockX--;
    if(key=='d'&&!collision(blockX+1,blockY,rotation)) blockX++;
    if(key=='s'&&!collision(blockX,blockY+1,rotation)) blockY++;
    if(key=='w'&&!collision(blockX,blockY,rotation+1)) rotation++;

    glutPostRedisplay();
}

// ================= INIT =================
void init()
{
    glClearColor(0,0,0,1);
    glPointSize(1.0f);  // ✅ ensure point size is 1
    srand(time(0));
    nextShape=rand()%7;
}

// ================= MAIN =================
int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(600,800);
    glutCreateWindow("FINAL TETRIS LAB PROJECT");

    init();
    newBlock();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0,update,0);

    glutMainLoop();
}
