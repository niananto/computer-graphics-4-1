#define _USE_MATH_DEFINES

#include <windows.h>  // For MS Windows
#include <GL/glut.h>  // GLUT, includes glu.h and gl.h
#include <cmath>

/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer

    // Draw two points
    // at 'top-left'
    glPointSize(5); // Make next points look bigger
    glBegin(GL_POINTS); // Each vertex is drawn as a single pixel
        glColor3f(1.0f,0.0f,1.0f); // Magenta
        glVertex2d(-0.6f, 0.8f);

        glColor3f(0.0f,1.0f,1.0f);  // Cyan
        glVertex2d(-0.8f, 0.6f);
    glEnd();

    // Draw two lines
    // at 'top-middle'
    glLineWidth(5); // Make next lines wider
    glBegin(GL_LINES);  // Each set of 2 vertices form a line of single pixel width
        glColor3f(1.0f,1.0f,1.0f);  // White
        glVertex2d(-0.2f, 0.9f);
        glVertex2d( 0.2f, 0.5f);

        glColor3f(0.0f,0.0f,1.0f);  // Blue
        glVertex2d(-0.2f, 0.5f);
        glVertex2d( 0.2f, 0.9f);
    glEnd();

    // Draw border of a square
    // at 'top-right'
    glBegin(GL_LINE_LOOP);  // All vertices form a single loop of single pixel width
        glColor3f(1.0f,1.0f,0.0f); // Yellow
        glVertex2d( 0.9f, 0.9f);
        glVertex2d( 0.9f, 0.5f);
        glVertex2d( 0.5f, 0.5f);
        glVertex2d( 0.5f, 0.9f);
    glEnd();

    /* Draw two filled triangles with second triangle having
       different colors in different vertices */
    // at 'left-middle'
    glBegin(GL_TRIANGLES);  // Each set of 3 vertices form a triangle
        glColor3f(0.0f,1.0f,1.0f);  // Cyan
        glVertex2f(-0.8f, 0.3f);
        glVertex2f(-1.0f,-0.1f);
        glVertex2f(-0.6f,-0.1f);

        // Rainbow colored triangle
        glColor3f(1.0f,0.0f,0.0f);  // Red
        glVertex2f(-0.5f, 0.1f);
        glColor3f(0.0f,1.0f,0.0f);  // Green
        glVertex2f(-0.7f,-0.3f);
        glColor3f(0.0f,0.0f,1.0f);  // Blue
        glVertex2f(-0.3f,-0.3f);
    glEnd();

    // Draw two filled quadrilaterals: the second one on top of first one
    // at the 'middle'
    glBegin(GL_QUADS);  // Each set of 4 vertices form a quad
        glColor3f(1.0f,0.0f,0.0f);  // Red
        glVertex2d( 0.0f, 0.3f);
        glVertex2d(-0.1f,-0.1f);
        glVertex2d( 0.4f,-0.2f);
        glVertex2d( 0.3f, 0.3f);

        glColor3f(0.0f,1.0f,0.0f);  // Green
        glVertex2d(-0.3f, 0.1f);
        glVertex2d(-0.2f,-0.3f);
        glVertex2d( 0.1f,-0.3f);
        glVertex2d( 0.2f, 0.2f);
    glEnd();

    /* Erroneous drawing of a quadrilateral.
       If v0, v1, v2, v3 are the vertices of the quadrilateral,
       then it actually draws two triangles: (v0,v1,v2) & (v0,v2,v3) */
    // at 'right-middle'
    glBegin(GL_QUADS);  // Each set of 4 vertices form a quad
        glColor3f(1.0f,0.0f,0.0f);  // Red
        glVertex2d( 0.5f,-0.2f);
        glVertex2d( 0.5f, 0.2f);
        glVertex2d( 0.9f,-0.2f);
        glVertex2d( 0.9f, 0.2f);
    glEnd();

    // Draw a filled hexagon (which is a polygon)
    // at 'bottom-left'
    glBegin(GL_POLYGON);    // All vertices form a single polygon
        glColor3f(0.5f,0.5f,0.5f);  // Gray
        glVertex2d(-0.82f,-0.5f);
        glVertex2d(-0.92f,-0.7f);
        glVertex2d(-0.82f,-0.9f);
        glVertex2d(-0.58f,-0.9f);
        glVertex2d(-0.48f,-0.7f);
        glVertex2d(-0.58f,-0.5f);
    glEnd();

    float cx, cy, r;

    // Draw the border of a circle
    // at 'bottom-middle'
    glBegin(GL_LINE_LOOP);  // All vertices form a single loop of single pixel width
        glColor3f(0.5f,0.5f,1.0f);  // Light-blue
        cx = 0;
        cy = -0.7;
        r = 0.2;
        for (float theta = 0; theta < 360; theta += 10) {
            float x = cx + r * cos(theta/180*M_PI);
            float y = cy + r * sin(theta/180*M_PI);
            glVertex2f(x,y);
        }
    glEnd();

    // Draw a filled circle
    // at 'bottom-right'
    glBegin(GL_POLYGON);    // All vertices form a single polygon
        glColor3f(1.0f,0.5f,0.0f);  // Orange
        cx = 0.7;
        cy = -0.7;
        r = 0.2;
        for (float theta = 0; theta < 360; theta += 10) {
            float x = cx + r * cos(theta/180*M_PI);
            float y = cy + r * sin(theta/180*M_PI);
            glVertex2f(x,y);
        }
    glEnd();

    glFlush();  // Render now
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    glutInit(&argc, argv);                  // Initialize GLUT
    glutInitWindowSize(640, 640);           // Set the window's initial width & height
    glutInitWindowPosition(50, 50);         // Position the window's initial top-left corner
    glutCreateWindow("OpenGL 2D Drawing");  // Create a window with the given title
    glutDisplayFunc(display);               // Register display callback handler for window re-paint
    glutMainLoop();                         // Enter the infinitely event-processing loop
    return 0;
}
