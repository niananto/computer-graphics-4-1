#define _USE_MATH_DEFINES

#include <windows.h>  // For MS Windows
#include <GL/glut.h>  // GLUT, includes glu.h and gl.h
#include <cmath>

GLdouble px = 0, py = 0;

/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer

    glPointSize(5);
    glBegin(GL_POINTS);
        glVertex2d(px, py);
    glEnd();

    glFlush();  // Render now
}

/* Callback handler for mouse event */
void mouse(int button, int state, int x, int y) {
    // (x, y) is the mouse-click location.
    // button: GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON, GLUT_MIDDLE_BUTTON
    // state: GLUT_UP, GLUT_DOWN
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        px = -1.0 + x * 2.0 / 640.0;
        py = 1.0 - y * 2.0 / 640.0;
        glutPostRedisplay();
    }
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    glutInit(&argc, argv);                  // Initialize GLUT
    glutInitWindowSize(640, 640);           // Set the window's initial width & height
    glutInitWindowPosition(50, 50);         // Position the window's initial top-left corner
    glutCreateWindow("OpenGL 2D Drawing");  // Create a window with the given title
    glutDisplayFunc(display);               // Register display callback handler for window re-paint
    glutMouseFunc(mouse);                   // Register callback handler for mouse event
    glutMainLoop();                         // Enter the infinitely event-processing loop
    return 0;
}
