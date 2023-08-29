#include <iostream>
#include <cmath>
using namespace std;

class Point {
public:
    double x;
    double y;
    double z;
    Point();
    Point(double x, double y, double z);
    ~Point();
    void set(double x, double y, double z);
    void print();

    void normalize();
    Point* add(Point *p);
    Point* subtract(Point *p);
    Point* multiply(double scalar);
    Point* cross(Point *p);
    double dot(Point *p);
};

Point::Point() {
    x = 0;
    y = 0;
    z = 0;
}

Point::Point(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Point::~Point() {
}

void Point::set(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void Point::print() {
    cout << "(" << x << ", " << y << ", " << z << ")" << endl;
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