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

GLfloat eyex = 4, eyey = 4, eyez = 4;
GLfloat centerx = 0, centery = 0, centerz = 0;
GLfloat upx = 0, upy = 1, upz = 0;

GLfloat scale = 0;
GLfloat STEP_SCALE = 1.0/16.0;

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
    glColor3f(1, 0, 1);
    glTranslatef(scale, scale, scale);
    glScalef(1-scale, 1-scale, 1-scale);
    drawTriangle();
  glPopMatrix();

  glRotatef(90, 0, 1, 0);
  glPushMatrix();
    glColor3f(0, 1, 1);
    glTranslatef(scale, scale, scale);
    glScalef(1-scale, 1-scale, 1-scale);
    drawTriangle();
  glPopMatrix();

  glRotatef(90, 0, 1, 0);
  glPushMatrix();
    glColor3f(1, 0, 1);
    glTranslatef(scale, scale, scale);
    glScalef(1-scale, 1-scale, 1-scale);
    drawTriangle();
  glPopMatrix();

  glRotatef(90, 0, 1, 0);
  glPushMatrix();
    glColor3f(0, 1, 1);
    glTranslatef(scale, scale, scale);
    glScalef(1-scale, 1-scale, 1-scale);
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
Point **getSphereSide(int subdivision, int radius)
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

    // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
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

void drawSphereSide(int subdivision = 5, int radius = 1)
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

void drawSphere(int subdivision = 5, int radius = 1)
{
  glPushMatrix();
  for (int i = 0; i < 4; i++)
  {
    if (i % 2)
      glColor3f(0, 1, 0);
    else
      glColor3f(0, 0, 1);
    glRotatef(90, 0, 1, 0);
    drawSphereSide(subdivision, radius);
  }

  glColor3f(1, 0, 0);
  glRotatef(90, 0, 0, 1);
  drawSphereSide(subdivision, radius);
  glRotatef(180, 0, 0, 1);
  drawSphereSide(subdivision, radius);
  glPopMatrix();
}

/*  Handler for window-repaint event. Call back when the window first appears and
    whenever the window needs to be re-painted. */
void display()
{
  // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
  glLoadIdentity();           // Reset the model-view matrix

  // default arguments of gluLookAt
  // gluLookAt(0,0,0, 0,0,-100, 0,1,0);

  // control viewing (or camera)
  gluLookAt(eyex, eyey, eyez,
            centerx, centery, centerz,
            upx, upy, upz);
  // draw
  drawAxes();
  // drawSphere();
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
  double lx = centerx - eyex;
  double lz = centerz - eyez;
  double s;

  switch (key)
  {
  // control viewing (or camera)
  case 'w':
    eyey += v;
    break;
  case 's':
    eyey -= v;
    break;

  // control the transformation
  case ',': // to sphere
    if (scale < 1) scale += STEP_SCALE;
    break;
  case '.': // to octahedron
    if (scale > 0) scale -= STEP_SCALE;
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
  double v = 0.25;
  double lx = centerx - eyex;
  double lz = centerz - eyez;
  double s;
  switch (key)
  {
    // case GLUT_KEY_UP:		//down arrow key
    // 		pos.x+=l.x;
    // 		pos.y+=l.y;
    // 		pos.z+=l.z;
    // 		break;
    // 	case GLUT_KEY_DOWN:		// up arrow key
    // 		pos.x-=l.x;
    // 		pos.y-=l.y;
    // 		pos.z-=l.z;
    // 		break;

    // 	case GLUT_KEY_RIGHT:
    // 		pos.x+=r.x;
    // 		pos.y+=r.y;
    // 		pos.z+=r.z;
    // 		break;
    // 	case GLUT_KEY_LEFT :
    // 		pos.x-=r.x;
    // 		pos.y-=r.y;
    // 		pos.z-=r.z;
    // 		break;

    // 	case GLUT_KEY_PAGE_UP:
    //     pos.x+=u.x;
    // 		pos.y+=u.y;
    // 		pos.z+=u.z;
    // 		break;
    // 	case GLUT_KEY_PAGE_DOWN:
    //     pos.x-=u.x;
    // 		pos.y-=u.y;
    // 		pos.z-=u.z;
    // 		break;

  default:
    break;
  }

  glutPostRedisplay(); // Post a paint request to activate display()
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
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
