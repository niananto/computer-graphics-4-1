#include <iostream>
#include <GL/glut.h>

GLdouble px = 0, py = 0; // Position of the red point
GLdouble vx = 0.001;    // Current velocity of the red point

/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(10);
    glBegin(GL_POINTS);
        glColor3d(1, 0, 0);
        glVertex2d(px, py);
    glEnd();

    glFlush();
}

/* Called back when there is no other event to be handled */
void idle() {
    px += vx;
    if (px >= 1 || px <= -1) // Reverse direction when point is about to pass bounding box [-1,1]
        vx = -vx;
    glutPostRedisplay();
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    glutInit(&argc, argv);                  // Initialize GLUT
    glutInitWindowSize(640, 640);           // Set the window's initial width & height
    glutInitWindowPosition(100, 50);        // Position the window's initial top-left corner
    glutCreateWindow("OpenGL Animation with Idle Function");  // Create a window with the given title
    glutDisplayFunc(display);               // Register display callback handler for window re-paint
    glutIdleFunc(idle);                     // Register callback handler if no other event
    glutMainLoop();                         // Enter the infinitely event-processing loop
    return 0;
}