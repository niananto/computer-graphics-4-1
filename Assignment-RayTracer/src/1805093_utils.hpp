#include <fstream>
#include <vector>
#include <cmath>
#include "bitmap_image.hpp"

using namespace std;

extern int nearPlane, farPlane, fovY, fovX, aspectRatio;
extern int recursionLevel, imageWidth, imageHeight;
extern int checkerBoardWidth, checkerBoardHeight;
extern vector<double> checkerBoardCoeff;
extern vector<Object *> objects;
extern vector<LightSource *> lights;

extern Point *pos;    // position of the eye
extern Point *look;   // look/forward direction
extern Point *r8;     // right direction - dynamically updated in the display function
extern Point *up;     // up direction
extern Point *center; // center of the scene - temp use

void getInputs()
{
    ifstream input("description.txt");

    input >> nearPlane >> farPlane >> fovY >> aspectRatio;
    fovX = fovY * aspectRatio;
    input >> recursionLevel >> imageWidth;
    imageHeight = imageWidth;

    input >> checkerBoardWidth;
    checkerBoardHeight = checkerBoardWidth;
    input >> checkerBoardCoeff[0] >> checkerBoardCoeff[1] >> checkerBoardCoeff[2];

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
            input >> cube->bottomLeft.x >> cube->bottomLeft.y >> cube->bottomLeft.z;
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
        input >> spot->direction.x >> spot->direction.y >> spot->direction.z;
        input >> spot->cutoffAngle;
        lights.push_back(spot);
    }

    input.close();

    // // output all the inputs taken

    // cout << "nearPlane: " << nearPlane << endl;
    // cout << "farPlane: " << farPlane << endl;
    // cout << "fovY: " << fovY << endl;
    // cout << "aspectRatio: " << aspectRatio << endl;
    // cout << "recursionLevel: " << recursionLevel << endl;
    // cout << "imageWidth: " << imageWidth << endl;
    // cout << "imageHeight: " << imageHeight << endl;
    // cout << "checkerBoardWidth: " << checkerBoardWidth << endl;
    // cout << "checkerBoardHeight: " << checkerBoardHeight << endl;
    // cout << "checkerBoardCoeff: " << checkerBoardCoeff[0] << " " << checkerBoardCoeff[1] << " " << checkerBoardCoeff[2] << endl;

    // cout << "noOfObjects: " << noOfObjects << endl;
    // for (int i = 0; i < noOfObjects; i++)
    // {
    //     cout << objects[i]->objectType << endl;
    //     if (objects[i]->objectType == "sphere")
    //     {
    //         Sphere *sphere = (Sphere *)objects[i];
    //         cout << sphere->center.x << " " << sphere->center.y << " " << sphere->center.z << endl;
    //         cout << sphere->radius << endl;
    //         cout << sphere->color.r << " " << sphere->color.g << " " << sphere->color.b << endl;
    //         cout << sphere->lightCoefficients.ambient << " " << sphere->lightCoefficients.diffuse << " " << sphere->lightCoefficients.specular << " " << sphere->lightCoefficients.reflection << endl;
    //         cout << sphere->shininess << endl;
    //     }
    //     else if (objects[i]->objectType == "pyramid")
    //     {
    //         Pyramid *pyramid = (Pyramid *)objects[i];
    //         cout << pyramid->lowest.x << " " << pyramid->lowest.y << " " << pyramid->lowest.z << endl;
    //         cout << pyramid->width << " " << pyramid->height << endl;
    //         cout << pyramid->color.r << " " << pyramid->color.g << " " << pyramid->color.b << endl;
    //         cout << pyramid->lightCoefficients.ambient << " " << pyramid->lightCoefficients.diffuse << " " << pyramid->lightCoefficients.specular << " " << pyramid->lightCoefficients.reflection << endl;
    //         cout << pyramid->shininess << endl;
    //     }
    //     else if (objects[i]->objectType == "cube")
    //     {
    //         Cube *cube = (Cube *)objects[i];
    //         cout << cube->bottomLeft.x << " " << cube->bottomLeft.y << " " << cube->bottomLeft.z << endl;
    //         cout << cube->side << endl;
    //         cout << cube->color.r << " " << cube->color.g << " " << cube->color.b << endl;
    //         cout << cube->lightCoefficients.ambient << " " << cube->lightCoefficients.diffuse << " " << cube->lightCoefficients.specular << " " << cube->lightCoefficients.reflection << endl;
    //         cout << cube->shininess << endl;
    //     }
    // }

    // cout << "noOfNormalLights: " << noOfNormalLights << endl;
    // cout << "noOfSpotLights: " << noOfSpotLights << endl;
    // for (int i = 0; i < lights.size(); i++)
    // {
    //     cout << lights[i]->lightType << endl;
    //     if (lights[i]->lightType == "normal")
    //     {
    //         NormalLightSource *normal = (NormalLightSource *)lights[i];
    //         cout << normal->position.x << " " << normal->position.y << " " << normal->position.z << endl;
    //         cout << normal->falloff << endl;
    //     }
    //     else if (lights[i]->lightType == "spot")
    //     {
    //         SpotLightSource *spot = (SpotLightSource *)lights[i];
    //         cout << spot->position.x << " " << spot->position.y << " " << spot->position.z << endl;
    //         cout << spot->falloff << endl;
    //         cout << spot->direction.x << " " << spot->direction.y << " " << spot->direction.z << endl;
    //         cout << spot->cutoffAngle << endl;
    //     }
    // }
}

void generateBmp()
{
    static int imgCount = 1;
    // cout << "pos: " << pos->x << " " << pos->y << " " << pos->z << endl;
    // cout << "look: " << look->x << " " << look->y << " " << look->z << endl;
    // cout << "r8: " << r8->x << " " << r8->y << " " << r8->z << endl;
    // cout << "up: " << up->x << " " << up->y << " " << up->z << endl;

    Point *nearMidpoint = pos->add(look->multiply(nearPlane));
    // cout << "nearMidpoint: " << nearMidpoint->x << " " << nearMidpoint->y << " " << nearMidpoint->z << endl;

    double height = 2 * nearPlane * tan((fovY / 2) * (M_PI / 180));
    double width = 2 * nearPlane * tan((fovX / 2) * (M_PI / 180));

    // declare pointBuffer
    vector<vector<Point *>> pointBuffer;
    double dx = width / (double)imageWidth;
    double dy = height / (double)imageHeight;

    // cout << "height: " << height << endl;
    // cout << "width: " << width << endl;
    // cout << "dx: " << dx << endl;
    // cout << "dy: " << dy << endl;

    // get the bottom left mid point
    Point *topLeftMid = nearMidpoint->subtract(r8->multiply(width / 2.0));
    topLeftMid = topLeftMid->add(up->multiply(height / 2.0));
    topLeftMid = topLeftMid->add(r8->multiply(dx / 2));
    topLeftMid = topLeftMid->subtract(up->multiply(dy / 2));
    // cout << "topLeftMid: " << topLeftMid->x << " " << topLeftMid->y << " " << topLeftMid->z << endl;

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
    vector<vector<Color *>> colorBuffer;
    for (int i = 0; i < imageHeight; i++)
    {
        vector<Color *> row;
        for (int j = 0; j < imageWidth; j++)
        {
            row.push_back(new Color());
        }
        colorBuffer.push_back(row);
    }

    for (int i = 0; i < imageHeight; i++)
    {
        for (int j = 0; j < imageWidth; j++)
        {
            Ray *ray = new Ray(pos->copy(), pointBuffer[i][j]->subtract(pos));

            for (Object *object : objects)
            {
                if (object->objectType == "sphere")
                {
                    Color *color = object->intersect(ray, 0);
                    if (color != NULL)
                        colorBuffer[i][j] = color;
                }
            }
        }
    }

    bitmap_image bmpFile(imageWidth, imageHeight);
    for (int i = 0; i < imageHeight; i++)
    {
        for (int j = 0; j < imageWidth; j++)
        {
            bmpFile.set_pixel(j, i, 255*colorBuffer[i][j]->r, 255*colorBuffer[i][j]->g, 255*colorBuffer[i][j]->b);
        }
    }

    bmpFile.save_image("out"+to_string(imgCount)+".bmp");
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