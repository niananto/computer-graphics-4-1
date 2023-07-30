#include "point.h"

class Matrix {
private:
    int rows;
    int cols;
    double** mat;
public:
    Matrix(int rows, int cols);
    Matrix(double** mat, int rows, int cols);
    ~Matrix();
    double** get();
    void set(double** val);
    double getEntry(int row, int col);
    void setEntry(int row, int col, double val);
    void print();

    void identity();
    void translation(double tx, double ty, double tz);
    void scaling(double sx, double sy, double sz);
    void rotation(double angle, double x, double y, double z);

    Matrix* multiply(Matrix *m);
    Point* multiply(Point *p);

    Point* R(Point* x, Point* a, double angle);
};

Matrix::Matrix(int rows, int cols) {
    this->rows = rows;
    this->cols = cols;
    mat = new double*[rows];
    for (int i = 0; i < rows; i++) {
        mat[i] = new double[cols];
        for (int j = 0; j < cols; j++) {
            mat[i][j] = 0;
        }
    }
}

Matrix::Matrix(double** mat, int rows, int cols) {
    this->rows = rows;
    this->cols = cols;
    this->mat = new double*[rows];
    for (int i = 0; i < rows; i++) {
        this->mat[i] = new double[cols];
        for (int j = 0; j < cols; j++) {
            this->mat[i][j] = mat[i][j];
        }
    }
}

Matrix::~Matrix() {
    for (int i = 0; i < rows; i++) {
        delete[] mat[i];
    }
    delete[] mat;
}

double** Matrix::get() {
    double** tempMat = new double*[rows];
    for (int i = 0; i < rows; i++) {
        tempMat[i] = new double[cols];
        for (int j = 0; j < cols; j++) {
            tempMat[i][j] = mat[i][j];
        }
    }
    return tempMat;
}

void Matrix::set(double** val) {
    for (int i = 0; i < rows; i++) {
        delete[] mat[i];
    }
    delete[] mat;
    mat = new double*[rows];
    for (int i = 0; i < rows; i++) {
        mat[i] = new double[cols];
        for (int j = 0; j < cols; j++) {
            mat[i][j] = val[i][j];
        }
    }
}

double Matrix::getEntry(int row, int col) {
    return mat[row][col];
}

void Matrix::setEntry(int row, int col, double val) {
    mat[row][col] = val;
}

void Matrix::print() {
    cout << "Matrix: " << endl;
    for (int i = 0; i < rows; i++) {
        cout << "[ ";
        for (int j = 0; j < cols; j++) {
            cout << mat[i][j] << " ";
        }
        cout << "]" << endl;
    }
}

void Matrix::identity() {
    if (rows != cols) {
        cout << "Cannot create identity matrix" << endl;
        return;
    }
    for (int i = 0; i < rows; i++) {
        mat[i][i] = 1;
    }
}

void Matrix::translation(double tx, double ty, double tz) {
    if (rows != 4 || cols != 4) {
        cout << "Cannot create translation matrix" << endl;
        return;
    }
    identity();
    mat[0][3] = tx;
    mat[1][3] = ty;
    mat[2][3] = tz;
}

void Matrix::scaling(double sx, double sy, double sz) {
    if (rows != 4 || cols != 4) {
        cout << "Cannot create scaling matrix" << endl;
        return;
    }
    identity();
    mat[0][0] = sx;
    mat[1][1] = sy;
    mat[2][2] = sz;
}

void Matrix::rotation(double angle, double x, double y, double z) {
    if (rows != 4 || cols != 4) {
        cout << "Cannot create rotation matrix" << endl;
        return;
    }
    identity();

    // angle in radians
    // angle = angle * M_PI / 180;
    // double c = cos(angle);
    // double s = sin(angle);

    // normalize vector
    // double length = sqrt(x*x + y*y + z*z);
    // x = x / length;
    // y = y / length;
    // z = z / length;

    // rodriquez formula
    // mat[0][0] = x*x*(1-c) + c;
    // mat[0][1] = x*y*(1-c) - z*s;
    // mat[0][2] = x*z*(1-c) + y*s;
    // mat[1][0] = y*x*(1-c) + z*s;
    // mat[1][1] = y*y*(1-c) + c;
    // mat[1][2] = y*z*(1-c) - x*s;
    // mat[2][0] = x*z*(1-c) - y*s;
    // mat[2][1] = y*z*(1-c) + x*s;
    // mat[2][2] = z*z*(1-c) + c;

    Point* a = new Point(x, y, z);
    a->normalize();
    Point* c1 = R(new Point(1,0,0), a, angle);
    Point* c2 = R(new Point(0,1,0), a, angle);
    Point* c3 = R(new Point(0,0,1), a, angle);

    mat[0][0] = c1->x;
    mat[0][1] = c2->x;
    mat[0][2] = c3->x;
    mat[1][0] = c1->y;
    mat[1][1] = c2->y;
    mat[1][2] = c3->y;
    mat[2][0] = c1->z;
    mat[2][1] = c2->z;
    mat[2][2] = c3->z;

    delete a, c1, c2, c3;
}

Matrix* Matrix::multiply(Matrix* m) {
    if (cols != m->rows) {
        cout << "Cannot multiply matrices" << endl;
        return new Matrix(0, 0);
    }
    double** result = new double*[rows];
    for (int i = 0; i < rows; i++) {
        result[i] = new double[m->cols];
        for (int j = 0; j < m->cols; j++) {
            result[i][j] = 0;
            for (int k = 0; k < cols; k++) {
                result[i][j] += mat[i][k] * m->getEntry(k, j);
            }
        }
    }
    return new Matrix(result, rows, m->cols);
}

Point* Matrix::multiply(Point* p) {
    if (cols != 4) {
        cout << "Cannot multiply matrix by point" << endl;
        return new Point();
    }
    double** point = new double*[4];
    for (int i = 0; i < 4; i++) {
        point[i] = new double[1];
        point[i][0] = 0;
    }
    point[0][0] = p->x;
    point[1][0] = p->y;
    point[2][0] = p->z;
    point[3][0] = 1;
    Matrix* result = multiply(new Matrix(point, 4, 1));

    double x = result->mat[0][0];
    double y = result->mat[1][0];
    double z = result->mat[2][0];

    // scale back to 1
    if (result->mat[3][0] != 0) {
      x = x / result->mat[3][0];
      y = y / result->mat[3][0];
      z = z / result->mat[3][0];
    }

    return new Point(x, y, z);
}

Point* Matrix::R(Point* x, Point* a, double angle) {
/*
Point *temp=new Point(x->x,x->y,x->z,x->w);
temp->multiply(cos(angle));
Point *cross=a->crossProduct(x);
cross->multiply(sin(angle));
temp->add(cross);
delete cross;
Point *temp2=new Point(a->x,a->y,a->z,a->w);
temp2->multiply(a->dotProduct(x)*(1-cos(angle)));
temp->add(temp2);
delete temp2;
return temp;
*/

    double c = cos(angle * M_PI/180);
    double s = sin(angle * M_PI/180);

    Point* temp1 = x->multiply(c);
    Point* temp2 = a->multiply(a->dot(x))->multiply(1-c);
    Point* temp3 = a->cross(x)->multiply(s);

    return temp1->add(temp2)->add(temp3);
}