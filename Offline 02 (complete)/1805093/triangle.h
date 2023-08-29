
static unsigned long int g_seed = 1; 
inline int g_random() 
{ 
    g_seed = (214013 * g_seed + 2531011); 
    return (g_seed >> 16) & 0x7FFF; 
}

class Triangle {
public:
  Point **p;
  Color color;
  Triangle();
  Triangle(Point* a, Point* b, Point* c);
  void getRandomColor();
  // bool isInside(double x, double y);
  double getZ(double x, double y);
  ~Triangle();
};

Triangle::Triangle() {
  this->p = new Point*[3];
  for (int i = 0; i < 3; i++) {
    this->p[i] = new Point();
  }
}

Triangle::Triangle(Point* a, Point* b, Point* c) {
  this->p = new Point*[3];
  this->p[0] = a;
  this->p[1] = b;
  this->p[2] = c;
}

void Triangle::getRandomColor() {
  this->color.red = g_random() % 256;
  this->color.green = g_random() % 256;
  this->color.blue = g_random() % 256;
}

// bool Triangle::isInside(double x, double y) {
//   // assuming a, b, c is in counter clockwise order
//   double x1 = this->p[0]->x;
//   double y1 = this->p[0]->y;
//   double x2 = this->p[1]->x;
//   double y2 = this->p[1]->y;
//   double x3 = this->p[2]->x;
//   double y3 = this->p[2]->y;

//   double denominator = ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3));
//   if (denominator == 0) {
//     return false;
//   }

//   double alpha = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / denominator;
//   double beta = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / denominator;
//   double gamma = 1.0 - alpha - beta;
//   return (alpha > 0 && alpha < 1) && (beta > 0 && beta < 1) && (gamma > 0 && gamma < 1);
// }

double Triangle::getZ(double x, double y) {
  //find normal of this triangle's plane
  Point* a = p[1]->subtract(p[0]);
  Point* b = p[2]->subtract(p[0]);
  Point* normal = a->cross(b);
  normal->normalize();

  //find d
  double d = -(normal->x * p[0]->x + normal->y * p[0]->y + normal->z * p[0]->z);
  double z = -(normal->x * x + normal->y * y + d) / normal->z;
  return z;
}

Triangle::~Triangle() {
  // for (int i = 0; i < 3; i++) {
  //   delete this->p[i];
  // }
  // delete[] this->p;
}