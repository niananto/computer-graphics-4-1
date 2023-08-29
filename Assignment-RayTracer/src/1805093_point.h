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