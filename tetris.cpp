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
// ================= DRAW GRID =================
void drawGrid()
{
    glColor4f(0.15f, 0.15f, 0.15f, 1);
    for(int i = 0; i <= ROW; i++)
    {
        glBegin(GL_LINES);
        glVertex2f(0, i); glVertex2f(COL, i);
        glEnd();
    }
    for(int j = 0; j <= COL; j++)
    {
        glBegin(GL_LINES);
        glVertex2f(j, 0); glVertex2f(j, ROW);
        glEnd();
    }

    for(int i = 0; i < ROW; i++)
        for(int j = 0; j < COL; j++)
            if(grid[i][j])
            {
                int s = grid[i][j] - 1;
                drawCell(j, ROW-1-i,
                         colors[s][0], colors[s][1], colors[s][2]);
            }
}

// ================= CURRENT BLOCK =================
void drawBlock()
{
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            if(getShape(currentShape, rotation, i, j))
                drawCell(blockX+j, ROW-1-(blockY+i),
                         colors[currentShape][0],
                         colors[currentShape][1],
                         colors[currentShape][2]);
}

// ================= DRAW TEXT =================
void drawText(float x, float y, string text, void* font = GLUT_BITMAP_HELVETICA_18)
{
    glRasterPos2f(x, y);
    for(char c : text) glutBitmapCharacter(font, c);
}

// ================= LABEL + VALUE HELPER (refactor #3) =================
void drawLabelValue(float x, float labelY, float valueY,
                    string label, string value,
                    float lr, float lg, float lb)
{
    glColor3f(lr, lg, lb);
    drawText(x, labelY, label);
    glColor3f(1, 1, 1);
    drawText(x, valueY, value);
}

// ================= MIDPOINT CIRCLE =================
void drawCircle(int cx, int cy, int r)
{
    int x = 0, y = r, p = 1 - r;
    glBegin(GL_POINTS);
    while(x <= y)
    {
        glVertex2i(cx+x, cy+y); glVertex2i(cx+y, cy+x);
        glVertex2i(cx-x, cy+y); glVertex2i(cx-y, cy+x);
        glVertex2i(cx-x, cy-y); glVertex2i(cx-y, cy-x);
        glVertex2i(cx+x, cy-y); glVertex2i(cx+y, cy-x);
        x++;
        if(p < 0) p += 2*x + 1;
        else { y--; p += 2*(x-y) + 1; }
    }
    glEnd();
}

// ================= CLOCK (refactor #8) =================
// Draws circle and tick marks directly in world coords — no push/scale block
void drawClock()
{
    // World-space center and radius for the clock face
    const float cx = 15.5f, cy = 4.5f;
    const float faceR   = 3.08f;   // 88 * 0.035
    const float innerR  = 3.01f;   // 86 * 0.035
    const float tickOut = 2.975f;  // 85 * 0.035
    const float tickIn  = 2.625f;  // 75 * 0.035

    // Clock outline — approximate with a 64-segment polygon
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_LINE_LOOP);
    for(int k = 0; k < 64; k++)
    {
        float a = k * 2.0f * M_PI / 64;
        glVertex2f(cx + faceR * cos(a), cy + faceR * sin(a));
    }
    glEnd();
    glBegin(GL_LINE_LOOP);
    for(int k = 0; k < 64; k++)
    {
        float a = k * 2.0f * M_PI / 64;
        glVertex2f(cx + innerR * cos(a), cy + innerR * sin(a));
    }
    glEnd();

    // Hour tick marks
    glColor3f(0.8f, 0.8f, 0.8f);
    for(int h = 0; h < 12; h++)
    {
        float ang = h * 30.0f * M_PI / 180.0f;
        float s = sin(ang), c = cos(ang);
        glBegin(GL_LINES);
        glVertex2f(cx + tickIn  * s, cy + tickIn  * c);
        glVertex2f(cx + tickOut * s, cy + tickOut * c);
        glEnd();
    }

    // Clock hands
    time_t t = time(0);
    tm* now = localtime(&t);

    float sec = now->tm_sec  * 6.0f;
    float min = now->tm_min  * 6.0f + now->tm_sec  * 0.1f;
    float hr  = (now->tm_hour % 12) * 30.0f + now->tm_min * 0.5f;

    // Hour hand
    glLineWidth(2.5f);
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f(cx, cy);
    glVertex2f(cx + 1.5f * sin(hr  * M_PI / 180), cy + 1.5f * cos(hr  * M_PI / 180));
    glEnd();

    // Minute hand
    glLineWidth(1.5f);
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_LINES);
    glVertex2f(cx, cy);
    glVertex2f(cx + 2.2f * sin(min * M_PI / 180), cy + 2.2f * cos(min * M_PI / 180));
    glEnd();

    // Second hand
    glLineWidth(1.0f);
    glColor3f(1, 0.2f, 0.2f);
    glBegin(GL_LINES);
    glVertex2f(cx, cy);
    glVertex2f(cx + 2.8f * sin(sec * M_PI / 180), cy + 2.8f * cos(sec * M_PI / 180));
    glEnd();

    // Center dot
    glPointSize(4.0f);
    glColor3f(1, 1, 1);
    glBegin(GL_POINTS);
    glVertex2f(cx, cy);
    glEnd();
    glPointSize(1.0f);
    glLineWidth(1.0f);
}

// ================= OVERLAY SCREEN (refactor #2) =================
// Merged drawGameOverScreen() + drawPauseScreen()
void drawOverlay(bool isGameOver)
{
    // Semi-transparent backdrop
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, isGameOver ? 0.75f : 0.6f);
    glBegin(GL_QUADS);
        glVertex2f(0, 0); glVertex2f(10, 0);
        glVertex2f(10, 20); glVertex2f(0, 20);
    glEnd();
    glDisable(GL_BLEND);

    if(isGameOver)
    {
        // Outer border
        glColor3f(1, 0.2f, 0.2f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(1.0f, 7.5f); glVertex2f(9.0f, 7.5f);
            glVertex2f(9.0f, 13.5f); glVertex2f(1.0f, 13.5f);
        glEnd();
        glLineWidth(1.0f);
        glColor3f(0.6f, 0.1f, 0.1f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(1.3f, 7.8f); glVertex2f(8.7f, 7.8f);
            glVertex2f(8.7f, 13.2f); glVertex2f(1.3f, 13.2f);
        glEnd();

        // Inner fill
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.15f, 0, 0, 0.95f);
        glBegin(GL_QUADS);
            glVertex2f(1.3f, 7.8f); glVertex2f(8.7f, 7.8f);
            glVertex2f(8.7f, 13.2f); glVertex2f(1.3f, 13.2f);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(1, 0.2f, 0.2f);
        drawText(2.2f, 12.2f, "GAME  OVER", GLUT_BITMAP_TIMES_ROMAN_24);

        glColor3f(0.5f, 0.1f, 0.1f);
        glBegin(GL_LINES);
        glVertex2f(1.5f, 11.7f); glVertex2f(8.5f, 11.7f);
        glEnd();

        glColor3f(0.7f, 0.7f, 0.7f);
        drawText(2.5f, 11.1f, "Score:  " + to_string(score));

        if(score >= highScore && score > 0)
        {
            glColor3f(1, 0.85f, 0);
            drawText(2.5f, 10.3f, "NEW HIGH SCORE!");
        }
        else
        {
            glColor3f(0.6f, 0.6f, 0.6f);
            drawText(2.5f, 10.3f, "Best:   " + to_string(highScore));
        }

        glColor3f(0.5f, 0.5f, 0.5f);
        drawText(2.5f, 9.3f, "R to restart", GLUT_BITMAP_HELVETICA_12);
    }
    else
    {
        // Pause box
        glColor3f(1, 1, 0);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(2.0f, 9.0f); glVertex2f(8.0f, 9.0f);
            glVertex2f(8.0f, 12.0f); glVertex2f(2.0f, 12.0f);
        glEnd();
        glLineWidth(1.0f);

        glColor3f(1, 1, 0);
        drawText(3.5f, 11.2f, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(0.7f, 0.7f, 0.7f);
        drawText(2.8f, 10.0f, "Press P to continue");
    }
}

// ================= SIDE PANEL (refactor #4 + #9) =================
// Merged drawNext() + drawScore() into one function
void drawSidePanel()
{
    // --- Next piece ---
    glColor3f(0.7f, 0.7f, 0.7f);
    drawText(12, 18.5, "Next:");

    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(12.5, 13.8); glVertex2f(17.5, 13.8);
        glVertex2f(17.5, 17.5); glVertex2f(12.5, 17.5);
    glEnd();

    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            if(shapes[nextShape][i][j])
                drawCell(13+j, 16-i,
                         colors[nextShape][0],
                         colors[nextShape][1],
                         colors[nextShape][2]);

    // --- Score and best (refactor #3) ---
    drawLabelValue(12, 13, 12.2, "Score:", to_string(score),   0.5f, 0.8f, 1.0f);
    drawLabelValue(12, 11, 10.2, "Best:",  to_string(highScore), 1.0f, 0.8f, 0.3f);
}

// ================= PLACE + CLEAR + NEW (refactor #5) =================
// Merged placeBlock() + clearLines() + newBlock() into lockBlock()
void lockBlock()
{
    // Place
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            if(getShape(currentShape, rotation, i, j))
                grid[blockY+i][blockX+j] = currentShape + 1;

    // Clear lines — iterate bottom-up to avoid skipping rows (bug fix)
    int cleared = 0;
    for(int i = ROW - 1; i >= 0; i--)
    {
        bool full = true;
        for(int j = 0; j < COL; j++)
            if(!grid[i][j]) { full = false; break; }

        if(full)
        {
            cleared++;
            for(int k = i; k > 0; k--)
                for(int j = 0; j < COL; j++)
                    grid[k][j] = grid[k-1][j];
            for(int j = 0; j < COL; j++) grid[0][j] = 0;
            i++;  // re-check this row index after shift
        }
    }

    int pts[] = {0, 100, 300, 500, 800};
    if(cleared > 0)
    {
        score += pts[cleared];
        if(score > highScore) highScore = score;
    }

    // New block
    currentShape = nextShape;
    nextShape = rand() % 7;
    blockX = 3; blockY = 0; rotation = 0;

    if(collision(blockX, blockY, rotation))
        gameOver = true;
}

// ================= RESET (for R key restart) =================
void resetGame()
{
    for(int i = 0; i < ROW; i++)
        for(int j = 0; j < COL; j++)
            grid[i][j] = 0;
    score = 0;
    blockX = 3; blockY = 0; rotation = 0;
    gameOver = false;
    isPaused = false;
    currentShape = rand() % 7;
    nextShape = rand() % 7;
}

// ================= DISPLAY =================
void display()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 20, 0, 20);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT);

    drawBorderRect(0, 10, 0, 20);   // game area  (refactor #1)
    drawGrid();

    if(!gameOver)
        drawBlock();

    drawBorderRect(11, 20, 0, 20);  // side panel (refactor #1)
    drawSidePanel();                 // next + score (refactor #9)
    drawClock();

    glColor3f(0.35f, 0.35f, 0.35f);
    drawText(11.5f, 1.3f, "A/D: move",          GLUT_BITMAP_HELVETICA_12);
    drawText(11.5f, 0.8f, "W: rotate",          GLUT_BITMAP_HELVETICA_12);
    drawText(11.5f, 0.3f, "S: drop  P: pause",  GLUT_BITMAP_HELVETICA_12);

    if(gameOver)  drawOverlay(true);   // (refactor #2)
    if(isPaused)  drawOverlay(false);  // (refactor #2)

    glutSwapBuffers();
}

// ================= UPDATE =================
void update(int)
{
    if(!gameOver && !isPaused)
    {
        if(!collision(blockX, blockY+1, rotation))
            blockY++;
        else
            lockBlock();  // (refactor #5)
    }

    glutPostRedisplay();
    glutTimerFunc(speed, update, 0);
}

// ================= KEYBOARD (refactor #6) =================
// Movement checks collapsed into a lookup table
void keyboard(unsigned char key, int, int)
{
    if(key == 'p' || key == 'P') { isPaused = !isPaused; return; }
    if(key == 'r' || key == 'R') { if(gameOver) resetGame(); return; }
    if(isPaused || gameOver) return;

    struct Move { unsigned char key; int dx, dy, dr; };
    Move moves[] = {
        {'a', -1,  0,  0},
        {'d',  1,  0,  0},
        {'s',  0,  1,  0},
        {'w',  0,  0,  1},
    };

    for(auto& m : moves)
        if(key == m.key && !collision(blockX + m.dx, blockY + m.dy, rotation + m.dr))
        {
            blockX += m.dx;
            blockY += m.dy;
            rotation += m.dr;
            break;
        }

    glutPostRedisplay();
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
