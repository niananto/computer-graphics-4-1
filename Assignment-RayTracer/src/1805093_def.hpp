#include <windows.h> // for MS Windows
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <cmath>
#include <iostream>
#include <vector>
#include <iomanip>
#include <cassert>

using namespace std;

#define EPSILON 0.00001

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

    Color *multiply(double scalar);
    Color *multiply(Color *c);
    Color *add(Color *c);
    Color *copy();
    void adjust();
};

extern boolean showTexture;
extern vector<vector<Color *>> whiteTileColorBuffer;
extern vector<vector<Color *>> blackTileColorBuffer;

class Ray
{
public:
    Point *start, *dir;
    Color color;
    Ray(Point *start, Point *dir);
    Point *getPoint(double t);
    Ray *copy();
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
    Point *getNormal(Point *p);
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
    Color *recIntersection(Ray *ray, Point *intersectionPoint, double t, int recLevel);
    virtual void draw() {}
    virtual double handleIntersecttion(Ray *ray) = 0;
    virtual Point *getNormal(Point *p, Point *rayDir) = 0;
    virtual Color *getColorAt(Point *p) { return color.copy(); }
};

class Board : public Object
{
public:
    int tileWidth, tileHeight;
    int tileCount;

    void draw();
    double handleIntersecttion(Ray *ray);
    Point *getNormal(Point *p, Point *rayDir);
    Color *getColorAt(Point *p);
    Color *getTextureAt(Point *p);
};

class Pyramid : public Object
{
    vector<Triangle *> sideTriangles;
    Rect *bottomRect;
    void calculateAllSides();

public:
    Point lowest;
    double width;
    double height;

    void draw();
    double handleIntersecttion(Ray *ray);
    Point *getNormal(Point *p, Point *rayDir);
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
    Point *getNormal(Point *p, Point *rayDir);
};

class Cube : public Object
{
public:
    Point bottomLeftFront;
    double side;

    void draw();
    double handleIntersecttion(Ray *ray);
    Point *getNormal(Point *p, Point *rayDir);
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

extern vector<Object *> objects;
extern vector<LightSource *> lights;

Color *Object::recIntersection(Ray *ray, Point *intersectionPoint, double t, int recLevel)
{
    if (t <= EPSILON || recLevel == 0)
        return new Color(0, 0, 0);

    Color *colorHere = getColorAt(intersectionPoint);
    // this is not to make the board dark, there is a corresponding commented out part below
    if (showTexture && objectType == "board")
        colorHere = ((Board *)this)->getTextureAt(intersectionPoint);

    // ambient
    Color *ambient = colorHere->multiply(this->lightCoefficients.ambient);

    // diffuse and specular and reflection
    double lambert = 0, phong = 0;
    Color *reflection = new Color(0, 0, 0); // reflection is black by default

    for (LightSource *light : lights)
    {
        // let's check if the light is blocked by any other object
        Point *toObject = intersectionPoint->subtract(&(light->position));
        Ray *toObjectRay = new Ray(light->position.copy(), toObject->copy());

        boolean isBlocked = false;
        double tCurrent = this->handleIntersecttion(toObjectRay);
        for (Object *object : objects)
        {
            // no need to check if the object is the current object
            // cause self blocking is a thing

            double t = object->handleIntersecttion(toObjectRay);
            // the blocking object must be closer than the current object from the source
            if (t > -EPSILON && (tCurrent < 0 || t < tCurrent))
            {
                isBlocked = true;
                break;
            }
        }
        delete toObjectRay;

        if (isBlocked == false && light->lightType == "spot")
        {
            SpotLightSource *spot = (SpotLightSource *)light;
            Point *sourceToObject = intersectionPoint->subtract(&(spot->position));
            sourceToObject->normalize();
            spot->direction.normalize();
            double angle = acos(sourceToObject->dot(&(spot->direction)));
            if (angle * 180 / M_PI > spot->cutoffAngle)
                isBlocked = true;
        }

        if (isBlocked)
            continue;

        Point *toSource = light->position.subtract(intersectionPoint);
        toSource->normalize();

        Point *N = this->getNormal(intersectionPoint, toSource);
        assert(N != NULL);
        N->normalize();

        double distance = light->position.subtract(intersectionPoint)->magnitude();
        double scalingFactor = exp(-distance * distance * light->falloff);
        lambert += max(0.0, toSource->dot(N)) * scalingFactor;

        Point *R = ray->dir->subtract(N->multiply(2 * ray->dir->dot(N)));
        // Point *R = N->multiply(2 * N->dot(toSource))->subtract(toSource);
        // this commented line here gave me multiple reflections for multiple sources
        R->normalize();

        phong += pow(max(0.0, R->dot(toSource)), this->shininess) * scalingFactor;

        delete toSource, N;

        // reflection
        if (recLevel == 1)
            continue;

        Ray *reflectedRay = new Ray(intersectionPoint->add(R->multiply(2 * EPSILON)), R->copy());

        double tMin = -1;
        Object *nearestObject = NULL;
        for (Object *object : objects)
        {
            // for spheres, pyramids and cubes - self reflection is not possible
            if (object == this)
                continue;

            double t = object->handleIntersecttion(reflectedRay);
            if (t > -EPSILON && (tMin < 0 || t < tMin))
            {
                tMin = t;
                nearestObject = object;
            }
        }

        if (nearestObject != NULL)
        {
            Point *reflectedPoint = reflectedRay->getPoint(tMin);
            Color *reflectedColor = nearestObject->recIntersection(reflectedRay, reflectedPoint, tMin, recLevel - 1);
            reflection = reflection->add(reflectedColor->multiply(this->lightCoefficients.reflection));
            reflection->adjust();
            delete reflectedPoint, reflectedColor;
        }

        delete reflectedRay, R;
    }

    Color *diffusedColor = colorHere->multiply(this->lightCoefficients.diffuse * lambert);
    Color *specularColor = colorHere->multiply(this->lightCoefficients.specular * phong);

    Color *currColor;
    // this makes the board too dark
    // if (showTexture && objectType == "board")
    // {
    //     Color *textureColor = ((Board *)this)->getTextureAt(intersectionPoint);
    //     assert(textureColor != NULL);

    //     // ambient = ambient->multiply(textureColor);
    //     // diffusedColor = diffusedColor->multiply(textureColor);
    //     delete textureColor;
    // }
    currColor = ambient->add(diffusedColor)->add(specularColor)->add(reflection);
    currColor->adjust();
    delete ambient, diffusedColor, specularColor, colorHere;

    return currColor;
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
    double length = this->magnitude();
    if (length == 0)
    {
        cout << "vector length is 0" << endl;
        return;
    }
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

Color *Color::multiply(double scalar)
{
    double r = this->r * scalar;
    double g = this->g * scalar;
    double b = this->b * scalar;
    return new Color(r, g, b);
}

Color *Color::multiply(Color *c)
{
    double r = this->r * c->r;
    double g = this->g * c->g;
    double b = this->b * c->b;
    return new Color(r, g, b);
}

Color *Color::add(Color *c)
{
    double r = this->r + c->r;
    double g = this->g + c->g;
    double b = this->b + c->b;
    return new Color(r, g, b);
}

Color *Color::copy()
{
    return new Color(r, g, b);
}

void Color::adjust()
{
    // // cout << fixed << setprecision(10) << "before: " << r << " " << g << " " << b << endl;
    if (r > 1)
        g = g / r, b = b / r, r = 1;
    if (g > 1)
        r = r / g, b = b / g, g = 1;
    if (b > 1)
        r = r / b, g = g / b, b = 1;
    // if (r > 1)
    //     r = 1;
    // if (g > 1)
    //     g = 1;
    // if (b > 1)
    //     b = 1;
    if (r < 0)
        g = g / (1 + r), b = b / (1 + r), r = 0;
    if (g < 0)
        r = r / (1 + g), b = b / (1 + g), g = 0;
    if (b < 0)
        r = r / (1 + b), g = g / (1 + b), b = 0;
    // if (r < 0)
    //     r = 0;
    // if (g < 0)
    //     g = 0;
    // if (b < 0)
    //     b = 0;
    // // cout << "after: " << r << " " << g << " " << b << endl;

    // double maxValue = max(r, max(g, b));
    // double minValue = min(r, min(g, b));

    // if (maxValue > 1.0) {
    //     r /= maxValue;
    //     g /= maxValue;
    //     b /= maxValue;
    // } else if (minValue < 0.0) {
    //     double adjustment = min(-minValue, 1.0);

    //     r += adjustment;
    //     g += adjustment;
    //     b += adjustment;
    // }

    // r = max(0.0, min(1.0, r));
    // g = max(0.0, min(1.0, g));
    // b = max(0.0, min(1.0, b));
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

Ray *Ray::copy()
{
    return new Ray(start->copy(), dir->copy());
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
    /////////////////// BARYCENTRIC COORDINATES ///////////////////
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

    if (beta >= -EPSILON && gamma >= -EPSILON && beta + gamma <= 1 + EPSILON && t > -EPSILON)
        return t;

    return -1;

    // these other two are suggested by copilot
    
    /////////////////// MOLLER TRUMBORE ALGORITHM ///////////////////
    // Point *edge1 = b.subtract(&a);
    // Point *edge2 = c.subtract(&a);

    // Point *h = ray->dir->cross(edge2);
    // double a = edge1->dot(h);

    // if (a > -EPSILON && a < EPSILON)
    //     return -1;

    // double f = 1 / a;
    // Point *s = ray->start->subtract(&this->a);
    // double u = f * s->dot(h);

    // if (u < 0 || u > 1)
    //     return -1;

    // Point *q = s->cross(edge1);
    // double v = f * ray->dir->dot(q);

    // if (v < 0 || u + v > 1)
    //     return -1;

    // double t = f * edge2->dot(q);

    // delete edge1, edge2, h, s, q;

    // if (t > -EPSILON)
    //     return t;

    // return -1;

    /////////////////// PLANE INTERSECTION ///////////////////
    // Point *normal = getNormal(&a);
    // double t = (normal->dot(&a) - normal->dot(ray->start)) / normal->dot(ray->dir);
    // if (t < EPSILON)
    //     return -1;

    // Point *intersection = ray->getPoint(t);

    // Point *edge1 = b.subtract(&a);
    // Point *edge2 = c.subtract(&a);

    // Point *C = intersection->subtract(&a);

    // double dot11 = edge1->dot(edge1);
    // double dot12 = edge1->dot(edge2);
    // double dot22 = edge2->dot(edge2);
    // double dot1c = edge1->dot(C);
    // double dot2c = edge2->dot(C);

    // double invDenom = 1 / (dot11 * dot22 - dot12 * dot12);
    // double u = (dot22 * dot1c - dot12 * dot2c) * invDenom;
    // double v = (dot11 * dot2c - dot12 * dot1c) * invDenom;

    // delete normal, intersection, edge1, edge2, C;

    // if (u >= -EPSILON && v >= -EPSILON && u + v <= 1 + EPSILON)
    //     return t;

    // return -1;
}

Point *Triangle::getNormal(Point *p)
{
    Point *normal = a.subtract(&b)->cross(a.subtract(&c));
    normal->normalize();
    // if (normal->dot(p) < EPSILON)
    //     return normal->multiply(-1);

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
        if (ray->dir->z >= -EPSILON && ray->dir->z <= EPSILON)
            return -1;

        // if the ray is not parallel to XY plane
        double t = (corner1.z - ray->start->z) / ray->dir->z;
        if (t < -EPSILON)
            return -1;

        Point *intersection = ray->getPoint(t);
        if (intersection->x >= corner1.x && intersection->x <= corner2.x && intersection->y >= corner1.y && intersection->y <= corner2.y)
            return t;
    }
    // if the rectangle is parallel to YZ plane
    else if (corner1.x == corner2.x)
    {
        // if the ray is parallel to YZ plane
        if (ray->dir->x >= -EPSILON && ray->dir->x <= EPSILON)
            return -1;

        // if the ray is not parallel to YZ plane
        double t = (corner1.x - ray->start->x) / ray->dir->x;
        if (t < -EPSILON)
            return -1;

        Point *intersection = ray->getPoint(t);
        if (intersection->z >= corner1.z && intersection->z <= corner2.z && intersection->y >= corner1.y && intersection->y <= corner2.y)
            return t;
    }
    // if the rectangle is parallel to XZ plane
    else if (corner1.y == corner2.y)
    {
        // if the ray is parallel to XZ plane
        if (ray->dir->y >= -EPSILON && ray->dir->y <= EPSILON)
            return -1;

        // if the ray is not parallel to XZ plane
        double t = (corner1.y - ray->start->y) / ray->dir->y;
        if (t < -EPSILON)
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
    for (int i = -tileCount/2; i < tileCount/2; i++)
    {
        for (int j = -tileCount/2; j < tileCount/2; j++)
        {
            if ((i + j) % 2 == 0)
                glColor3f(1, 1, 1);
            else
                glColor3f(0, 0, 0);

            glBegin(GL_QUADS);
            glVertex3f(i * tileWidth, j * tileHeight, 0);
            glVertex3f((i + 1) * tileWidth, j * tileHeight, 0);
            glVertex3f((i + 1) * tileWidth, (j + 1) * tileHeight, 0);
            glVertex3f(i * tileWidth, (j + 1) * tileHeight, 0);

            glEnd();
        }
    }
}

double Board::handleIntersecttion(Ray *ray)
{
    // check if the ray intersects with the board
    // board is on the XY plane
    // double t = Rect(Point(-tileCount/2 * width, -tileCount/2 * height, 0), Point(tileCount/2 * width, tileCount/2 * height, 0)).calcIntersection(ray);
    // return t;

    Point *normal = new Point(0, 0, 1);
    if (normal->dot(ray->dir) >= -EPSILON && normal->dot(ray->dir) <= EPSILON)
        return -1;

    double t = (0 - ray->start->z) / ray->dir->z;
    if (t < -EPSILON)
        return -1;

    Point *intersection = ray->getPoint(t);
    if (intersection->x >= -tileCount/2 * tileWidth && intersection->x <= tileCount/2 * tileWidth && intersection->y >= -tileCount/2 * tileHeight && intersection->y <= tileCount/2 * tileHeight)
        return t;

    return -1;
}

Point *Board::getNormal(Point *p, Point *rayDir)
{
    // check if the point is on the board
    // board is on the XY plane
    if (p->z >= -EPSILON && p->z <= EPSILON)
    {
        if (rayDir->z < EPSILON)
            return new Point(0, 0, -1);
        else
            return new Point(0, 0, 1);
    }

    return NULL;
}

Color *Board::getColorAt(Point *p)
{
    // first get the bottom left corner of the board
    double bottomLeftX = -tileCount/2 * tileWidth;
    double bottomLeftY = -tileCount/2 * tileHeight;

    int x = (int)floor((p->x) / tileWidth);
    int y = (int)floor((p->y) / tileHeight);

    if ((x + y) % 2 == 0)
        return new Color(1, 1, 1);
    else
        return new Color(0, 0, 0);
}

Color *Board::getTextureAt(Point *p)
{
    if (!showTexture)
        return NULL;

    int x = (int)floor((p->x) / tileWidth);
    int y = (int)floor((p->y) / tileHeight);

    if ((x + y) % 2 == 0)
    {
        int cellX = ((p->x / tileWidth) - floor(p->x / tileWidth)) * (blackTileColorBuffer[0].size() - 1);
        int cellY = ((p->y / tileHeight) - floor(p->y / tileHeight)) * (blackTileColorBuffer.size() - 1);

        assert(cellX >= 0 && cellX < blackTileColorBuffer[0].size());
        assert(cellY >= 0 && cellY < blackTileColorBuffer.size());

        return blackTileColorBuffer[cellY][cellX]->copy();
    }
    else
    {
        int cellX = ((p->x / tileWidth) - floor(p->x / tileWidth)) * (whiteTileColorBuffer[0].size() - 1);
        int cellY = ((p->y / tileHeight) - floor(p->y / tileHeight)) * (whiteTileColorBuffer.size() - 1);

        assert(cellX >= 0 && cellX < whiteTileColorBuffer[0].size());
        assert(cellY >= 0 && cellY < whiteTileColorBuffer.size());

        return whiteTileColorBuffer[cellY][cellX]->copy();
    }
}

/////////////////////////////// PYRAMID ///////////////////////////////

void Pyramid::calculateAllSides()
{
    if (sideTriangles.size() > 0)
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

    bottomRect = new Rect(*bottom1, *bottom3);

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
    double tMin = -1;
    for (Triangle *triangle : sideTriangles)
    {
        double t = triangle->calcIntersection(ray);
        if (t > -EPSILON && (tMin < 0 || t < tMin))
            tMin = t;
    }

    // check if the ray intersects with the bottom Rect
    double t = bottomRect->calcIntersection(ray);
    if (t > -EPSILON && (tMin < 0 || t < tMin)) {
        cout << "bottom rect" << endl;
        tMin = t;
    }

    return tMin;
}

Point *Pyramid::getNormal(Point *p, Point *rayDir)
{
    calculateAllSides();

    // check if the point is on the bottom Rect
    if (p->z >= lowest.z - EPSILON && p->z <= lowest.z + EPSILON)
        return new Point(0, 0, -1);
    /*&& p->x >= lowest.x - width / sqrt(2) && p->x <= lowest.x + width / sqrt(2) && p->y >= lowest.y - width / sqrt(2) && p->y <= lowest.y + width / sqrt(2)*/

    // check if the point is on the side triangles
    for (Triangle *triangle : sideTriangles)
    {
        // check if the point is on the triangle or not
        Point *normal = triangle->getNormal(p);
        Point *pToA = p->subtract(&triangle->a);
        double dot = normal->dot(pToA);
        if (dot >= -EPSILON && dot <= EPSILON)
        {
            if (normal->dot(rayDir) < EPSILON)
            {
                normal = normal->multiply(-1);
            }

            delete pToA;
            return normal;
        }

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
    double a = 1; // ray->dir->dot(ray->dir)
    double b = 2 * ray->dir->dot(centerToStart);
    double c = centerToStart->dot(centerToStart) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    if (discriminant < EPSILON)
        return -1;

    double t1 = (-b + sqrt(discriminant)) / (2 * a);
    double t2 = (-b - sqrt(discriminant)) / (2 * a);

    if (t1 < EPSILON && t2 < EPSILON)
        return -1;
    else if (t1 < EPSILON)
        return t2;
    else if (t2 < EPSILON)
        return t1;
    else
        return min(t1, t2);
}

Point *Sphere::getNormal(Point *p, Point *rayDir)
{
    // check if the point is on the sphere
    Point *centerToP = p->subtract(&center);
    double dif = centerToP->magnitude() - radius;
    if (dif >= -EPSILON && dif <= EPSILON)
    {
        centerToP->normalize();
        if (centerToP->dot(rayDir) < EPSILON)
            return centerToP->multiply(-1);
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
        if (t > -EPSILON && (tMin < 0 || t < tMin))
            tMin = t;

        delete rect;
    }

    return tMin;
}

Point *Cube::getNormal(Point *p, Point *rayDir)
{
    Point *normal = NULL;
    // check which face the point is on
    // if the point is on the bottom face
    if (bottomLeftFront.z - EPSILON <= p->z && p->z <= bottomLeftFront.z + EPSILON)
        normal = new Point(0, 0, -1);
    // if the point is on the top face
    else if (bottomLeftFront.z + side - EPSILON <= p->z && p->z <= bottomLeftFront.z + side + EPSILON)
        normal = new Point(0, 0, 1);
    // if the point is on the left face
    else if (bottomLeftFront.x - EPSILON <= p->x && p->x <= bottomLeftFront.x + EPSILON)
        normal = new Point(-1, 0, 0);
    // if the point is on the right face
    else if (bottomLeftFront.x + side - EPSILON <= p->x && p->x <= bottomLeftFront.x + side + EPSILON)
        normal = new Point(1, 0, 0);
    // if the point is on the back face
    else if (bottomLeftFront.y - EPSILON <= p->y && p->y <= bottomLeftFront.y + EPSILON)
        normal = new Point(0, -1, 0);
    // if the point is on the front face
    else if (bottomLeftFront.y + side - EPSILON <= p->y && p->y <= bottomLeftFront.y + side + EPSILON)
        normal = new Point(0, 1, 0);

    // check if the normal is pointing towards the ray
    if (normal != NULL && normal->dot(rayDir) < EPSILON)
        normal->multiply(-1);

    return normal;
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