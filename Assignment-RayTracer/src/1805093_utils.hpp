#include <fstream>
#include <vector>
#include <cmath>
#include "bitmap_image.hpp"

using namespace std;

extern int nearPlane, farPlane, fovY, fovX, aspectRatio;
extern int recursionLevel, imageWidth, imageHeight;
extern vector<Object *> objects;
extern vector<LightSource *> lights;

extern Point *pos;    // position of the eye
extern Point *look;   // look/forward direction
extern Point *r8;     // right direction - dynamically updated in the display function
extern Point *up;     // up direction
extern Point *center; // center of the scene - temp use

extern boolean showTexture;
extern vector<vector<Color *>> whiteTileColorBuffer;
extern vector<vector<Color *>> blackTileColorBuffer;

void getInputs()
{
    ifstream input("description.txt");

    input >> nearPlane >> farPlane >> fovY >> aspectRatio;
    fovX = fovY * aspectRatio;
    input >> recursionLevel >> imageWidth;
    imageHeight = imageWidth;

    Board *board = new Board();
    board->objectType = "board";
    input >> board->tileWidth;
    board->tileHeight = board->tileWidth;
    input >> board->lightCoefficients.ambient >> board->lightCoefficients.diffuse >> board->lightCoefficients.reflection;
    board->lightCoefficients.specular = 0;
    board->color = Color(1, 1, 1);
    board->shininess = 0;
    board->tileCount = 200;
    objects.push_back(board);

    int noOfObjects;
    input >> noOfObjects;

    for (int i = 0; i < noOfObjects; i++)
    {
        string objectType;
        input >> objectType;

        if (objectType == "sphere")
        {
            Sphere *sphere = new Sphere();
            sphere->objectType = objectType;
            input >> sphere->center.x >> sphere->center.y >> sphere->center.z;
            input >> sphere->radius;
            input >> sphere->color.r >> sphere->color.g >> sphere->color.b;
            input >> sphere->lightCoefficients.ambient >> sphere->lightCoefficients.diffuse >> sphere->lightCoefficients.specular >> sphere->lightCoefficients.reflection;
            input >> sphere->shininess;
            objects.push_back(sphere);
        }
        else if (objectType == "pyramid")
        {
            Pyramid *pyramid = new Pyramid();
            pyramid->objectType = objectType;
            input >> pyramid->lowest.x >> pyramid->lowest.y >> pyramid->lowest.z;
            input >> pyramid->width >> pyramid->height;
            input >> pyramid->color.r >> pyramid->color.g >> pyramid->color.b;
            input >> pyramid->lightCoefficients.ambient >> pyramid->lightCoefficients.diffuse >> pyramid->lightCoefficients.specular >> pyramid->lightCoefficients.reflection;
            input >> pyramid->shininess;
            objects.push_back(pyramid);
        }
        else if (objectType == "cube")
        {
            Cube *cube = new Cube();
            cube->objectType = objectType;
            input >> cube->bottomLeftFront.x >> cube->bottomLeftFront.y >> cube->bottomLeftFront.z;
            input >> cube->side;
            input >> cube->color.r >> cube->color.g >> cube->color.b;
            input >> cube->lightCoefficients.ambient >> cube->lightCoefficients.diffuse >> cube->lightCoefficients.specular >> cube->lightCoefficients.reflection;
            input >> cube->shininess;
            objects.push_back(cube);
        }
    }

    int noOfNormalLights;
    input >> noOfNormalLights;

    for (int i = 0; i < noOfNormalLights; i++)
    {
        NormalLightSource *normal = new NormalLightSource();
        normal->lightType = "normal";
        input >> normal->position.x >> normal->position.y >> normal->position.z;
        input >> normal->falloff;
        lights.push_back(normal);
    }

    int noOfSpotLights;
    input >> noOfSpotLights;

    for (int i = 0; i < noOfSpotLights; i++)
    {
        SpotLightSource *spot = new SpotLightSource();
        spot->lightType = "spot";
        input >> spot->position.x >> spot->position.y >> spot->position.z;
        input >> spot->falloff;
        Point *temp = new Point();
        input >> temp->x >> temp->y >> temp->z;
        spot->direction = *(temp->subtract(&(spot->position)));
        spot->direction.normalize();
        delete temp;
        input >> spot->cutoffAngle;
        lights.push_back(spot);
    }

    input.close();
}

void generateBmp()
{
    static int imgCount = 1;

    Point *nearMidpoint = pos->add(look->multiply(nearPlane));

    double height = 2 * nearPlane * tan((fovY / 2) * (M_PI / 180));
    double width = 2 * nearPlane * tan((fovX / 2) * (M_PI / 180));

    // declare pointBuffer
    vector<vector<Point *>> pointBuffer;
    double dx = width / (double)imageWidth;
    double dy = height / (double)imageHeight;

    // get the bottom left mid point
    Point *topLeftMid = nearMidpoint->subtract(r8->multiply(width / 2.0));
    topLeftMid = topLeftMid->add(up->multiply(height / 2.0));
    topLeftMid = topLeftMid->add(r8->multiply(dx / 2));
    topLeftMid = topLeftMid->subtract(up->multiply(dy / 2));

    for (int i = 0; i < imageHeight; i++)
    {
        vector<Point *> row;
        for (int j = 0; j < imageWidth; j++)
        {
            Point *point = topLeftMid->add(r8->multiply(j * dx));
            point = point->subtract(up->multiply(i * dy));
            row.push_back(point);
        }
        pointBuffer.push_back(row);
    }

    // declare colorBuffer
    vector<vector<Color *>> colorBuffer(imageHeight, vector<Color *>(imageWidth));

    for (int i = 0; i < imageHeight; i++)
    {
        for (int j = 0; j < imageWidth; j++)
        {
            Ray *ray = new Ray(pointBuffer[i][j]->copy(), pointBuffer[i][j]->subtract(pos));

            double tMin = -1;
            Object *nearestObject = NULL;
            for (int k = 0; k < objects.size(); k++)
            {
                double t = objects[k]->handleIntersecttion(ray);
                if (t > 0 && (tMin < 0 || t < tMin))
                {
                    tMin = t;
                    nearestObject = objects[k];
                }
            }

            Color *color;
            if (nearestObject == NULL || tMin > farPlane) // no intersection or intersection beyond far plane
            {
                color = new Color(0, 0, 0);
            }
            else
            {
                Point *intersectionPoint = ray->getPoint(tMin);
                color = nearestObject->recIntersection(ray, intersectionPoint, tMin, recursionLevel);
                delete intersectionPoint;
            }
            colorBuffer[i][j] = color;
            delete ray;
        }

        // print loading percentage
        if (i % 100 == 0)
        {
            cout << "loading: " << (i * 100) / imageHeight << "%" << endl;
        }
    }

    bitmap_image bmpFile(imageWidth, imageHeight);
    for (int i = 0; i < imageHeight; i++)
    {
        for (int j = 0; j < imageWidth; j++)
        {
            colorBuffer[i][j]->adjust();
            bmpFile.set_pixel(j, i, 255 * colorBuffer[i][j]->r, 255 * colorBuffer[i][j]->g, 255 * colorBuffer[i][j]->b);
        }
    }

    bmpFile.save_image("images/out" + to_string(imgCount) + ".bmp");
    imgCount++;

    cout << "image generated" << endl;

    // free memory
    for (int i = 0; i < imageHeight; i++)
    {
        for (int j = 0; j < imageWidth; j++)
        {
            delete pointBuffer[i][j];
            delete colorBuffer[i][j];
        }
    }
}

void loadTextureIntoBuffer(vector< vector<Color*> > &buffer, string imageName)
{
    bitmap_image image(imageName);
    assert(image);

    const unsigned int height = image.height();
    const unsigned int width = image.width();

    buffer = vector< vector<Color*> >(width, vector<Color*>(height));

    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            unsigned char red, green, blue;
            image.get_pixel(x, y, red, green, blue);

            Color *color = new Color(red / 255.0, green / 255.0, blue / 255.0);
            buffer[x][y] = color;
        }
    }
}

void getTextureInputs(vector< vector<Color*> > &whiteBuffer, vector< vector<Color*> > &blackBuffer)
{
    loadTextureIntoBuffer(whiteBuffer, "images/texture_w.bmp");
    loadTextureIntoBuffer(blackBuffer, "images/texture_b.bmp");
}