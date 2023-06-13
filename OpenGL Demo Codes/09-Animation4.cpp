#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h

/* Initialize OpenGL Graphics */
void initGL() {
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Black and opaque
}

// Global variables
GLfloat rot_a_1 = 0;    // Rotational angle of Yellow square
GLfloat rot_v_1 = 0.05; // Rotational speed of Yellow square

GLfloat rot_a_2 = 0;    // Rotational angle of Cyan square
GLfloat rot_v_2 = 0.1;  // Rotational speed of Cyan square

GLfloat rot_a_3 = 0;    // Rotational angle of Pink square
GLfloat rot_v_3 = 0.2;  // Rotational speed of Pink square

GLfloat rev_d_2 = 0.5;  // Distance between Cyan square and Yellow square
GLfloat rev_a_2 = 0;    // Revolution angle of Cyan square around Yellow square
GLfloat rev_v_2 = 0.1;  // Revolution speed of Cyan square around Yellow square

GLfloat rev_d_3 = 0.2;  // Distance between Pink square and Cyan square
GLfloat rev_a_3 = 0;    // Revolution angle of Pink square around Cyan square
GLfloat rev_v_3 = 0.5;  // Revolution speed of Pink square around Cyan square

int refreshMillis = 2;  // Refresh period in milliseconds
bool play = true;       // Play or Pause?

// Draw a 1x1 Square
void drawSquare() {
    glBegin(GL_QUADS);
        glVertex2f(-0.5,-0.5);
        glVertex2f(-0.5, 0.5);
        glVertex2f( 0.5, 0.5);
        glVertex2f( 0.5,-0.5);
    glEnd();
}

/*  Handler for window-repaint event. Call back when the window first appears and
    whenever the window needs to be re-painted. */
void display() {
    glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glMatrixMode(GL_MODELVIEW);             // To operate on Model-View matrix
    glLoadIdentity();                       // Reset the model-view matrix

    // Draw Yellow Square
    glPushMatrix();
        glRotatef(rot_a_1, 0,0,1);  // Align the square for Rotation about it's own axis
        glScalef(0.2,0.2,1);    // Scale down the square
        glColor3f(1,1,0);   // Yellow
        drawSquare();
    glPopMatrix();

    // Draw Cyan Square
    glRotatef(rev_a_2, 0,0,1);  // Align everything for Revolution about Cyan Square
    glTranslatef(rev_d_2,0,0);  // Translate the Cyan Square by it's distance from Yellow Square
    glPushMatrix();
        glRotatef(rot_a_2, 0,0,1);  // Align the square for Rotation about it's own axis
        glScalef(0.1,0.1,1);    // Scale down the square
        glColor3f(0,1,1);   // Cyan
        drawSquare();
    glPopMatrix();

    // Draw Pink Square
    glRotatef(rev_a_3, 0,0,1);  // Align everything for Revolution about Cyan Square
    glTranslatef(rev_d_3,0,0);  // Translate the Pink Square by it's distance from Cyan Square
    glPushMatrix();
        glRotatef(rot_a_3, 0,0,1);  // Align the square for Rotation about it's own axis
        glScalef(0.05,0.05,1);  // Scale down the square
        glColor3f(1,0,1);   // Pink
        drawSquare();
    glPopMatrix();

    glutSwapBuffers();      // Swap front and back buffers (of double buffered mode)

    // Update angles by adding speed
    rot_a_1 += rot_v_1;
    rot_a_2 += rot_v_2;
    rot_a_3 += rot_v_3;

    rev_a_2 += rev_v_2;
    rev_a_3 += rev_v_3;
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
    if (width >= height) {
        // aspect >= 1, set the height from -1 to 1, with larger width
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        // aspect < 1, set the width to -1 to 1, with larger height
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }
}

/* Called back when there is no other event to be handled */
void idle() {
    glutPostRedisplay();    // Post a re-paint request to activate display()
}

/* Called back when timer expired */
void timer(int value) {
    glutPostRedisplay();    // Post re-paint request to activate display()
    if (play)   // Call next 'timer' if currently in 'play state'
        glutTimerFunc(refreshMillis, timer, 0); // Call next 'timer' milliseconds later
}

/* Callback handler for normal-key event */
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case ' ':
        play = !play;   // Toggle play/pause state
        if (play)   // Call 'timer' if currently in 'play state'
            glutTimerFunc(0, timer, 0); // Call 'timer' immediately
        break;
    case 27:    // ESC key
        exit(0);    // Exit window
        break;
    }
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    glutInit(&argc, argv);                  // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE);       // Enable double buffered mode
    glutInitWindowSize(640, 640);           // Set the window's initial width & height
    glutInitWindowPosition(50, 50);         // Position the window's initial top-left corner
    glutCreateWindow("Animation 3");          // Create a window with the given title
    glutDisplayFunc(display);               // Register display callback handler for window re-paint
    glutReshapeFunc(reshape);               // Register callback handler for window re-shape
    // glutIdleFunc(idle);                     // Register callback handler if no other event
    glutTimerFunc(0, timer, 0);             // First timer call immediately
    glutKeyboardFunc(keyboard);             // Register callback handler for normal-key event
    initGL();                               // Our own OpenGL initialization
    glutMainLoop();                         // Enter the event-processing loop
    return 0;
}
