#include "1805093_def.hpp"
#include "1805093_utils.hpp"

int nearPlane, farPlane, fovY, fovX, aspectRatio;
int recursionLevel, imageWidth, imageHeight;
int checkerBoardWidth, checkerBoardHeight;
vector<double> checkerBoardCoeff(3);
vector<Object *> objects;
vector<LightSource *> lights;

Point* pos; // position of the eye
Point* look;   // look/forward direction
Point* r8;   // r8 direction - dynamically updated in the display function
Point* up;   // up direction
Point* center;   // center of the scene - temp use

GLint rotate_z = 0;

void init()
{
    pos = new Point(0, -200, 100);
    look = new Point(0, 50, -10);
    look->normalize();
    r8 = new Point(1, 0, 0);
    r8->normalize();
    up = r8->cross(look);
    center = new Point();

    getInputs();

    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovY, aspectRatio, nearPlane, farPlane);
    glEnable(GL_DEPTH_TEST);
}

void clearMem()
{
    delete pos;
    delete look;
    delete r8;
    delete up;
    delete center;

    for (Object *object : objects)
    {
        delete object;
    }

    for (LightSource *light : lights)
    {
        delete light;
    }
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
    // // r = (l-pos) x u
    // r8->x = (look->y - pos->y) * up->z - (look->z - pos->z) * up->y;
    // r8->y = (look->z - pos->z) * up->x - (look->x - pos->x) * up->z;
    // r8->z = (look->x - pos->x) * up->y - (look->y - pos->y) * up->x;
    // // normalize r
    // r8->normalize();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(pos->x, pos->y, pos->z,
              pos->x + look->x, pos->y + look->y, pos->z + look->z,
              up->x, up->y, up->z);

    // rotation for a, d buttons
    glRotatef(rotate_z, 0, 0, 1);

    // draw
    drawAxes();
    drawChecker();
    for (Object *object : objects)
    {
        // if (object->objectType == "pyramid")
        object->draw();
    }
    for (LightSource *light : lights)
    {
        light->draw();
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

    center->x = pos->x + look->x;
    center->y = pos->y + look->y;
    center->z = pos->z + look->z;

    switch (key)
    {
    // control translation
    case '1':
        r8->x = r8->x * cos(rate) + look->x * sin(rate);
        r8->y = r8->y * cos(rate) + look->y * sin(rate);
        r8->z = r8->z * cos(rate) + look->z * sin(rate);

        look->x = look->x * cos(rate) - r8->x * sin(rate);
        look->y = look->y * cos(rate) - r8->y * sin(rate);
        look->z = look->z * cos(rate) - r8->z * sin(rate);
        break;

    case '2':
        r8->x = r8->x * cos(-rate) + look->x * sin(-rate);
        r8->y = r8->y * cos(-rate) + look->y * sin(-rate);
        r8->z = r8->z * cos(-rate) + look->z * sin(-rate);

        look->x = look->x * cos(-rate) - r8->x * sin(-rate);
        look->y = look->y * cos(-rate) - r8->y * sin(-rate);
        look->z = look->z * cos(-rate) - r8->z * sin(-rate);
        break;

    case '3':
        look->x = look->x * cos(rate) + up->x * sin(rate);
        look->y = look->y * cos(rate) + up->y * sin(rate);
        look->z = look->z * cos(rate) + up->z * sin(rate);

        up->x = up->x * cos(rate) - look->x * sin(rate);
        up->y = up->y * cos(rate) - look->y * sin(rate);
        up->z = up->z * cos(rate) - look->z * sin(rate);
        break;

    case '4':
        look->x = look->x * cos(-rate) + up->x * sin(-rate);
        look->y = look->y * cos(-rate) + up->y * sin(-rate);
        look->z = look->z * cos(-rate) + up->z * sin(-rate);

        up->x = up->x * cos(-rate) - look->x * sin(-rate);
        up->y = up->y * cos(-rate) - look->y * sin(-rate);
        up->z = up->z * cos(-rate) - look->z * sin(-rate);
        break;

    case '5':
        up->x = up->x * cos(rate) + r8->x * sin(rate);
        up->y = up->y * cos(rate) + r8->y * sin(rate);
        up->z = up->z * cos(rate) + r8->z * sin(rate);

        r8->x = r8->x * cos(rate) - up->x * sin(rate);
        r8->y = r8->y * cos(rate) - up->y * sin(rate);
        r8->z = r8->z * cos(rate) - up->z * sin(rate);
        break;

    case '6':
        up->x = up->x * cos(-rate) + r8->x * sin(-rate);
        up->y = up->y * cos(-rate) + r8->y * sin(-rate);
        up->z = up->z * cos(-rate) + r8->z * sin(-rate);

        r8->x = r8->x * cos(-rate) - up->x * sin(-rate);
        r8->y = r8->y * cos(-rate) - up->y * sin(-rate);
        r8->z = r8->z * cos(-rate) - up->z * sin(-rate);
        break;

    case '0':
        // start raytracing
        // and output a bmp
        generateBmp();
        break;

    // control viewing (or camera)
    case 'w':
        // move up without changing reference point
        pos->x += v * up->x;
        pos->y += v * up->y;
        pos->z += v * up->z;

        look->x = center->x - pos->x;
        look->y = center->y - pos->y;
        look->z = center->z - pos->z;
        break;
    case 's':
        // move down without changing reference point
        pos->x -= v * up->x;
        pos->y -= v * up->y;
        pos->z -= v * up->z;

        look->x = center->x - pos->x;
        look->y = center->y - pos->y;
        look->z = center->z - pos->z;
        break;
    case 'a':
        // rotate the object in the clockwise direction about its own axis
        // pos->x += v * (-up->y*look->z);
        // pos->y += v * (look->x*up->y);

        // look->x = center->x - pos->x;
        // look->y = center->y - pos->y;
        // look->normalize();

        rotate_z -= 5;
        break;
    case 'd':
        // rotate the object in the anti-clockwise direction about its own axis
        // pos->x += v * (up->y*look->z);
        // pos->z += v * (-look->x*up->y);

        // look->x = center->x - pos->x;
        // look->z = center->z - pos->z;
        // look->normalize();

        rotate_z += 5;
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
        pos->x += look->x;
        pos->y += look->y;
        pos->z += look->z;
        break;
    case GLUT_KEY_DOWN:
        pos->x -= look->x;
        pos->y -= look->y;
        pos->z -= look->z;
        break;

    case GLUT_KEY_RIGHT:
        pos->x += r8->x;
        pos->y += r8->y;
        pos->z += r8->z;
        break;
    case GLUT_KEY_LEFT:
        pos->x -= r8->x;
        pos->y -= r8->y;
        pos->z -= r8->z;
        break;

    case GLUT_KEY_PAGE_UP:
        pos->x += up->x;
        pos->y += up->y;
        pos->z += up->z;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos->x -= up->x;
        pos->y -= up->y;
        pos->z -= up->z;
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
    clearMem();
    return 0;
}
