#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <cmath>
#include <iomanip>
#include "matrix.h"

using namespace std;

void stage1() {
  ifstream input("scene.txt");
  ofstream output("stage1.txt");

  // ignore the first 4 lines
  string _;
  for (int i = 0; i < 4; i++) {
    getline(input, _);
  }

  // initialize empty stack S
  stack<Matrix*> S;
  // initialize M = Identity matrix
  Matrix* M = new Matrix(4, 4);
  M->identity();

  while (true) {
    string command;
    input >> command;

    if (command == "triangle") {
      // read three points
      Point** oldPoints = new Point*[3];
      for (int i = 0; i < 3; i++) {
        oldPoints[i] = new Point();
        input >> oldPoints[i]->x >> oldPoints[i]->y >> oldPoints[i]->z;
      }

      // for each three-point P
      // P’ <- transformPoint(M, P)
      // output P’
      Point** newPoints = new Point*[3];
      for (int i = 0; i < 3; i++) {
        newPoints[i] = M->multiply(oldPoints[i]);
        // print with decimal precesions
        // use setprecision
        output << fixed << setprecision(7) << newPoints[i]->x << " " << newPoints[i]->y << " " << newPoints[i]->z << endl;
      }
      output << endl;
    }
    else if (command == "translate") {
      // read translation amounts
      double tx, ty, tz;
      input >> tx >> ty >> tz;

      // generate the corresponding translation matrix T
      Matrix* T = new Matrix(4, 4);
      T->translation(tx, ty, tz);

      // M = product(M, T)
      M = M->multiply(T);

      // output << "M: " << endl;
      // M->print();
    }
    else if (command == "scale") {
      // read scaling factors
      double sx, sy, sz;
      input >> sx >> sy >> sz;

      // generate the corresponding scaling matrix T
      Matrix* T = new Matrix(4, 4);
      T->scaling(sx, sy, sz);

      // M = product(M, T)
      M = M->multiply(T);

      // output << "M: " << endl;
      // M->print();
    }
    else if (command == "rotate") {
      // read rotation angle and axis
      double angle, x, y, z;
      input >> angle >> x >> y >> z;

      // generate the corresponding rotation matrix T
      Matrix* T = new Matrix(4, 4);
      T->rotation(angle, x, y, z);

      // M = product(M, T)
      M = M->multiply(T);

      // output << "M: " << endl;
      // M->print();
    }
    else if (command == "push") {
      // S.push(M)
      S.push(M);
    }
    else if (command == "pop") {
      // M = S.pop()
      M = S.top();
      S.pop();
    }
    else if (command == "end") {
      break;
    }
  }

  input.close();
  output.close();
}

void stage2() {
  ifstream input("scene.txt");

  Point* eye = new Point();
  Point* look = new Point();
  Point* up = new Point();

  input >> eye->x >> eye->y >> eye->z;
  input >> look->x >> look->y >> look->z;
  input >> up->x >> up->y >> up->z;

  Point* l = look->subtract(eye);
  l->normalize();
  Point* r = l->cross(up);
  r->normalize();
  Point* u = r->cross(l);

  Matrix* T = new Matrix(4, 4);
  T->translation(-eye->x, -eye->y, -eye->z);

  Matrix* R = new Matrix(4, 4);
  R->setEntry(0, 0, r->x);
  R->setEntry(0, 1, r->y);
  R->setEntry(0, 2, r->z);
  R->setEntry(1, 0, u->x);
  R->setEntry(1, 1, u->y);
  R->setEntry(1, 2, u->z);
  R->setEntry(2, 0, -l->x);
  R->setEntry(2, 1, -l->y);
  R->setEntry(2, 2, -l->z);

  Matrix* V = R->multiply(T);

  // output << "V: " << endl;
  // V->print();

  input.close();
  input.open("stage1.txt");
  ofstream output("stage2.txt", ios::trunc);

  // the points in stage1.txt are transformed by V and written in stage2.txt
  // take input the triangle points and transform them
  // output the transformed points
  string line;
  while (getline(input, line)) {
    for (int i=0; i<3; i++) {
      // get the point
      Point* oldPoint = new Point();
      stringstream ss(line);
      ss >> oldPoint->x >> oldPoint->y >> oldPoint->z;

      // transform the point
      Point* newPoint = V->multiply(oldPoint);
      output << fixed << setprecision(7) << newPoint->x << " " << newPoint->y << " " << newPoint->z << endl;

      // get the next line
      getline(input, line);
    }
    output << endl;
  }

  input.close();
  output.close();
}

void stage3() {
  ifstream input("scene.txt");
  string _;
  for (int i = 0; i < 3; i++) {
    getline(input, _);
  }

  double fovY, aspectRatio, near, far;
  input >> fovY >> aspectRatio >> near >> far;
  input.close();

  double fovX = fovY * aspectRatio;
  double t = near * tan(fovY * M_PI / 360);
  double r = near * tan(fovX * M_PI / 360);

  Matrix* P = new Matrix(4, 4);
  P->setEntry(0, 0, near / r);
  P->setEntry(1, 1, near / t);
  P->setEntry(2, 2, -(far + near) / (far - near));
  P->setEntry(2, 3, -(2 * far * near) / (far - near));
  P->setEntry(3, 2, -1);

  // cout << "P: " << endl;
  // P->print();

  input.open("stage2.txt");
  ofstream output("stage3.txt", ios::trunc);

  // the points in stage2.txt are transformed by P and written in stage3.txt  
  string line;
  while (getline(input, line)) {
    for (int i=0; i<3; i++) {
      // get the point
      Point* oldPoint = new Point();
      stringstream ss(line);
      ss >> oldPoint->x >> oldPoint->y >> oldPoint->z;

      // transform the point
      Point* newPoint = P->multiply(oldPoint);
      output << fixed << setprecision(7) << newPoint->x << " " << newPoint->y << " " << newPoint->z << endl;

      // get the next line
      getline(input, line);
    }
    output << endl;
  }

  input.close();
  output.close();
}

int main(int argc, char** argv) {
  stage1();
  stage2();
  stage3();

  return 0;
}