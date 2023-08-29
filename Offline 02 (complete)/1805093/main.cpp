#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <cmath>
#include <iomanip>
#include <vector>
#include "point.h"
#include "matrix.h"
#include "color.h"
#include "triangle.h"
#include "bitmap_image.hpp"

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

void stage4() {
  ifstream input("config.txt");
  double screenWidth, screenHeight;
  input >> screenWidth >> screenHeight;
  input.close();

  input.open("stage3.txt");
  vector<Triangle> triangles;

  string line;
  while (getline(input, line)) {
    Point* p1 = new Point();
    Point* p2 = new Point();
    Point* p3 = new Point();
    stringstream ss(line);
    ss >> p1->x >> p1->y >> p1->z;
    getline(input, line);
    ss = stringstream(line);
    ss >> p2->x >> p2->y >> p2->z;
    getline(input, line);
    ss = stringstream(line);
    ss >> p3->x >> p3->y >> p3->z;
    getline(input, line);

    Triangle t = Triangle(p1, p2, p3);
    t.getRandomColor();
    triangles.push_back(t);
  }
  input.close();

  // initialize max_x, max_y, min_x, min_y,
  double max_x = 1, max_y = 1, min_x = -1, min_y = -1;
  double z_max = 1, z_front_limit = -1;

  // initialize z buffer and frame buffer
  vector<vector<double>> zBuffer;
  vector<vector<Color>> frameBuffer;
  for (int i = 0; i < screenHeight; i++) {
    vector<double> zRow;
    vector<Color> frameRow;
    for (int j = 0; j < screenWidth; j++) {
      zRow.push_back(z_max);
      frameRow.push_back(Color(0, 0, 0)); // black
    }
    zBuffer.push_back(zRow);
    frameBuffer.push_back(frameRow);
  }

  double dx = (max_x - min_x) / screenWidth;
  double dy = (max_y - min_y) / screenHeight;
  double top_y = max_y - dy / 2.0;
  double bottom_y = min_y + dy / 2.0;
  double left_x = min_x + dx / 2.0;
  double right_x = max_x - dx / 2.0;

  // loop over all triangles
  for (int i = 0; i < triangles.size(); i++) {
    Triangle t = triangles[i];

    // Find top_scanline and bottom_scanline after necessary clipping
    double top_scanline = max(t.p[0]->y, max(t.p[1]->y, t.p[2]->y));
    double bottom_scanline = min(t.p[0]->y, min(t.p[1]->y, t.p[2]->y));

    // clipping
    if (top_scanline < bottom_y || bottom_scanline > top_y) {
      continue;
    }
    if (top_scanline > top_y) {
      top_scanline = top_y;
    }
    if (bottom_scanline < bottom_y) {
      bottom_scanline = bottom_y;
    }

    int start_row = (int)round((top_y - top_scanline) / dy);
    int end_row = (int)round((top_y - bottom_scanline) / dy);

    // for row_no from start_row to end_row
    for (double row_no = start_row; row_no <= end_row; row_no++) {
      double y = top_y - row_no * dy;

      // get left_scanline and right_scanline
      vector<double> left_right_scanlines;
      for (int k=0; k<3; k++) {
        int j = (k+1) % 3;
        // ignore if they have the same y
        if (t.p[k]->y == t.p[j]->y) {
          continue;
        }
        // check if current y is in between their y
        if (y > min(t.p[k]->y, t.p[j]->y) && y < max(t.p[k]->y, t.p[j]->y)) {
          double tt = (y - t.p[k]->y) / (t.p[j]->y - t.p[k]->y);
          double x = t.p[k]->x + tt * (t.p[j]->x - t.p[k]->x);
          left_right_scanlines.push_back(x);
        }
      }

      if(left_right_scanlines.size() != 2) {
        continue;
      }
      double left_scanline = min(left_right_scanlines[0], left_right_scanlines[1]);
      double right_scanline = max(left_right_scanlines[0], left_right_scanlines[1]);

      // clipping
      if (left_scanline > right_x || right_scanline < left_x) {
        continue;
      }
      if (left_scanline < left_x) {
        left_scanline = left_x;
      }
      if (right_scanline > right_x) {
        right_scanline = right_x;
      }

      int start_col = (int)round((left_scanline - left_x) / dx);
      int end_col = (int)round((right_scanline - left_x) / dx);

      // for col_no from start_col to end_col
      for (double col_no = start_col; col_no <= end_col; col_no++) {
        double x = left_x + col_no * dx;
        double z = t.getZ(x, y);

        // If z is less than zBuffer[row_no][col_no], then
        // update zBuffer[row_no][col_no] and frameBuffer[row_no][col_no]
        if (z < zBuffer[row_no][col_no] && z > z_front_limit) {
          zBuffer[row_no][col_no] = z;
          frameBuffer[row_no][col_no] = t.color;
        }
      }
    }
  }

  // write to file
  ofstream output("z-buffer.txt");
  bitmap_image image(screenWidth, screenHeight);

  for (int i = 0; i < screenHeight; i++) {
    for (int j = 0; j < screenWidth; j++) {
      if (zBuffer[i][j] < z_max) {
        output << fixed << setprecision(6) << zBuffer[i][j] << "\t";
      }
      image.set_pixel(j, i, frameBuffer[i][j].red, frameBuffer[i][j].green, frameBuffer[i][j].blue);
    }
    output << endl;
  }
  output.close();
  image.save_image("out.bmp");
}

int main(int argc, char** argv) {
  stage1();
  stage2();
  stage3();
  stage4();

  return 0;
}