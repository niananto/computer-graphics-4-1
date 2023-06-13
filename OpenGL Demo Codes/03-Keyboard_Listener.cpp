#include <iostream>
#include <GL/glut.h>

GLdouble px = 0, py = 0;

/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(10);
    glBegin(GL_POINTS);
        glColor3d(1,0,0);
        glVertex2d(px, py);
    glEnd();

    glFlush();
}

/* Callback handler for normal-key event */
void keyboard(unsigned char key, int x, int y) {
    // key is the char pressed, e.g., 'a' or 27 for ESC
    // (x, y) is the mouse location in Windows' coordinates
    switch (key) {
    case 'a':
        px -= 0.1;
        break;
    case 'd':
        px += 0.1;
    
    default:
        break;
    }
    glutPostRedisplay();
}

/* Callback handler for special-key event */
void special(int key, int x, int y) {
    // specialKey: GLUT_KEY_* (* for LEFT, RIGHT, UP, DOWN, HOME, END, PAGE_UP, PAGE_DOWN, F1,...F12). 
    // (x, y) is the mouse location in Windows' coordinates
    switch(key) {
    case GLUT_KEY_LEFT:
        px -= 0.1;
        break;
    case GLUT_KEY_RIGHT:
        px += 0.1;
        break;
    }
    glutPostRedisplay();
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(640, 640);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("OpenGL Keyboard + Special key Listener");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}