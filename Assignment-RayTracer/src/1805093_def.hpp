#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

#define EPSILON 0.01

double determinant(double a[3][3])
{
    double det = 0;
    for (int i = 0; i < 3; i++)
        det = det + (a[0][i] * (a[1][(i + 1) % 3] * a[2][(i + 2) % 3] - a[1][(i + 2) % 3] * a[2][(i + 1) % 3]));
    return det;
}

class Point
{
public:
    double x, y, z;
    Point();
    Point(double x, double y, double z);
    Point *copy();
    void normalize();
    Point *add(Point *p);
    Point *subtract(Point *p);
    Point *multiply(double scalar);
    Point *cross(Point *p);
    double dot(Point *p);
    double magnitude();
};

class Color
{
public:
    double r, g, b;
    Color();
    Color(double r, double g, double b);

    Color* multiply(double scalar);
    Color* add(Color* c);
    Color* copy();
};

class Ray
{
public:
    Point *start, *dir;
    Color color;
    Ray(Point *start, Point *dir);
    Point *getPoint(double t);
    ~Ray();
};

class LightCoefficients
{
public:
    double ambient, diffuse, specular, reflection;
};

class Triangle
{
public:
    Point a, b, c;

    Triangle(Point a, Point b, Point c);
    double calcIntersection(Ray *ray);
    Point* getNormal();
};

class Rect
{
public:
    Point corner1, corner2;
    Rect(Point corner1, Point corner2);
    double calcIntersection(Ray *ray);
};

class Object
{
public:
    string objectType;
    Color color;
    LightCoefficients lightCoefficients;
    double shininess;

    Object();
    double recIntersection(Ray *ray, int recLevel);
    virtual void draw() {}
    virtual double handleIntersecttion(Ray *ray) = 0;
    virtual Point* getNormal(Point *p) = 0;
};

class Board : public Object
{
public:
    int width, height;

    void draw();
    double handleIntersecttion(Ray *ray);
    Point* getNormal(Point *p);
};

class Pyramid : public Object
{
    vector<Triangle *> sideTriangles;
    Rect* bottomRect;
    void calculateAllSides();
public:
    Point lowest;
    double width;
    double height;

    void draw();
    double handleIntersecttion(Ray *ray);
    Point* getNormal(Point *p);
    ~Pyramid();
};

class Sphere : public Object
{
    Point **getSide(int subdivision);
    void drawSide(int subdivision);

public:
    Point center;
    double radius;

    void draw();
    double handleIntersecttion(Ray *ray);
    Point* getNormal(Point *p);
};

class Cube : public Object
{
public:
    Point bottomLeftFront;
    double side;

    void draw();
    double handleIntersecttion(Ray *ray);
    Point* getNormal(Point *p);
};

class LightSource
{
public:
    string lightType;
    Point position;
    double falloff;
    Color color;

    LightSource(string lightType);
    virtual void draw() = 0;
};

class NormalLightSource : public LightSource
{
public:
    NormalLightSource();
    void draw();
};

class SpotLightSource : public LightSource
{
public:
    Point direction;
    double cutoffAngle;

    SpotLightSource();
    void draw();
};

//////////////////////////////// OBJECT ////////////////////////////////

Object::Object()
{
    color = Color();
    lightCoefficients = LightCoefficients();
    shininess = 0;
}

double Object::recIntersection(Ray *ray, int recLevel)
{
    double t = handleIntersecttion(ray);

    if (t < 0)
        return -1;
    if (recLevel == 0)
        return t;

    return t;
}

//////////////////////////////// POINT ////////////////////////////////

Point::Point()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Point::Point(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Point *Point::copy()
{
    return new Point(x, y, z);
}

void Point::normalize()
{
    double length = sqrt(x * x + y * y + z * z);
    if (length == 0)
        return;
    x /= length;
    y /= length;
    z /= length;
}

Point *Point::add(Point *p)
{
    double x = this->x + p->x;
    double y = this->y + p->y;
    double z = this->z + p->z;
    return new Point(x, y, z);
}

Point *Point::subtract(Point *p)
{
    double x = this->x - p->x;
    double y = this->y - p->y;
    double z = this->z - p->z;
    return new Point(x, y, z);
}

Point *Point::multiply(double scalar)
{
    double x = this->x * scalar;
    double y = this->y * scalar;
    double z = this->z * scalar;
    return new Point(x, y, z);
}

Point *Point::cross(Point *p)
{
    double x = this->y * p->z - this->z * p->y;
    double y = this->z * p->x - this->x * p->z;
    double z = this->x * p->y - this->y * p->x;
    return new Point(x, y, z);
}

double Point::dot(Point *p)
{
    return this->x * p->x + this->y * p->y + this->z * p->z;
}

double Point::magnitude()
{
    return sqrt(x * x + y * y + z * z);
}

//////////////////////////////// COLOR ////////////////////////////////

Color::Color()
{
    this->r = 0;
    this->g = 0;
    this->b = 0;
}

Color::Color(double r, double g, double b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

Color* Color::multiply(double scalar)
{
    double r = this->r * scalar;
    double g = this->g * scalar;
    double b = this->b * scalar;
    return new Color(r, g, b);
}

Color* Color::add(Color* c)
{
    double r = this->r + c->r;
    double g = this->g + c->g;
    double b = this->b + c->b;
    return new Color(r, g, b);
}

Color* Color::copy()
{
    return new Color(r, g, b);
}

//////////////////////////////// RAY ////////////////////////////////

Ray::Ray(Point *start, Point *dir)
{
    this->start = start;
    this->dir = dir;
    this->dir->normalize();
    color = Color(1, 1, 1);
}

Point *Ray::getPoint(double t)
{
    return start->add(dir->multiply(t));
}

Ray::~Ray()
{
    delete start;
    delete dir;
}

/////////////////////////////// TRIANGLE ///////////////////////////////

Triangle::Triangle(Point a, Point b, Point c)
{
    this->a = a;
    this->b = b;
    this->c = c;
}

double Triangle::calcIntersection(Ray *ray)
{
    double betaMatrix[3][3] = {
        {a.x - ray->start->x, a.x - c.x, ray->dir->x},
        {a.y - ray->start->y, a.y - c.y, ray->dir->y},
        {a.z - ray->start->z, a.z - c.z, ray->dir->z}};
    double gammaMatrix[3][3] = {
        {a.x - b.x, a.x - ray->start->x, ray->dir->x},
        {a.y - b.y, a.y - ray->start->y, ray->dir->y},
        {a.z - b.z, a.z - ray->start->z, ray->dir->z}};
    double tMatrix[3][3] = {
        {a.x - b.x, a.x - c.x, a.x - ray->start->x},
        {a.y - b.y, a.y - c.y, a.y - ray->start->y},
        {a.z - b.z, a.z - c.z, a.z - ray->start->z}};
    double aMatrix[3][3] = {
        {a.x - b.x, a.x - c.x, ray->dir->x},
        {a.y - b.y, a.y - c.y, ray->dir->y},
        {a.z - b.z, a.z - c.z, ray->dir->z}};

    double aDet = determinant(aMatrix);
    double beta = determinant(betaMatrix) / aDet;
    double gamma = determinant(gammaMatrix) / aDet;
    double t = determinant(tMatrix) / aDet;

    if (beta >= -EPSILON && gamma >= -EPSILON && beta + gamma <= 1+EPSILON && t > 0)
        return t;

    return -1;
}

Point* Triangle::getNormal()
{
    Point *normal = a.subtract(&b)->cross(a.subtract(&c));
    normal->normalize();
    return normal;
}

/////////////////////////////// RECT ///////////////////////////////

Rect::Rect(Point corner1, Point corner2)
{
    this->corner1 = corner1;
    this->corner2 = corner2;
}

double Rect::calcIntersection(Ray *ray)
{
    // first find out which plane the rectangle in parallel to

    // if the rectangle is parallel to XY plane
    if (corner1.z == corner2.z)
    {
        // if the ray is parallel to XY plane
        if (ray->dir->z == 0)
            return -1;

        // if the ray is not parallel to XY plane
        double t = (corner1.z - ray->start->z) / ray->dir->z;
        if (t < 0)
            return -1;

        Point *intersection = ray->getPoint(t);
        if (intersection->x >= corner1.x && intersection->x <= corner2.x && intersection->y >= corner1.y && intersection->y <= corner2.y)
            return t;
    }
    // if the rectangle is parallel to YZ plane
    else if (corner1.x == corner2.x)
    {
        // if the ray is parallel to YZ plane
        if (ray->dir->x == 0)
            return -1;

        // if the ray is not parallel to YZ plane
        double t = (corner1.x - ray->start->x) / ray->dir->x;
        if (t < 0)
            return -1;

        Point *intersection = ray->getPoint(t);
        if (intersection->z >= corner1.z && intersection->z <= corner2.z && intersection->y >= corner1.y && intersection->y <= corner2.y)
            return t;
    }
    // if the rectangle is parallel to XZ plane
    else if (corner1.y == corner2.y)
    {
        // if the ray is parallel to XZ plane
        if (ray->dir->y == 0)
            return -1;

        // if the ray is not parallel to XZ plane
        double t = (corner1.y - ray->start->y) / ray->dir->y;
        if (t < 0)
            return -1;

        Point *intersection = ray->getPoint(t);
        if (intersection->x >= corner1.x && intersection->x <= corner2.x && intersection->z >= corner1.z && intersection->z <= corner2.z)
            return t;
    }

    // not parallel to any plane?
    return -1;
}

/////////////////////////////// BOARD ///////////////////////////////

void Board::draw()
{
    // draw in infinite checker board with given size and coeff
    for (int i = -100; i < 100; i++)
    {
        for (int j = -100; j < 100; j++)
        {
            if ((i + j) % 2 == 0)
                glColor3f(1, 1, 1);
            else
                glColor3f(0, 0, 0);

            glBegin(GL_QUADS);
            glVertex3f(i * width, j * height, 0);
            glVertex3f((i + 1) * width, j * height, 0);
            glVertex3f((i + 1) * width, (j + 1) * height, 0);
            glVertex3f(i * width, (j + 1) * height, 0);

            glEnd();
        }
    }
}

double Board::handleIntersecttion(Ray *ray)
{
    // check if the ray intersects with the board
    // board is on the XY plane
    double t = Rect(Point(-100*width, -100*height, 0), Point(100*width, 100*height, 0)).calcIntersection(ray);
    return t;
}

Point* Board::getNormal(Point *p)
{
    return new Point(0, 0, 1);
}

/////////////////////////////// PYRAMID ///////////////////////////////

void Pyramid::calculateAllSides()
{
    if (sideTriangles.size() > 0 && bottomRect != NULL)
        return;

    Point *bottom1 = lowest.add(new Point(width / sqrt(2), 0, 0));
    Point *bottom2 = lowest.add(new Point(0, width / sqrt(2), 0));
    Point *bottom3 = lowest.add(new Point(-width / sqrt(2), 0, 0));
    Point *bottom4 = lowest.add(new Point(0, -width / sqrt(2), 0));
    Point *top = lowest.add(new Point(0, 0, height));

    sideTriangles.push_back(new Triangle(*bottom1, *bottom2, *top));
    sideTriangles.push_back(new Triangle(*bottom2, *bottom3, *top));
    sideTriangles.push_back(new Triangle(*bottom3, *bottom4, *top));
    sideTriangles.push_back(new Triangle(*bottom4, *bottom1, *top));

    bottomRect = new Rect(*bottom3, *bottom1);

    delete bottom1, bottom2, bottom3, bottom4, top;
}

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
        glScalef(width / sqrt(2), width / sqrt(2), height);

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

double Pyramid::handleIntersecttion(Ray *ray)
{
    calculateAllSides();

    // apply barrycentric coordinates
    // for each triangle, check if the ray intersects
    // if intersects, check if the point is inside the triangle
    // if inside, return the color of the triangle
    double tMin = -1;
    for (Triangle *triangle : sideTriangles)
    {
        double t = triangle->calcIntersection(ray);
        if (t > 0 && (tMin < 0 || t < tMin))
            tMin = t;
    }

    // check if the ray intersects with the bottom Rect
    double t = bottomRect->calcIntersection(ray);
    if (t > 0 && (tMin < 0 || t < tMin))
        tMin = t;

    return tMin;
}

Point* Pyramid::getNormal(Point* p)
{
    calculateAllSides();

    // check if the point is on the bottom Rect
    if (p->z == lowest.z)
        return new Point(0, 0, -1);

    // check if the point is on the side triangles
    for (Triangle *triangle : sideTriangles)
    {
        // check if the point is on the triangle or not
        Point *normal = triangle->getNormal();
        Point *pToA = p->subtract(&triangle->a);
        double dot = normal->dot(pToA);
        if (dot < EPSILON)
            return normal;

        delete normal, pToA;
    }
    
    return NULL;
}

Pyramid::~Pyramid()
{
    for (Triangle *triangle : sideTriangles)
        delete triangle;
    delete bottomRect;
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
double Sphere::handleIntersecttion(Ray *ray)
{
    Point *centerToStart = ray->start->subtract(&center);
    double a = 1; /*ray->dir.dot(&ray->dir)*/
    double b = 2 * ray->dir->dot(centerToStart);
    double c = centerToStart->dot(centerToStart) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
        return -1;

    double t1 = (-b + sqrt(discriminant)) / (2 * a);
    double t2 = (-b - sqrt(discriminant)) / (2 * a);
    if (t1 < 0 && t2 < 0)
        return -1;
    else if (t1 < 0)
        return t2;
    else if (t2 < 0)
        return t1;
    else
        return min(t1, t2);
}

Point* Sphere::getNormal(Point *p)
{
    // check if the point is on the sphere
    Point *centerToP = p->subtract(&center);
    if (abs(centerToP->dot(centerToP) - radius * radius) < EPSILON)
    {
        centerToP->normalize();
        return centerToP;
    }

    return NULL;
}

/////////////////////////////// CUBE ///////////////////////////////

void Cube::draw()
{
    glPushMatrix();
    // {
    glColor3f(color.r, color.g, color.b);
    glTranslatef(bottomLeftFront.x, bottomLeftFront.y, bottomLeftFront.z);
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

double Cube::handleIntersecttion(Ray *ray)
{
    // divide the cube in 6 rectangles
    vector<Rect *> rects;
    rects.push_back(new Rect(Point(bottomLeftFront.x, bottomLeftFront.y, bottomLeftFront.z), Point(bottomLeftFront.x + side, bottomLeftFront.y + side, bottomLeftFront.z)));
    rects.push_back(new Rect(Point(bottomLeftFront.x, bottomLeftFront.y, bottomLeftFront.z), Point(bottomLeftFront.x, bottomLeftFront.y + side, bottomLeftFront.z + side)));
    rects.push_back(new Rect(Point(bottomLeftFront.x, bottomLeftFront.y, bottomLeftFront.z), Point(bottomLeftFront.x + side, bottomLeftFront.y, bottomLeftFront.z + side)));
    rects.push_back(new Rect(Point(bottomLeftFront.x + side, bottomLeftFront.y, bottomLeftFront.z), Point(bottomLeftFront.x + side, bottomLeftFront.y + side, bottomLeftFront.z + side)));
    rects.push_back(new Rect(Point(bottomLeftFront.x, bottomLeftFront.y + side, bottomLeftFront.z), Point(bottomLeftFront.x + side, bottomLeftFront.y + side, bottomLeftFront.z + side)));
    rects.push_back(new Rect(Point(bottomLeftFront.x, bottomLeftFront.y, bottomLeftFront.z + side), Point(bottomLeftFront.x + side, bottomLeftFront.y + side, bottomLeftFront.z + side)));

    // for each rectangle, check if the ray intersects
    double tMin = -1;
    for (Rect *rect : rects)
    {
        double t = rect->calcIntersection(ray);
        if (t > 0 && (tMin < 0 || t < tMin))
            tMin = t;

        delete rect;
    }

    return tMin;
}

Point* Cube::getNormal(Point *p)
{
    // check which face the point is on
    // if the point is on the bottom face
    if (bottomLeftFront.z-EPSILON <= p->z && p->z <= bottomLeftFront.z+EPSILON)
        return new Point(0, 0, -1);
    // if the point is on the top face
    else if (bottomLeftFront.z+side-EPSILON <= p->z && p->z <= bottomLeftFront.z+side+EPSILON)
        return new Point(0, 0, 1);
    // if the point is on the left face
    else if (bottomLeftFront.x-EPSILON <= p->x && p->x <= bottomLeftFront.x+EPSILON)
        return new Point(-1, 0, 0);
    // if the point is on the right face
    else if (bottomLeftFront.x+side-EPSILON <= p->x && p->x <= bottomLeftFront.x+side+EPSILON)
        return new Point(1, 0, 0);
    // if the point is on the back face
    else if (bottomLeftFront.y-EPSILON <= p->y && p->y <= bottomLeftFront.y+EPSILON)
        return new Point(0, -1, 0);
    // if the point is on the front face
    else if (bottomLeftFront.y+side-EPSILON <= p->y && p->y <= bottomLeftFront.y+side+EPSILON)
        return new Point(0, 1, 0);

    return NULL;
}

/////////////////////////// LIGHTSOURCE //////////////////////////////

LightSource::LightSource(string lightType)
{
    this->lightType = lightType;
    this->color = Color(1, 1, 1);
}

///////////////////////// NORMAL LIGHTSOURCE /////////////////////////

NormalLightSource::NormalLightSource() : LightSource("normal") {}

void NormalLightSource::draw()
{
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

void SpotLightSource::draw()
{
    glPushMatrix();
    {
        glColor3f(color.r, color.g, color.b);
        glTranslatef(position.x, position.y, position.z);
        glutSolidSphere(3, 20, 20);
    }
    glPopMatrix();
}