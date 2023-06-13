#include <GL/glut.h>
#include <cmath>

const float radius = 1.0f;
const int numSegments = 6;

void drawSphereSegment(float alpha, float beta, float red, float green, float blue) {
    const float pi = 3.14159265359f;

    glColor3f(red, green, blue);

    for (int j = 0; j <= numSegments / 2; j++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= numSegments; i++) {
            float theta = 2.0f * pi * float(i) / numSegments;
            float x = radius * sin(theta) * sin(beta) * cos(alpha);
            float y = radius * cos(theta) * sin(beta) * cos(alpha);
            float z = radius * sin(alpha);
            glVertex3f(x, y, z);

            x = radius * sin(theta) * sin(beta) * sin(alpha);
            y = radius * cos(theta) * sin(beta) * sin(alpha);
            z = radius * cos(alpha);
            glVertex3f(x, y, z);
        }
        glEnd();
        alpha += pi / numSegments;
    }
}

void drawSphere() {
    const float pi = 3.14159265359f;
    float alpha = 0.0f;
    float beta = pi / numSegments;

    // Draw each segment with a different color
    drawSphereSegment(alpha, beta, 1.0f, 0.0f, 0.0f); // Red
    drawSphereSegment(alpha, beta + pi / numSegments, 0.0f, 1.0f, 0.0f); // Green
    drawSphereSegment(alpha, beta + 2 * pi / numSegments, 0.0f, 0.0f, 1.0f); // Blue
    drawSphereSegment(alpha, beta + 3 * pi / numSegments, 1.0f, 1.0f, 0.0f); // Yellow
    drawSphereSegment(alpha, beta + 4 * pi / numSegments, 1.0f, 0.0f, 1.0f); // Magenta
    drawSphereSegment(alpha, beta + 5 * pi / numSegments, 0.0f, 1.0f, 1.0f); // Cyan
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // Set the camera position and orientation

    // Draw the sphere
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f); // Red color
    drawSphere();
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / height, 1.0, 100.0); // Set the projection matrix

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Spherical Segments");

    glEnable(GL_DEPTH_TEST); // Enable depth testing

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
