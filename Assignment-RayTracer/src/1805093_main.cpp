#include "1805093_def.hpp"
#include "1805093_utils.hpp"

int nearPlane, farPlane, fovY, fovX, aspectRatio;
int recursionLevel, imageWidth, imageHeight;
vector<Object *> objects;
vector<LightSource *> lights;

Point* pos; // position of the eye
Point* look;   // look/forward direction
Point* r8;   // r8 direction - dynamically updated in the display function
Point* up;   // up direction
Point* center;   // center of the scene - temp use

boolean showTexture = false;
vector< vector<Color*> > whiteTileColorBuffer;
vector< vector<Color*> > blackTileColorBuffer;

void init()
{
    pos = new Point(0, 100, 100);
    look = new Point(0, -1, -1);
    look->normalize();
    r8 = new Point(-20, 0, 0);
    r8->normalize();
    up = r8->cross(look);
    center = new Point();

    getInputs();
    getTextureInputs(whiteTileColorBuffer, blackTileColorBuffer);

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
        delete object;

    for (LightSource *light : lights)
        delete light;

    for (vector<Color*> row : whiteTileColorBuffer)
        for (Color* color : row)
            delete color;

    for (vector<Color*> row : blackTileColorBuffer)
        for (Color* color : row)
            delete color;
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

/*  Handler for window-repaint event. Call back when the window first appears and
    whenever the window needs to be re-painted. */
void display()
{
    // update r8, up, look
    r8 = look->cross(up);
    r8->normalize();
    up = r8->cross(look);
    up->normalize();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(pos->x, pos->y, pos->z,
              pos->x + look->x, pos->y + look->y, pos->z + look->z,
              up->x, up->y, up->z);

    // draw
    drawAxes();
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
        look->normalize();

        break;

    case 's':
        // move down without changing reference point
        pos->x -= v * up->x;
        pos->y -= v * up->y;
        pos->z -= v * up->z;

        look->x = center->x - pos->x;
        look->y = center->y - pos->y;
        look->z = center->z - pos->z;
        look->normalize();

        break;

    case 'a':
        // rotate the object in the clockwise direction about its own axis
        pos->x += v * (-up->y*look->y);
        pos->y += v * (look->x*up->y);

        look->x = center->x - pos->x;
        look->y = center->y - pos->y;
        look->normalize();

        break;

    case 'd':
        // rotate the object in the anti-clockwise direction about its own axis
        pos->x += v * (up->y*look->y);
        pos->y += v * (-look->x*up->y);

        look->x = center->x - pos->x;
        look->y = center->y - pos->y;
        look->normalize();

        break;

    case ' ':
        // toggle texture mode
        showTexture = !showTexture;
        if (showTexture)
            cout << "Texture mode ON" << endl;
        else
            cout << "Texture mode OFF" << endl;
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
    double v = 2;

    switch (key)
    {
    case GLUT_KEY_UP:
        pos->x += look->x * 2;
        pos->y += look->y * 2;
        pos->z += look->z * 2;
        break;
    case GLUT_KEY_DOWN:
        pos->x -= look->x * 2;
        pos->y -= look->y * 2;
        pos->z -= look->z * 2;
        break;

    case GLUT_KEY_RIGHT:
        pos->x += r8->x * 2;
        pos->y += r8->y * 2;
        pos->z += r8->z * 2;
        break;
    case GLUT_KEY_LEFT:
        pos->x -= r8->x * 2;
        pos->y -= r8->y * 2;
        pos->z -= r8->z * 2;
        break;

    case GLUT_KEY_PAGE_UP:
        pos->x += up->x * 2;
        pos->y += up->y * 2;
        pos->z += up->z * 2;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos->x -= up->x * 2;
        pos->y -= up->y * 2;
        pos->z -= up->z * 2;
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
    glutInitWindowSize(650, 650);
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
