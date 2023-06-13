#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <cmath>

/* Initialize OpenGL Graphics */
void initGL() {
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
}

/* Draw a Red Square centered at the origin */
void drawSquare() {
    glBegin(GL_QUADS);  // Each set of 4 vertices form a quad
        glColor3f(1.0f,0.0f,0.0f);  // Red

        glVertex2f( 0.1f, 0.1f);
        glVertex2f(-0.1f, 0.1f);
        glVertex2f(-0.1f,-0.1f);
        glVertex2f( 0.1f,-0.1f);
    glEnd();
}

/* Draw a Green Triangle centered at the origin */
void drawTriangle() {
    glBegin(GL_TRIANGLES);  // Each set of 3 vertices form a triangle
        glColor3f(0.0f,1.0f,0.0f);  //Green

        glVertex2f( 0.00f, 0.1f);
        glVertex2f( 0.1f,-0.1f);
        glVertex2f(-0.1f,-0.1f);
    glEnd();
}

/*  Draw a Plus Shape with 4 Red Squares
    arranged on four sides 0.2 units away from center */
void drawPlusWithSquare() {
    glPushMatrix(); // Create a new scope
        glTranslatef( 0.2, 0.0,0);  // Translate the square 0.2 units right
        drawSquare();
    glPopMatrix();

    glPushMatrix(); // Create a new scope
        glTranslatef(-0.2, 0.0,0);  // Translate the square 0.2 units left
        drawSquare();
    glPopMatrix();

    glPushMatrix(); // Create a new scope
        glTranslatef( 0.0, 0.2,0);  // Translate the square 0.2 units up
        drawSquare();
    glPopMatrix();

    glPushMatrix(); // Create a new scope
        glTranslatef( 0.0,-0.2,0);  // Translate the square 0.2 units down
        drawSquare();
    glPopMatrix();
}

/*  Draw a Plus Shape with 4 Green Triangles
    arranged on four sides 0.2 units away from center */
void drawPlusWithTriangle() {
    glPushMatrix(); // Create a new scope
        // Positive-y is Up
        glTranslatef(0.0,0.2,0.0);
        drawTriangle(); // Triangle is drawn upright, 0.2 units above origin
    glPopMatrix();

    glPushMatrix(); // Create a new scope
        glRotatef(90, 0,0,1);
        // Positive-y is Left
        glTranslatef(0.0,0.2,0.0);
        drawTriangle(); // Triangle is drawn rotated 90 degrees anti-clockwise (pointing left), 0.2 units to the left of origin
    glPopMatrix();

    glPushMatrix(); // Create a new scope
        glRotatef(180, 0,0,1);
        // Positive-y is Down
        glTranslatef(0.0,0.2,0.0);
        drawTriangle(); // Triangle is drawn rotated 180 degrees anti-clockwise (downwards), 0.2 units down from origin
    glPopMatrix();

    glPushMatrix(); // Create a new scope
        glRotatef(270, 0,0,1);
        // Positive-y is Right
        glTranslatef(0.0,0.2,0.0);
        drawTriangle(); // Triangle is drawn rotated 270 degrees anti-clockwise (pointing right), 0.2 units to the right of origin
    glPopMatrix();
}

/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
    glClear(GL_COLOR_BUFFER_BIT);   // Clear the color buffer with current clearing color
    glMatrixMode(GL_MODELVIEW);     // To operate on Model-View matrix
    glLoadIdentity();               // Reset the model-view matrix

    // Middle
    glPushMatrix(); // Create a new scope
        drawSquare();   // Draw a Red Square centered at origin

        glTranslatef(0.2,0.0,0.0);
        drawTriangle(); // Draw a Green Triangle centered at 0.2 units to the right of origin (to the right of the Red Square)
    glPopMatrix();

    // Top-Left
    glPushMatrix(); // Create a new scope
        glTranslatef(-0.6,0.6,0.0); // Translate everything in this scope to top-left

        drawSquare();   // Draw a Red Square

        glTranslatef(0.2,0.0,0.0);
        drawTriangle(); // Draw a Green Triangle to the right of the Red Square
    glPopMatrix();

    // Top-Middle
    glPushMatrix(); // Create a new scope
        glTranslatef( 0.0,0.6,0.0); // Translate everything in this scope to top-middle
        glRotatef(45,0,0,1); // Rotate everything in this scope by 90 degrees anti-clockwise

        drawSquare();   // Draw a Red Square

        glTranslatef(0.2,0.0,0.0);
        drawTriangle(); // Draw a Green Triangle to the right of the Red Square
    glPopMatrix();

    // Top-Right
    glPushMatrix(); // Create a new scope
        glTranslatef( 0.6,0.6,0.0); // Translate everything in this scope to top-right

        glPushMatrix(); // Create a new nested scope
            glRotatef(-30,0,0,1);
            drawSquare();   // Draw a Red Square rotated 30 degrees clockwise
        glPopMatrix();

        glTranslatef(0.2,0.0,0.0);
        drawTriangle(); // Draw a Green Triangle at the usual location; not rotated like the Red Square
    glPopMatrix();

    // Left-Middle
    glPushMatrix(); // Create a new scope
        glTranslatef(-0.6,0.0,0.0); // Translate everything in this scope to left-middle
        glScalef(1,0.5,1);  // Scale everything in this scope to half only on y-direction

        drawSquare();   // Draw a Red Square

        glTranslatef(0.2,0.0,0.0);
        drawTriangle(); // Draw a Green Triangle to the right of the Red Square
    glPopMatrix();

    // Right-Middle
    glPushMatrix(); // Create a new scope
        glTranslatef( 0.6,0.0,0.0); // Translate everything in this scope to right-middle

        drawSquare();   // Draw a Red Square

        glTranslatef(0.2,0.0,0.0);
        glScalef(0.5,0.5,1);
        drawTriangle(); // Draw a Green Triangle to the right of the Red Square; scaled half on both x and y direction
    glPopMatrix();

    // Bottom-Left
    glPushMatrix(); // Create a new scope
        glTranslatef(-0.6,-0.6,0.0);    // Translate everything in this scope to bottom-left

        drawPlusWithSquare();   // Draw plus shape with squares
    glPopMatrix();

    // Bottom-Middle
    glPushMatrix(); // Create a new scope
        glTranslatef( 0.0,-0.6,0.0);    // Translate everything in this scope to bottom-middle
        glRotatef(45, 0,0,1);   // Rotate everything in this scope by 455 degrees anti-clockwise

        drawPlusWithTriangle(); // Draw plus shape with triangles. 45 degrees rotation make it look like cross
    glPopMatrix();

    // Bottom-Right
    glPushMatrix(); // Create a new scope
        glTranslatef( 0.6,-0.6,0.0);    // Translate everything in this scope to bottom-right

        drawPlusWithSquare();   // Draw plus shape with squares

        float scale = sqrt(2.0)/3.0;
        glScalef(scale, scale, 1);  // Scale everything later in this scope by sqrt(2)/3 on both axis

        glRotatef(45, 0,0,1);   // Rotate everything later in this scope by 45 degrees anti-clockwise

        drawPlusWithTriangle(); // Draw a cross shape with Green Triangles centered at the origin

        glPushMatrix();
            glTranslatef( 0.6, 0.0,0);
            drawPlusWithTriangle(); // Draw a cross shape with Green Triangles at top-right position of Red Plus
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-0.6, 0.0,0);
            drawPlusWithTriangle(); // Draw a cross shape with Green Triangles at bottom-left position of Red Plus
        glPopMatrix();

        glPushMatrix();
            glTranslatef( 0.0, 0.6,0);
            drawPlusWithTriangle(); // Draw a cross shape with Green Triangles at top-left position of Red Plus
        glPopMatrix();

        glPushMatrix();
            glTranslatef( 0.0,-0.6,0);
            drawPlusWithTriangle(); // Draw a cross shape with Green Triangles at bottom-right position of Red Plus
        glPopMatrix();
    glPopMatrix();

    glFlush();  // Render now
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

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
    glutInit(&argc, argv);                          // Initialize GLUT
    glutInitWindowSize(640, 640);                   // Set the window's initial width & height - non-square
    glutInitWindowPosition(50, 50);                 // Position the window's initial top-left corner
    glutCreateWindow("Model-View Transformation");  // Create window with the given title
    glutDisplayFunc(display);                       // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);                       // Register callback handler for window re-size event
    initGL();                                       // Our own OpenGL initialization
    glutMainLoop();                                 // Enter the infinite event-processing loop
    return 0;
}
