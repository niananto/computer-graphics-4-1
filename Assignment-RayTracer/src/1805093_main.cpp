#include "1805093_def.hpp"
#include "1805093_utils.hpp"
#include "1805093_point.h"

int nearPlane, farPlane, fovY, aspectRatio;
int recursionLevel, imageWidth, imageHeight;
int checkerBoardWidth, checkerBoardHeight;
vector<double> checkerBoardCoeff(3);
vector<Object *> objects;
vector<LightSource *> lights;

Point pos; // position of the eye
Point l;   // look/forward direction
Point r;   // right direction - dynamically updated in the display function
Point u;   // up direction
Point c;   // center of the scene - temp use

GLint rotate_y = 0;

void init()
{
    pos.x = 0;
    pos.y = -200;
    pos.z = 40;
    l.x = 0;
    l.y = 50;
    l.z = -10;
    l.normalize();
    r.x = 1;
    r.y = 0;
    r.z = 0;
    u = *(r.cross(&l));

    getInputs();

    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovY, aspectRatio, nearPlane, farPlane);
    glEnable(GL_DEPTH_TEST);
}

void drawAxes()
{
    glLineWidth(3);
    glBegin(GL_LINES);
    
    glColor3f(100, 0, 0); // Red
    // X axis
    glVertex3f(0, 0, 0);
    glVertex3f(100, 0, 0);

    glColor3f(0, 100, 0); // Green
    // Y axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 100, 0);

    glColor3f(0, 0, 100); // Blue
    // Z axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 100);
    glEnd();
}

void drawChecker()
{
    // draw in infinite checker board with given size and coeff
    for (int i = -100; i < 100; i++)
    {
        for (int j = -100; j < 100; j++)
        {
            if ((i + j) % 2 == 0)
                glColor3f(1, 1, 1);
            else
                glColor3f(0, 0, 0);

            // glBegin(GL_QUADS);
            // glVertex3f(i * checkerBoardWidth, 0, j * checkerBoardHeight);
            // glVertex3f((i + 1) * checkerBoardWidth, 0, j * checkerBoardHeight);
            // glVertex3f((i + 1) * checkerBoardWidth, 0, (j + 1) * checkerBoardHeight);
            // glVertex3f(i * checkerBoardWidth, 0, (j + 1) * checkerBoardHeight);

            // checkerboard in XY plane
            glBegin(GL_QUADS);
            glVertex3f(i * checkerBoardWidth, j * checkerBoardHeight, 0);
            glVertex3f((i + 1) * checkerBoardWidth, j * checkerBoardHeight, 0);
            glVertex3f((i + 1) * checkerBoardWidth, (j + 1) * checkerBoardHeight, 0);
            glVertex3f(i * checkerBoardWidth, (j + 1) * checkerBoardHeight, 0);

            glEnd();
        }
    }
}

/*  Handler for window-repaint event. Call back when the window first appears and
    whenever the window needs to be re-painted. */
void display()
{
    // r = (l-pos) x u
    r.x = (l.y - pos.y) * u.z - (l.z - pos.z) * u.y;
    r.y = (l.z - pos.z) * u.x - (l.x - pos.x) * u.z;
    r.z = (l.x - pos.x) * u.y - (l.y - pos.y) * u.x;
    // normalize r
    float scale = 1 / sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
    r.x *= scale;
    r.y *= scale;
    r.z *= scale;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(pos.x, pos.y, pos.z,
              pos.x + l.x, pos.y + l.y, pos.z + l.z,
              u.x, u.y, u.z);

    // rotation for a, d buttons
    glRotatef(rotate_y, 0, 1, 0);

    // draw
    drawAxes();
    drawChecker();
    for (Object *object : objects)
    {
        object->draw();
    }

    glutSwapBuffers();
}

void reshapeListener(GLsizei width, GLsizei height)
{
    if (height == 0)
        height = 1; // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovY, aspect, nearPlane, farPlane);
}

void keyboardListener(unsigned char key, int x, int y)
{
    double v = 0.25;
    double rate = 0.05;
    double s;

    c.x = pos.x + l.x;
    c.y = pos.y + l.y;
    c.z = pos.z + l.z;

    switch (key)
    {
    // control translation
    case '1':
        r.x = r.x * cos(rate) + l.x * sin(rate);
        r.y = r.y * cos(rate) + l.y * sin(rate);
        r.z = r.z * cos(rate) + l.z * sin(rate);

        l.x = l.x * cos(rate) - r.x * sin(rate);
        l.y = l.y * cos(rate) - r.y * sin(rate);
        l.z = l.z * cos(rate) - r.z * sin(rate);
        break;

    case '2':
        r.x = r.x * cos(-rate) + l.x * sin(-rate);
        r.y = r.y * cos(-rate) + l.y * sin(-rate);
        r.z = r.z * cos(-rate) + l.z * sin(-rate);

        l.x = l.x * cos(-rate) - r.x * sin(-rate);
        l.y = l.y * cos(-rate) - r.y * sin(-rate);
        l.z = l.z * cos(-rate) - r.z * sin(-rate);
        break;

    case '3':
        l.x = l.x * cos(rate) + u.x * sin(rate);
        l.y = l.y * cos(rate) + u.y * sin(rate);
        l.z = l.z * cos(rate) + u.z * sin(rate);

        u.x = u.x * cos(rate) - l.x * sin(rate);
        u.y = u.y * cos(rate) - l.y * sin(rate);
        u.z = u.z * cos(rate) - l.z * sin(rate);
        break;

    case '4':
        l.x = l.x * cos(-rate) + u.x * sin(-rate);
        l.y = l.y * cos(-rate) + u.y * sin(-rate);
        l.z = l.z * cos(-rate) + u.z * sin(-rate);

        u.x = u.x * cos(-rate) - l.x * sin(-rate);
        u.y = u.y * cos(-rate) - l.y * sin(-rate);
        u.z = u.z * cos(-rate) - l.z * sin(-rate);
        break;

    case '5':
        u.x = u.x * cos(rate) + r.x * sin(rate);
        u.y = u.y * cos(rate) + r.y * sin(rate);
        u.z = u.z * cos(rate) + r.z * sin(rate);

        r.x = r.x * cos(rate) - u.x * sin(rate);
        r.y = r.y * cos(rate) - u.y * sin(rate);
        r.z = r.z * cos(rate) - u.z * sin(rate);
        break;

    case '6':
        u.x = u.x * cos(-rate) + r.x * sin(-rate);
        u.y = u.y * cos(-rate) + r.y * sin(-rate);
        u.z = u.z * cos(-rate) + r.z * sin(-rate);

        r.x = r.x * cos(-rate) - u.x * sin(-rate);
        r.y = r.y * cos(-rate) - u.y * sin(-rate);
        r.z = r.z * cos(-rate) - u.z * sin(-rate);
        break;

    case '0':
        // start raytracing
        // and output a bmp
        break;

    // control viewing (or camera)
    case 'w':
        // move up without changing reference point
        pos.x += v * u.x;
        pos.y += v * u.y;
        pos.z += v * u.z;

        l.x = c.x - pos.x;
        l.y = c.y - pos.y;
        l.z = c.z - pos.z;
        break;
    case 's':
        // move down without changing reference point
        pos.x -= v * u.x;
        pos.y -= v * u.y;
        pos.z -= v * u.z;

        l.x = c.x - pos.x;
        l.y = c.y - pos.y;
        l.z = c.z - pos.z;
        break;
    case 'a':
        // rotate the object in the clockwise direction about its own axis
        // pos.x += v * (-u.y*l.z);
        // pos.y += v * (l.x*u.y);

        // l.x = c.x - pos.x;
        // l.y = c.y - pos.y;
        // l.normalize();

        rotate_y -= 5;
        break;
    case 'd':
        // rotate the object in the anti-clockwise direction about its own axis
        // pos.x += v * (u.y*l.z);
        // pos.z += v * (-l.x*u.y);

        // l.x = c.x - pos.x;
        // l.z = c.z - pos.z;
        // l.normalize();

        rotate_y += 5;
        break;

    // control exit
    case 27:     // ESC key
        exit(0); // Exit window
        break;

    default:
        break;
    }

    glutPostRedisplay(); // Post a paint request to activate display()
}

/* Callback handler for special-key event */
void specialKeyListener(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        pos.x += l.x;
        pos.y += l.y;
        pos.z += l.z;
        break;
    case GLUT_KEY_DOWN:
        pos.x -= l.x;
        pos.y -= l.y;
        pos.z -= l.z;
        break;

    case GLUT_KEY_RIGHT:
        pos.x += r.x;
        pos.y += r.y;
        pos.z += r.z;
        break;
    case GLUT_KEY_LEFT:
        pos.x -= r.x;
        pos.y -= r.y;
        pos.z -= r.z;
        break;

    case GLUT_KEY_PAGE_UP:
        pos.x += u.x;
        pos.y += u.y;
        pos.z += u.z;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x -= u.x;
        pos.y -= u.y;
        pos.z -= u.z;
        break;

    default:
        break;
    }

    glutPostRedisplay(); // Post a paint request to activate display()
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 640);
    glutInitWindowPosition(1100, 100);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Ray Tracing");
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeListener);
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    init();
    glutMainLoop();
    return 0;
}
