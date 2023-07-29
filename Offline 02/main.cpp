#include <iostream>
#include <stack>
#include <cmath>
#include "matrix.h"

using namespace std;

void stage1() {
  freopen("scene.txt", "r", stdin);
  freopen("stage1.txt", "w", stdout);

  // ignore the first 4 lines
  string line;
  for (int i = 0; i < 4; i++) {
    getline(cin, line);
  }

  // initialize empty stack S
  stack<Matrix> S;
  // initialize M = Identity matrix
  Matrix M(4, 4);
  for (int i = 0; i < 4; i++) {
    M.setEntry(i, i, 1);
  }

  while (true) {
    string command;
    cin >> command;

/*
if command = “triangle”
read three points
for each three-point P
P’ <- transformPoint(M, P)
output P’
else if command = “translate”
read translation amounts
generate the corresponding translation matrix T
M = product(M, T)
else if command = “scale”
read scaling factors
generate the corresponding scaling matrix T
M = product(M, T)
else if command = “rotate”
read rotation angle and axis
generate the corresponding rotation matrix T
M = product(M, T)
else if command = “push”
S.push(M)
else if command =“pop”
// do it yourself
else if command =“end”
break
*/

    if (command == "triangle") {
      // read three points
      Point* oldPoints = new Point[3];
      for (int i = 0; i < 3; i++) {
        cin >> oldPoints[i].x >> oldPoints[i].y >> oldPoints[i].z;
      }

      // for each three-point P
      // P’ <- transformPoint(M, P)
      // output P’
      Point* newPoints = new Point[3];
      for (int i = 0; i < 3; i++) {
        newPoints[i] = M.multiply(oldPoints[i]);
        cout << newPoints[i].x << " " << newPoints[i].y << " " << newPoints[i].z << endl;
      }
    }
    else if (command == "translate") {
      // read translation amounts
      double tx, ty, tz;
      cin >> tx >> ty >> tz;

      // generate the corresponding translation matrix T
      Matrix T(4, 4);
      for (int i = 0; i < 4; i++) {
        T.setEntry(i, i, 1);
      }
      T.setEntry(0, 3, tx);
      T.setEntry(1, 3, ty);
      T.setEntry(2, 3, tz);

      // M = product(M, T)
      M = M.multiply(T);

      // cout << "M: " << endl;
      // M.print();
    }
    else if (command == "scale") {
      // read scaling factors
      double sx, sy, sz;
      cin >> sx >> sy >> sz;

      // generate the corresponding scaling matrix T
      Matrix T(4, 4);
      T.setEntry(0, 0, sx);
      T.setEntry(1, 1, sy);
      T.setEntry(2, 2, sz);
      T.setEntry(3, 3, 1);

      // M = product(M, T)
      M = M.multiply(T);

      // cout << "M: " << endl;
      // M.print();
    }
    else if (command == "rotate") {
      // read rotation angle and axis
      double angle, x, y, z;
      cin >> angle >> x >> y >> z;

      // generate the corresponding rotation matrix T
      Matrix T(4, 4);
      for (int i = 0; i < 4; i++) {
        T.setEntry(i, i, 1);
      }
      T.setEntry(0, 0, cos(angle) + x*x*(1-cos(angle)));
      T.setEntry(0, 1, x*y*(1-cos(angle)) - z*sin(angle));
      T.setEntry(0, 2, x*z*(1-cos(angle)) + y*sin(angle));
      T.setEntry(1, 0, y*x*(1-cos(angle)) + z*sin(angle));
      T.setEntry(1, 1, cos(angle) + y*y*(1-cos(angle)));
      T.setEntry(1, 2, y*z*(1-cos(angle)) - x*sin(angle));
      T.setEntry(2, 0, z*x*(1-cos(angle)) - y*sin(angle));
      T.setEntry(2, 1, z*y*(1-cos(angle)) + x*sin(angle));
      T.setEntry(2, 2, cos(angle) + z*z*(1-cos(angle)));

      // M = product(M, T)
      M = M.multiply(T);

      // cout << "M: " << endl;
      // M.print();
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
}

void stage2() {

}

void stage3() {

}

int main(int argc, char** argv) {
  stage1();
  // stage2();
  // stage3();

  return 0;
}