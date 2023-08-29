class Color {
public:
  int red, green, blue;
  Color();
  Color(int red, int green, int blue);
  ~Color();
};

Color::Color() {
  this->red = 0;
  this->green = 0;
  this->blue = 0;
}

Color::Color(int red, int green, int blue) {
  this->red = red;
  this->green = green;
  this->blue = blue;
}

Color::~Color() {
}