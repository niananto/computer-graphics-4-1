#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

class Point {
public:
    double x, y, z;
    Point();
    Point(double x, double y, double z);
    Point* copy();
    void normalize();
    Point* add(Point *p);
    Point* subtract(Point *p);
    Point* multiply(double scalar);
    Point* cross(Point *p);
    double dot(Point *p);
};

class Color {
public:
    double r, g, b;
    Color();
    Color(double r, double g, double b);
};

class Ray {
public:
    Point *start, *dir;
    Color color;
    Ray(Point *start, Point *dir);
    Point* getPoint(double t);
    ~Ray();
};

class LightCoefficients {
public:
    double ambient, diffuse, specular, reflection;
};

class CheckerBoard {
public:
    int width, height;
    LightCoefficients lightCoefficients;
};

class Object {
public:
    string objectType;
    Color color;
    LightCoefficients lightCoefficients;
    double shininess;

    virtual void draw() {}
    virtual Color* intersect(Ray *ray, int recLevel) {return NULL;}
};

class Pyramid : public Object {
public:
    Point lowest;
    double width;
    double height;

    void draw();
    Color* intersect(Ray *ray, int recLevel) {return NULL;}
};

class Sphere : public Object {
    Point** getSide(int subdivision);
    void drawSide(int subdivision);
public:
    Point center;
    double radius;

    void draw();
    Color* intersect(Ray *ray, int recLevel);
};

class Cube : public Object {
public:
    Point bottomLeft;
    double side;

    void draw();
    Color* intersect(Ray *ray, int recLevel) {return NULL;}
};

class LightSource {
public:
    string lightType;
    Point position;
    double falloff;
    Color color;
    
    LightSource(string lightType);
    virtual void draw() {}
};

class NormalLightSource : public LightSource {
public:
    NormalLightSource();
    void draw();
};

class SpotLightSource : public LightSource {
public:
    Point direction;
    double cutoffAngle;

    SpotLightSource();
    void draw();
};

//////////////////////////////// POINT ////////////////////////////////

Point::Point() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Point::Point(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Point* Point::copy() {
    return new Point(x, y, z);
}

void Point::normalize() {
    double length = sqrt(x * x + y * y + z * z);
    x /= length;
    y /= length;
    z /= length;
}

Point* Point::add(Point *p) {
    double x = this->x + p->x;
    double y = this->y + p->y;
    double z = this->z + p->z;
    return new Point(x, y, z);
}

Point* Point::subtract(Point *p) {
    double x = this->x - p->x;
    double y = this->y - p->y;
    double z = this->z - p->z;
    return new Point(x, y, z);
}

Point* Point::multiply(double scalar) {
    double x = this->x * scalar;
    double y = this->y * scalar;
    double z = this->z * scalar;
    return new Point(x, y, z);
}

Point* Point::cross(Point *p) {
    double x = this->y * p->z - this->z * p->y;
    double y = this->z * p->x - this->x * p->z;
    double z = this->x * p->y - this->y * p->x;
    return new Point(x, y, z);
}

double Point::dot(Point *p) {
    return this->x * p->x + this->y * p->y + this->z * p->z;
}

//////////////////////////////// COLOR ////////////////////////////////

Color::Color() {
    this->r = 0;
    this->g = 0;
    this->b = 0;
}

Color::Color(double r, double g, double b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

//////////////////////////////// RAY ////////////////////////////////

Ray::Ray(Point *start, Point *dir) {
    this->start = start;
    this->dir = dir;
    this->dir->normalize();
    color = Color(1, 1, 1);
}

Point* Ray::getPoint(double t) {
    return start->add(dir->multiply(t));
}

Ray::~Ray() {
    delete start;
    delete dir;
}

/////////////////////////////// PYRAMID ///////////////////////////////

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

void Pyramid::draw()
{
    glPushMatrix();
    {
        glColor3f(color.r, color.g, color.b);

        glTranslatef(lowest.x, lowest.y, lowest.z);
        // width will affect x and z, height will affect y
        glScalef(width/sqrt(2), width/sqrt(2), height);

        drawTriangle();
        glRotatef(90, 0, 0, 1);
        drawTriangle();
        glRotatef(90, 0, 0, 1);
        drawTriangle();
        glRotatef(90, 0, 0, 1);
        drawTriangle();
        glRotatef(90, 0, 0, 1);

        glBegin(GL_QUADS);
        glVertex3f(1, 0, 0);
        glVertex3f(0, -1, 0);
        glVertex3f(-1, 0, 0);
        glVertex3f(0, 1, 0);
        glEnd();
    }
    glPopMatrix();
}

/////////////////////////////// SPHERE ///////////////////////////////


// generate vertices for +X face only by intersecting 2 circular planes
// (longitudinal and latitudinal) at the given longitude/latitude angles
Point **Sphere::getSide(int subdivision)
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

        // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-r.)
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

void Sphere::drawSide(int subdivision = 5)
{
    Point **points = getSide(subdivision);
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

void Sphere::draw()
{
    glPushMatrix();
    {
        glColor3f(color.r, color.g, color.b);
        glTranslatef(center.x, center.y, center.z);

        drawSide();
        glRotatef(90, 0, 1, 0);
        drawSide();
        glRotatef(90, 0, 1, 0);
        drawSide();
        glRotatef(90, 0, 1, 0);
        drawSide();
        glRotatef(90, 0, 0, 1);
        drawSide();
        glRotatef(180, 0, 0, 1);
        drawSide();
    }
    glPopMatrix();
}

// intersection calculation
Color* Sphere::intersect(Ray *ray, int recLevel) {
    Point *centerToStart = ray->start->subtract(&center);
    double a = 1; /*ray->dir.dot(&ray->dir)*/
    double b = 2 * ray->dir->dot(centerToStart);
    double c = centerToStart->dot(centerToStart) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return NULL;
    }
    double t1 = (-b + sqrt(discriminant)) / (2 * a);
    double t2 = (-b - sqrt(discriminant)) / (2 * a);
    double t = min(t1, t2);
    // return ray->getPoint(t);
    return &color;
}

/////////////////////////////// CUBE ///////////////////////////////

void Cube::draw()
{
    glPushMatrix();
    // {
        glColor3f(color.r, color.g, color.b);
        glTranslatef(bottomLeft.x, bottomLeft.y, bottomLeft.z);
        glScalef(side, side, side);

        glBegin(GL_QUADS);
        {
            // bottom
            glVertex3f(0, 0, 0);
            glVertex3f(1, 0, 0);
            glVertex3f(1, 1, 0);
            glVertex3f(0, 1, 0);

            // top
            glVertex3f(0, 0, 1);
            glVertex3f(1, 0, 1);
            glVertex3f(1, 1, 1);
            glVertex3f(0, 1, 1);

            // left
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, 1);
            glVertex3f(0, 1, 1);
            glVertex3f(0, 1, 0);

            // right
            glVertex3f(1, 0, 0);
            glVertex3f(1, 0, 1);
            glVertex3f(1, 1, 1);
            glVertex3f(1, 1, 0);

            // back
            glVertex3f(0, 1, 0);
            glVertex3f(1, 1, 0);
            glVertex3f(1, 1, 1);
            glVertex3f(0, 1, 1);

            // front
            glVertex3f(0, 0, 0);
            glVertex3f(1, 0, 0);
            glVertex3f(1, 0, 1);
            glVertex3f(0, 0, 1);
        }
        glEnd();
    // }
    glPopMatrix();
}

/////////////////////////// LIGHTSOURCE //////////////////////////////

LightSource::LightSource(string lightType) {
    this->lightType = lightType;
    this->color = Color(1, 1, 1);
}

///////////////////////// NORMAL LIGHTSOURCE /////////////////////////

NormalLightSource::NormalLightSource() : LightSource("normal") {}

void NormalLightSource::draw() {
    glPushMatrix();
    {
        glColor3f(color.r, color.g, color.b);
        glTranslatef(position.x, position.y, position.z);
        glutSolidSphere(3, 20, 20);
    }
    glPopMatrix();
}

///////////////////////// SPOT LIGHTSOURCE /////////////////////////

SpotLightSource::SpotLightSource() : LightSource("spot") {}

void SpotLightSource::draw() {
    glPushMatrix();
    {
        glColor3f(color.r, color.g, color.b);
        glTranslatef(position.x, position.y, position.z);
        glutSolidSphere(3, 20, 20);
    }
    glPopMatrix();
}