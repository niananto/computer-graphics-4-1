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

    Matrix multiply(Matrix m);
    Point multiply(Point p);
};

Matrix::Matrix(int rows, int cols) {
    this->rows = rows;
    this->cols = cols;
    mat = new double*[rows];
    for (int i = 0; i < rows; i++) {
        mat[i] = new double[cols];
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

Matrix Matrix::multiply(Matrix m) {
    if (cols != m.rows) {
        cout << "Cannot multiply matrices" << endl;
        return Matrix(0, 0);
    }
    double** result = new double*[rows];
    for (int i = 0; i < rows; i++) {
        result[i] = new double[m.cols];
        for (int j = 0; j < m.cols; j++) {
            result[i][j] = 0;
            for (int k = 0; k < cols; k++) {
                result[i][j] += mat[i][k] * m.getEntry(k, j);
            }
        }
    }
    return Matrix(result, rows, m.cols);
}

Point Matrix::multiply(Point p) {
    if (cols != 4) {
        cout << "Cannot multiply matrix by point" << endl;
        return Point();
    }
    double** point = new double*[4];
    for (int i = 0; i < 4; i++) {
        point[i] = new double[1];
        point[i][0] = 0;
    }
    point[0][0] = p.x;
    point[1][0] = p.y;
    point[2][0] = p.z;
    point[3][0] = 1;
    Matrix result = multiply(Matrix(point, 4, 1));
    return Point(result.getEntry(0, 0), result.getEntry(1, 0), result.getEntry(2, 0));
}