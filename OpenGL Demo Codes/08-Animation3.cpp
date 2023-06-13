#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h

/* Initialize OpenGL Graphics */
void initGL() {
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Black and opaque
}

// Global variables
GLfloat rot_a = 0;  // Rotational angle of the triangle
GLfloat rot_v = 0.1;    // Rotational speed of the triangle

GLfloat tr_x = 0;   // Translation distance for left-right movement
GLfloat tr_v = 0.001;   // Translation speed for left-right movement

GLfloat rev_a = 0;  // Revolution angle of the square
GLfloat rev_v = 0.5;    // Revolution speed of the dquare

int refreshMillis = 1;  // Refresh period in milliseconds
bool play = true;       // Play or Pause?

/*  Handler for window-repaint event. Call back when the window first appears and
    whenever the window needs to be re-painted. */
void display() {
    glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glMatrixMode(GL_MODELVIEW);             // To operate on Model-View matrix
    glLoadIdentity();                       // Reset the model-view matrix

    glTranslatef(tr_x,0,0); // Translate everything for left-right movement

    // Draw the Triangle
    glPushMatrix();
        glRotatef(rot_a, 0,0,1);    // Align the triangle for Rotation about it's own axis
        glScalef(0.5,0.5,1);    // Scale down the Triangle
        // Draw a rainbow Triangle
        glBegin(GL_TRIANGLES);                      // Each set of 3 vertices form a triangle
            glColor3f(1.0f, 0.0f, 0.0f);        // Red
            glVertex2f( 0.0f, 0.3f);            // Top vertex
            glColor3f(0.0f,1.0f,0.0f);          // Green
            glVertex2f( 0.3f, -0.3f);           // Right vertex
            glColor3f(0.0f,0.0f,1.0f);          // Blue
            glVertex2f(-0.3f, -0.3f);           // Left vertex
        glEnd();
    glPopMatrix();

    // Draw the Square
    glRotatef(rev_a, 0,0,1);    // Align everything for Revolution of the Square about the Triangle
    glTranslatef(0.6,0,0);  // Translate everything by distance between the Triangle and the Square
    glPushMatrix();
        glScalef(0.25,0.25,1);  // Scale down the Square
        // Draw a red Square
        glBegin(GL_QUADS);
            glColor3f(1,0,0);   // Red
            glVertex2f(-0.5,0.5);
            glVertex2f(0.5,0.5);
            glVertex2f(0.5,-0.5);
            glVertex2f(-0.5,-0.5);
        glEnd();
    glPopMatrix();

    glutSwapBuffers();      // Swap front and back buffers (of double buffered mode)

    tr_x += tr_v;   // Change the x-coordinate by velocity after each display()
    if (tr_x > 0.5 || tr_x <-0.5) { // Check if outside of some range
        tr_v = -tr_v;   // Reverse direction of movement
    }

    rot_a += rot_v;
    rev_a += rev_v;
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
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(640, 640);           // Set the window's initial width & height
    glutInitWindowPosition(50, 50);         // Position the window's initial top-left corner
    glutCreateWindow("Animation 2");  // Create a window with the given title
    glutDisplayFunc(display);               // Register display callback handler for window re-paint
    glutReshapeFunc(reshape);               // Register callback handler for window re-shape
    // glutIdleFunc(idle);                     // Register callback handler if no other event
    glutTimerFunc(0, timer, 0);             // First timer call immediately
    glutKeyboardFunc(keyboard);             // Register callback handler for normal-key event
    initGL();                               // Our own OpenGL initialization
    glutMainLoop();                         // Enter the event-processing loop
    return 0;
}
