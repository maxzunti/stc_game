#include "../renderer.h"
#include "../camera.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <vector>
#include <cstdlib>
#include <ctime>



using namespace std;
using namespace glm;

#define PI 3.14159265359

char sun[] = "texture_sun.jpg";
char earth[] = "texture_earth_surface.jpg";
char moon[] = "texture_moon.jpg";
char stars[] = "stars_milkyway.jpg";

char *imageNames[] = { sun,earth,moon,stars };


vector<vec3> points;
vector<vec2> uvs;
vector<vec3> normals;
vector<unsigned int> indices;

vector<vec3> points2;
vector<vec3> normals2;
vector<vec2> uvs2;
vector<unsigned int> indices2;

vector<vec3> points3;
vector<vec3> normals3;
vector<vec2> uvs3;
vector<unsigned int> indices3;

vector<vec3> points4;
vector<vec3> normals4;
vector<vec2> uvs4;
vector<unsigned int> indices4;


vec3 earthCenter = vec3(-3.24f, 0, 0);
vec3 moonCenter = vec3(-3.24f, 0, 2.42);

// These really don't belong in renderer, but we should remove them eventually anyway
void generateTorus(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals,
    std::vector<glm::vec2>& uvs, std::vector<unsigned int>& indices,
    float c_r, float t_r,
    int uDivisions, int vDivisions);
void generatePSphere(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals,
    std::vector<glm::vec2>& uvs, std::vector<unsigned int>& indices,
    float r, glm::vec3 center,
    int uDivisions, int vDivisions);

// here thar be shapes
//SUN
// Radius: 3 root scale on radius (then divided them all by 100): 0.8862 
//generatePSphere(points, normals, uvs, indices, .8862,earthCenter, 40, 40);

//EARTH
// Distance from the sun: 16 root scale: 3.24
// Radius: 3 root scale on radius (then divided them all by 100): 0.1855 
//generatePSphere(points2, normals2, uvs2, indices2, .2404f, earthCenter, 20, 20);
//generateTorus(points2, normals2, uvs2, indices2, .5404f, 0.3008f, 20, 20);
//MOON 
// Distance from the earth: 16 root scale: 2.42
// Radius 3 root scale on radius (then divided them all by 100): 0.1202
//generatePSphere(points3, normals3, uvs3, indices3, .1202f, moonCenter, 20, 20);

//STARS
//	generatePSphere(points4, normals4, uvs4, indices4, 400.f, vec3(0, 0, 0), 100, 100);


//u parameterizes in the big circle, v parameterizes in the little circle
//c_r is the circle radius, t_r is the tube radius
void generateTorus(vector<vec3>& positions, vector<vec3>& normals,
    vector<vec2>& uvs, vector<unsigned int>& indices,
    float c_r, float t_r,
    int uDivisions, int vDivisions)
{
    float uStep = 1.f / (float)(uDivisions - 1);
    float vStep = 1.f / (float)(vDivisions - 1);

    float u = 0.f;

    //Traversing u
    for (int i = 0; i<uDivisions; i++)
    {

        vec3 center = vec3(c_r*cos(2.f*PI*u),
            0.f,
            c_r*sin(2.f*PI*u));

        float v = 0.f;

        //Traversing v
        for (int j = 0; j<vDivisions; j++)
        {
            vec3 pos = vec3((c_r + t_r*cos(2.f*PI*v)) * cos(2.f*PI*u),
                t_r*sin(2.f*PI*v),
                (c_r + t_r*cos(2.f*PI*v)) * sin(2.f*PI*u));

            vec3 normal = normalize(pos - center);

            positions.push_back(pos);
            normals.push_back(normal);
            uvs.push_back(vec2(u, v));

            v += vStep;
        }

        u += uStep;
    }

    for (int i = 0; i<uDivisions - 1; i++)
    {
        for (int j = 0; j<vDivisions - 1; j++)
        {
            unsigned int p00 = i*vDivisions + j;
            unsigned int p01 = i*vDivisions + j + 1;
            unsigned int p10 = (i + 1)*vDivisions + j;
            unsigned int p11 = (i + 1)*vDivisions + j + 1;

            indices.push_back(p00);
            indices.push_back(p10);
            indices.push_back(p01);

            indices.push_back(p01);
            indices.push_back(p10);
            indices.push_back(p11);
        }
    }
}

void generatePSphere(vector<vec3>& positions, vector<vec3>& normals,
    vector<vec2>& uvs, vector<unsigned int>& indices,
    float r, vec3 center,
    int uDivisions, int vDivisions)
{
    // udivisions will be theta
    // vdivisions will be phi	
    float uStep = 1.f / (float)(uDivisions - 1);
    float vStep = 1.f / (float)(vDivisions - 1);

    float u = 0.f;

    // Iterate through phi and theta
    for (double phi = 0.; phi < uDivisions; phi++) // Azimuth [0, 2PI]
    {
        float v = 0.f;
        for (double theta = 0.; theta < vDivisions; theta++) // Elevation [0, PI]
        {
            vec3 point;
            point.x = r * cos(v * 2 * PI) * sin(u*PI) + center.x;
            point.y = r * sin(v * 2 * PI) * sin(u*PI) + center.y;
            point.z = r               * cos(u*PI) + center.z;

            vec3 normal = normalize(point - center);

            positions.push_back(point);
            normals.push_back(normal);
            uvs.push_back(vec2(u, v));

            v += vStep;
        }
        u += uStep;
    }

    for (int i = 0; i<uDivisions - 1; i++)
    {
        for (int j = 0; j<vDivisions - 1; j++)
        {
            unsigned int p00 = i*vDivisions + j;
            unsigned int p01 = i*vDivisions + j + 1;
            unsigned int p10 = (i + 1)*vDivisions + j;
            unsigned int p11 = (i + 1)*vDivisions + j + 1;

            indices.push_back(p00);
            indices.push_back(p10);
            indices.push_back(p01);

            indices.push_back(p01);
            indices.push_back(p10);
            indices.push_back(p11);
        }
    }
}