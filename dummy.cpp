#include <GL/glut.h>

float angle = 0.0;

void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-100, 100, -100, 100, -1, 1);
}

void drawRectangle()
{
    glBegin(GL_QUADS);
        glVertex2f(-30, -20);
        glVertex2f(30, -20);
        glVertex2f(30, 20);
        glVertex2f(-30, 20);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1, 0, 0);

    glPushMatrix();
    glRotatef(angle, 0, 0, 1);  // Z-axis rotate
    drawRectangle();
    glPopMatrix();

    glFlush();
}

void specialKeys(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_RIGHT:
            angle += 5;   // rotate right
            break;

        case GLUT_KEY_LEFT:
            angle -= 5;   // rotate left
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("2D Rotation");

    init();

    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}
