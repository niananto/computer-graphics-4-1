#include <iostream>
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
