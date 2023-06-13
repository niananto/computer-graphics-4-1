#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <ctime>

#define PI (2*acos(0.0))

float cx = 0;
float cy = 0.4;
float r_outer = 0.5;
float r_inner = 0.4;
float r_hour = 0.2;
float r_minute = 0.25;
float r_second = 0.3;
float theta_hour = 90;
float theta_minute = 90;
float theta_second = 90;
float r_pendulum_top = 0.055;
float pendulum_length = 0.45;
float theta_pendulum = 0;
float r_pendulum_bottom = 0.08;


void display(){
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
  glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer

  float x, y;

  // the outer box
  glLineWidth(1);
  glBegin(GL_LINE_LOOP);
    glColor3d(1, 1, 1);  // White
    glVertex2f(-0.7, 0.95);
    glVertex2f(0.7, 0.95);
    glVertex2f(0.7, -0.45);
    glVertex2f(0, -0.85);
    glVertex2f(-0.7, -0.45);
  glEnd();

  // the outer circle
  glLineWidth(2);
  glBegin(GL_LINE_LOOP);
    glColor3d(1, 1, 1);  // White
    for (float theta = 0; theta < 360; theta += 10) {
        x = cx + r_outer * cos(theta/180*PI);
        y = cy + r_outer * sin(theta/180*PI);
        glVertex2f(x,y);
    }
  glEnd();

  // the inner circle
  glLineWidth(2);
  glBegin(GL_LINE_LOOP);
    glColor3d(1, 1, 1);  // White
    for (float theta = 0; theta < 360; theta += 10) {
        x = cx + r_inner * cos(theta/180*PI);
        y = cy + r_inner * sin(theta/180*PI);
        glVertex2f(x,y);
    }
  glEnd();

  // the hour ticks
  glLineWidth(1);
  glBegin(GL_LINES);
    glColor3d(1, 1, 1);  // White
    for (float theta = 0; theta < 360; theta += 90) {
        x = cx + r_inner * cos(theta/180*PI);
        y = cy + r_inner * sin(theta/180*PI);
        glVertex2f(x,y);
        x = cx + (r_inner - 0.07) * cos(theta/180*PI);
        y = cy + (r_inner - 0.07) * sin(theta/180*PI);
        glVertex2f(x,y);
    }
    for (float theta = 30; theta < 360; theta += 30) {
        x = cx + r_inner * cos(theta/180*PI);
        y = cy + r_inner * sin(theta/180*PI);
        glVertex2f(x,y);
        x = cx + (r_inner - 0.03) * cos(theta/180*PI);
        y = cy + (r_inner - 0.03) * sin(theta/180*PI);
        glVertex2f(x,y);
    }
  glEnd();

  // the hour hand
  glLineWidth(5);
  x = cx + r_hour * cos(theta_hour/180*PI);
  y = cy + r_hour * sin(theta_hour/180*PI);
  glBegin(GL_LINES);
    glColor3d(1, 1, 1);  // White
    glVertex2f(cx, cy);
    glVertex2f(x, y);
  glEnd();

  // the minute hand
  glLineWidth(3);
  x = cx + r_minute * cos(theta_minute/180*PI);
  y = cy + r_minute * sin(theta_minute/180*PI);
  glBegin(GL_LINES);
    glColor3d(1, 1, 1);  // White
    glVertex2f(cx, cy);
    glVertex2f(x, y);
  glEnd();

  // the second hand
  glLineWidth(1);
  x = cx + r_second * cos(theta_second/180*PI);
  y = cy + r_second * sin(theta_second/180*PI);
  glBegin(GL_LINES);
    glColor3d(1, 1, 1);  // White
    glVertex2f(cx, cy);
    glVertex2f(x, y);
  glEnd();

  // the pendulum
  // the top circle
  glBegin(GL_POLYGON);
    glColor3d(1, 1, 1);  // White
    for (float theta = 0; theta < 360; theta += 10) {
        x = cx + r_pendulum_top * cos(theta/180*PI);
        y = (cy - r_outer) + r_pendulum_top * sin(theta/180*PI);
        glVertex2f(x,y);
    }
  glEnd();

  // the rod
  glLineWidth(3);
  x = cx + pendulum_length * cos(theta_pendulum/180*PI);
  y = (cy - r_outer) + pendulum_length * sin(theta_pendulum/180*PI);
  glBegin(GL_LINES);
    glColor3d(1, 1, 1);  // White
    glVertex2f(cx, cy - r_outer);
    glVertex2f(x, y);
  glEnd();

  // the bottom circle
  glBegin(GL_POLYGON);
    glColor3d(1, 1, 1);  // White
    for (float theta = 0; theta < 360; theta += 10) {
        float temp_x = x + r_pendulum_bottom * cos(theta/180*PI);
        float temp_y = y + r_pendulum_bottom * sin(theta/180*PI);
        glVertex2f(temp_x, temp_y);
    }
  glEnd();

  glFlush();  // Render now
}

void timerPendulum(int value){
  glutTimerFunc(16, timerPendulum, 0); // next timer call milliseconds later

  // pendulum
  float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // Convert milliseconds to seconds
  float angularFrequency = M_PI; // period 2 seconds
  theta_pendulum = (-90) + sin(angularFrequency * t) * 45; // Simple harmonic motion formula; start from -90 degree

  glutPostRedisplay(); // Post re-paint request to activate display()
}

void timer(int value){
  glutTimerFunc(1000, timer, 0); // next timer call milliseconds later
  
  theta_second -= 6.0;
  theta_minute -= 6.0/60;
  theta_hour -= 6.0/3600;

  // reset
  theta_second = fmod(theta_second, 360);
  theta_minute = fmod(theta_minute, 360);
  theta_hour = fmod(theta_hour, 360);

  glutPostRedisplay(); // Post re-paint request to activate display()
}

void init(){
  std::time_t currentTime = std::time(nullptr);
  std::tm* localTime = std::localtime(&currentTime);

  int hour = localTime->tm_hour % 12;
  int minute = localTime->tm_min;
  int second = localTime->tm_sec;

  // Print the extracted time components
  std::cout << "Hour: " << hour << std::endl;
  std::cout << "Minute: " << minute << std::endl;
  std::cout << "Second: " << second << std::endl;

  // set the initial time
  theta_second -= 6.0 * second;
  theta_minute -= 6.0 * (minute + second/60.0);
  theta_hour -= 30.0 * (hour + minute/60.0 + second/3600.0);
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Clock");

  init();

	glutDisplayFunc(display);	//display callback function
  glutTimerFunc(0, timer, 0); // First timer call immediately
	glutTimerFunc(0, timerPendulum, 0); // First timer call immediately

	glutMainLoop();		//The main loop of OpenGL
	return 0;
}