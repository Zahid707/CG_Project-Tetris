#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
using namespace std;

// ================= GLOBAL =================
const int ROW = 20, COL = 10;
int grid[ROW][COL] = {0};

int blockX = 3, blockY = 0;
int currentShape, nextShape;
int rotation = 0;

int score = 0;
int highScore = 0;
int speed = 400;

bool gameOver = false;
bool isPaused = false;

// ================= SHAPES =================
int shapes[7][4][4] =
    {
        {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};

float colors[7][3] =
    {
        {0, 1, 1},
        {1, 1, 0},
        {0.6, 0, 1},
        {1, 0.5, 0},
        {0, 0.4, 1},
        {0, 1, 0.3},
        {1, 0.1, 0.1}};

// ================= ROTATION =================
int getShape(int s, int r, int i, int j)
{
    switch (r % 4)
    {
    case 0:
        return shapes[s][i][j];
    case 1:
        return shapes[s][3 - j][i];
    case 2:
        return shapes[s][3 - i][3 - j];
    case 3:
        return shapes[s][j][3 - i];
    }
    return 0;
}

// ================= DRAW CELL =================
void drawCell(float x, float y, float r, float g, float b, float alpha = 1.0f)
{
    glColor4f(r, g, b, alpha);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + 1, y);
    glVertex2f(x + 1, y + 1);
    glVertex2f(x, y + 1);
    glEnd();

    glColor4f(1, 1, 1, 0.25f * alpha);
    glBegin(GL_LINES);
    glVertex2f(x + 0.05f, y + 0.05f);
    glVertex2f(x + 0.95f, y + 0.05f);
    glVertex2f(x + 0.05f, y + 0.05f);
    glVertex2f(x + 0.05f, y + 0.95f);
    glEnd();

    glColor4f(0, 0, 0, alpha);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + 1, y);
    glVertex2f(x + 1, y + 1);
    glVertex2f(x, y + 1);
    glEnd();
}

// ================= COLLISION =================
bool collision(int x, int y, int r)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (getShape(currentShape, r, i, j))
            {
                int nx = x + j, ny = y + i;
                if (nx < 0 || nx >= COL || ny >= ROW)
                    return true;
                if (ny >= 0 && grid[ny][nx])
                    return true;
            }
    return false;
}

// ================= DRAW BORDER (refactor #1) =================
// Merged drawGameBorder() + drawUIPanel() — just pass x1/x2
void drawBorderRect(float x1, float x2, float y1, float y2)
{
    glColor3f(0.8f, 0.8f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
    glLineWidth(1.0f);
}

// ================= INIT =================
void init()
{
    glClearColor(0.05f, 0.05f, 0.08f, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    srand(time(0));
    nextShape = rand() % 7;
}

// ================= MAIN =================
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);

    glutCreateWindow("Tetris");

    init();

    // Seed first block properly via lockBlock path — just set up shapes directly
    currentShape = rand() % 7;
    nextShape = rand() % 7;

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
}
