#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <cmath>

using namespace std;

void initGL()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
  glEnable(GL_DEPTH_TEST);              // Enable depth testing for z-culling
}

typedef struct Point
{
  GLfloat x, y, z;
} Point;

// GLfloat eyex = 4, eyey = 4, eyez = 4;
// GLfloat centerx = 0, centery = 0, centerz = 0;
// GLfloat upx = 0, upy = 1, upz = 0;

Point pos; // position of the eye
Point l;   // look/forward direction
Point r;   // right direction - temp use
Point u;   // up direction
Point c;   // center of the scene - temp use

GLfloat scale = 0;
GLfloat STEP_SCALE = 1.0 / 16.0;

GLint rotate_y = 0;

void drawAxes()
{
  glLineWidth(3);
  glBegin(GL_LINES);
  glColor3f(2, 0, 0); // Red
  // X axis
  glVertex3f(0, 0, 0);
  glVertex3f(2, 0, 0);

  glColor3f(0, 2, 0); // Green
  // Y axis
  glVertex3f(0, 0, 0);
  glVertex3f(0, 2, 0);

  glColor3f(0, 0, 2); // Blue
  // Z axis
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, 2);
  glEnd();
}

void drawTriangle()
{
  glPushMatrix();
  glBegin(GL_TRIANGLES);
  glVertex3f(1, 0, 0);
  glVertex3f(0, 1, 0);
  glVertex3f(0, 0, 1);
  glEnd();
  glPopMatrix();
}

void drawPyramid()
{
  glPushMatrix();
  glColor3f(0, 1, 1); // cyan
  glTranslatef(scale / 3, scale / 3, scale / 3);
  glScalef(1 - scale, 1 - scale, 1 - scale);
  drawTriangle();
  glPopMatrix();

  glRotatef(90, 0, 1, 0);
  glPushMatrix();
  glColor3f(1, 0, 1); // magenta
  glTranslatef(scale / 3, scale / 3, scale / 3);
  glScalef(1 - scale, 1 - scale, 1 - scale);
  drawTriangle();
  glPopMatrix();

  glRotatef(90, 0, 1, 0);
  glPushMatrix();
  glColor3f(0, 1, 1); // cyan
  glTranslatef(scale / 3, scale / 3, scale / 3);
  glScalef(1 - scale, 1 - scale, 1 - scale);
  drawTriangle();
  glPopMatrix();

  glRotatef(90, 0, 1, 0);
  glPushMatrix();
  glColor3f(1, 0, 1); // magenta
  glTranslatef(scale / 3, scale / 3, scale / 3);
  glScalef(1 - scale, 1 - scale, 1 - scale);
  drawTriangle();
  glPopMatrix();
}

void drawOctahedron()
{
  // upper pyramid
  drawPyramid();

  // lower pyramid
  glPushMatrix();
  glRotatef(180, 0, 0, 1);
  glRotatef(90, 0, 1, 0); // this is just for color consistency
  drawPyramid();
  glPopMatrix();
}

// generate vertices for +X face only by intersecting 2 circular planes
// (longitudinal and latitudinal) at the given longitude/latitude angles
Point **getSphereSide(int subdivision, float radius)
{
  float n1[3]; // normal of longitudinal plane rotating along Y-axis
  float n2[3]; // normal of latitudinal plane rotating along Z-axis
  float v[3];  // direction vector intersecting 2 planes, n1 x n2
  float a1;    // longitudinal angle along Y-axis
  float a2;    // latitudinal angle along Z-axis

  // compute the number of vertices per row, 2^n + 1
  int pointsPerRow = (int)pow(2, subdivision) + 1;
  Point **points = new Point *[pointsPerRow];
  for (int i = 0; i < pointsPerRow; ++i)
    points[i] = new Point[pointsPerRow];

  // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
  for (unsigned int i = 0; i < pointsPerRow; ++i)
  {
    // normal for latitudinal plane
    // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
    // therefore, it is rotating (0,1,0) vector by latitude angle a2
    a2 = M_PI / 4 - (M_PI / 2 * i / (pointsPerRow - 1));
    n2[0] = -sin(a2);
    n2[1] = cos(a2);
    n2[2] = 0;

    // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-r.)
    for (unsigned int j = 0; j < pointsPerRow; ++j)
    {
      // normal for longitudinal plane
      // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
      // therefore, it is rotating (0,0,-1) vector by longitude angle a1
      a1 = -M_PI / 4 + (M_PI / 2 * j / (pointsPerRow - 1));
      n1[0] = -sin(a1);
      n1[1] = 0;
      n1[2] = -cos(a1);

      // find direction vector of intersected line, n1 x n2
      v[0] = n1[1] * n2[2] - n1[2] * n2[1];
      v[1] = n1[2] * n2[0] - n1[0] * n2[2];
      v[2] = n1[0] * n2[1] - n1[1] * n2[0];

      // normalize direction vector
      float scale = 1 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
      v[0] *= scale;
      v[1] *= scale;
      v[2] *= scale;

      // add a vertex into array
      points[i][j].x = radius * v[0];
      points[i][j].y = radius * v[1];
      points[i][j].z = radius * v[2];
    }
  }
  return points;
}

void drawSphereSide(int subdivision = 5, float radius = 1)
{
  Point **points = getSphereSide(subdivision, radius);
  int pointsPerRow = (int)pow(2, subdivision) + 1;

  // draw the sphere
  glBegin(GL_QUADS);
  for (unsigned int i = 0; i < pointsPerRow - 1; ++i)
  {
    for (unsigned int j = 0; j < pointsPerRow - 1; ++j)
    {
      // draw a quad by 4 vertices
      glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
      glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
      glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
      glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
    }
  }
  glEnd();
}

void drawSphere(int subdivision = 5, float radius = 1 / sqrt(3)) // as the circle should touch the octahedron at (1/3, 1/3, 1/3)
{
  glPushMatrix();
  glPushMatrix();
  glColor3f(0, 1, 0); // green
  glTranslatef(1 - scale, 0, 0);
  glScalef(scale, scale, scale);
  drawSphereSide(subdivision, radius);
  glPopMatrix();

  glRotatef(90, 0, 1, 0);
  glPushMatrix();
  glColor3f(0, 0, 1); // blue
  glTranslatef(1 - scale, 0, 0);
  glScalef(scale, scale, scale);
  drawSphereSide(subdivision, radius);
  glPopMatrix();

  glRotatef(90, 0, 1, 0);
  glPushMatrix();
  glColor3f(0, 1, 0); // green
  glTranslatef(1 - scale, 0, 0);
  glScalef(scale, scale, scale);
  drawSphereSide(subdivision, radius);
  glPopMatrix();

  glRotatef(90, 0, 1, 0);
  glPushMatrix();
  glColor3f(0, 0, 1); // blue
  glTranslatef(1 - scale, 0, 0);
  glScalef(scale, scale, scale);
  drawSphereSide(subdivision, radius);
  glPopMatrix();

  glRotatef(90, 0, 0, 1);
  glPushMatrix();
  glColor3f(1, 0, 0); // red
  glTranslatef(1 - scale, 0, 0);
  glScalef(scale, scale, scale);
  drawSphereSide(subdivision, radius);
  glPopMatrix();

  glRotatef(180, 0, 0, 1);
  glPushMatrix();
  glColor3f(1, 0, 0); // red
  glTranslatef(1 - scale, 0, 0);
  glScalef(scale, scale, scale);
  drawSphereSide(subdivision, radius);
  glPopMatrix();
  glPopMatrix();
}

// draw a cylinder from +angle to -angle
void drawCylinder(float angle = (M_PI - acos(-1.0/3.0))/2.0, float radius = 1 / sqrt(3),
                  float height = sqrt(2)) // these values come from spec & geometry
{
  float angle_stepsize = 0.1;
  int segments = (int)(angle * 2 / angle_stepsize) + 1;

  height *= (1 - scale);
  radius *= scale;

  // Draw the tube
  glPushMatrix();
  glTranslatef(1 / sqrt(2) * (1 - scale), 0, 0);
  glTranslatef(0, 0, -height / 2);
  glRotatef(-angle * 180 / M_PI, 0, 0, 1);
  glBegin(GL_QUAD_STRIP);
  for (int i = 0; i <= segments; i++)
  {
    float x = radius * cos(i * angle_stepsize);
    float y = radius * sin(i * angle_stepsize);
    float z = 0;
    glVertex3f(x, y, z);
    glVertex3f(x, y, z + height);
  }
  glEnd();
  glPopMatrix();
}

void drawCylinders()
{
  glPushMatrix();
  glColor3f(1, 1, 0); // yellow

  // middle 4 cylinders
  glRotatef(-45, 0, 1, 0);
  for (int i = 0; i < 4; i++)
  {
    drawCylinder();
    glRotatef(90, 0, 1, 0);
  }

  // left
  glRotatef(90, 1, 0, 1);
  for (int i = 0; i < 4; i++)
  {
    drawCylinder();
    glRotatef(90, 0, 1, 0);
  }

  // r.
  glRotatef(90, 1, 0, -1);
  for (int i = 0; i < 4; i++)
  {
    drawCylinder();
    glRotatef(90, 0, 1, 0);
  }

  glPopMatrix();
}

/*  Handler for window-repaint event. Call back when the window first appears and
    whenever the window needs to be re-painted. */
void display()
{
  // r = (l-pos) x u
  r.x = (l.y - pos.y) * u.z - (l.z - pos.z) * u.y;
  r.y = (l.z - pos.z) * u.x - (l.x - pos.x) * u.z;
  r.z = (l.x - pos.x) * u.y - (l.y - pos.y) * u.x;

  // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
  glLoadIdentity();           // Reset the model-view matrix

  // default arguments of gluLookAt
  // gluLookAt(0,0,0, 0,0,-100, 0,1,0);

  // control viewing (or camera)
  // gluLookAt(eyex, eyey, eyez,
  //           centerx, centery, centerz,
  //           upx, upy, upz);
  gluLookAt(pos.x, pos.y, pos.z,
            pos.x + l.x, pos.y + l.y, pos.z + l.z,
            u.x, u.y, u.z);

  // rotation for a, d buttons
  glRotatef(rotate_y, 0, 1, 0);
  // draw
  // drawAxes();
  drawSphere();
  drawCylinders();
  drawOctahedron();

  glutSwapBuffers(); // Render now
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshapeListener(GLsizei width, GLsizei height)
{ // GLsizei for non-negative integer
  // Compute aspect ratio of the new window
  if (height == 0)
    height = 1; // To prevent divide by 0
  GLfloat aspect = (GLfloat)width / (GLfloat)height;

  // Set the viewport to cover the new window
  glViewport(0, 0, width, height);

  // Set the aspect ratio of the clipping area to match the viewport
  glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
  glLoadIdentity();            // Reset the projection matrix
  /*if (width >= height) {
      // aspect >= 1, set the height from -1 to 1, with larger width
      gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
  } else {
      // aspect < 1, set the width to -1 to 1, with larger height
      gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
  }*/
  // Enable perspective projection with fovy, aspect, zNear and zFar
  gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y)
{
  double v = 0.25;
  // float lengthl = sqrt(l.x * l.x + l.y * l.y + l.z * l.z);
  // float lengthr = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
  // float lengthu = sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
  double rate = 0.01;
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
    // pos.z += v * (l.x*u.y);

    // l.x = c.x - pos.x;
    // l.z = c.z - pos.z;

    rotate_y -= 5;
    break;
  case 'd':
    // rotate the object in the anti-clockwise direction about its own axis
    // pos.x += v * (u.y*l.z);
    // pos.z += v * (-l.x*u.y);

    // l.x = c.x - pos.x;
    // l.z = c.z - pos.z;

    rotate_y += 5;
    break;

  // control the transformation
  case ',': // to sphere
    if (scale < 1)
      scale += STEP_SCALE;
    break;
  case '.': // to octahedron
    if (scale > 0)
      scale -= STEP_SCALE;
    break;

  // control exit
  case 27:   // ESC key
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
  float length;
  switch (key)
  {
    case GLUT_KEY_UP:
    		pos.x+=l.x;
    		pos.y+=l.y;
    		pos.z+=l.z;
    		break;
    	case GLUT_KEY_DOWN:
    		pos.x-=l.x;
    		pos.y-=l.y;
    		pos.z-=l.z;
    		break;

    	case GLUT_KEY_RIGHT:
    		pos.x+=r.x;
    		pos.y+=r.y;
    		pos.z+=r.z;
    		break;
    	case GLUT_KEY_LEFT:
    		pos.x-=r.x;
    		pos.y-=r.y;
    		pos.z-=r.z;
    		break;

    	case GLUT_KEY_PAGE_UP:
        pos.x+=u.x;
    		pos.y+=u.y;
    		pos.z+=u.z;
    		break;
    	case GLUT_KEY_PAGE_DOWN:
        pos.x-=u.x;
    		pos.y-=u.y;
    		pos.z-=u.z;
    		break;

  default:
    break;
  }

  glutPostRedisplay(); // Post a paint request to activate display()
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
  pos.x = 2;
  pos.y = 2;
  pos.z = 2;
  l.x = -2;
  l.y = -2;
  l.z = -2;
  u.x = 0;
  u.y = 1;
  u.z = 0;
  // r.x = 1;
  // r.y = 0;
  // r.z = 0;

  glutInit(&argc, argv);                                    // Initialize GLUT
  glutInitWindowSize(640, 640);                             // Set the window's initial width & height
  glutInitWindowPosition(50, 50);                           // Position the window's initial top-left corner
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color
  glutCreateWindow("OpenGL 3D Drawing");                    // Create a window with the given title
  glutDisplayFunc(display);                                 // Register display callback handler for window re-paint
  glutReshapeFunc(reshapeListener);                         // Register callback handler for window re-shape
  glutKeyboardFunc(keyboardListener);                       // Register callback handler for normal-key event
  glutSpecialFunc(specialKeyListener);                      // Register callback handler for special-key event
  initGL();                                                 // Our own OpenGL initialization
  glutMainLoop();                                           // Enter the event-processing loop
  return 0;
}
