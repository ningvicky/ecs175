#include "shapes.h"

void makeCube(std::vector<GLfloat>* vertices, std::vector<GLfloat>* normals, std::vector<GLubyte>* indices)
{
  vertices->resize(6*3*4);
  normals->resize(6*3*4);
  indices->resize(6*3*2);
  for(int i = 0; i < 6; ++i)
  {
    int face = i*12;
    int component = i%3;
    int sign = -(2*(i/3)-1);
    // 4 vertices per face and same normal for each vertex of each face
    for(int j = 0; j < 4; ++j)
    {
      int vertex = 3*j;
     
      (*vertices)[face+vertex+component] = sign*0.5;
      (*vertices)[face+vertex+((component+1)%3)] = sign*((j%2)-0.5);
      (*vertices)[face+vertex+((component+2)%3)] = (j/2)-0.5;

      (*normals)[face+vertex+component] = sign;
      (*normals)[face+vertex+((component+1)%3)] = 0;
      (*normals)[face+vertex+((component+2)%3)] = 0;
    }

    // 2 triangles per face
    for(int j = 0; j < 2; ++j)
    {
      // front face
      (*indices)[i*6+3*j] = 4*i + 3*j;
      (*indices)[i*6+3*j+1] = 4*i + j + 1;
      (*indices)[i*6+3*j+2] = 4*i - j + 2;
    }
  }
}

void makeSphere(std::vector<GLfloat>* vertices, std::vector<GLfloat>* normals, int num_subdivisions)
{
  std::vector<tri> triangles;
  triangles.reserve(8);
  triangles.push_back(tri(point(0,0,1),point(0,-1,0),point(1,0,0)));
  triangles.push_back(tri(point(1,0,0),point(0,-1,0),point(0,0,-1)));
  triangles.push_back(tri(point(0,0,-1),point(0,-1,0),point(-1,0,0)));
  triangles.push_back(tri(point(-1,0,0),point(0,-1,0),point(0,0,1)));

  triangles.push_back(tri(point(0,0,1),point(1,0,0),point(0,1,0)));
  triangles.push_back(tri(point(1,0,0),point(0,0,-1),point(0,1,0)));
  triangles.push_back(tri(point(0,0,-1),point(-1,0,0),point(0,1,0)));
  triangles.push_back(tri(point(-1,0,0),point(0,0,1),point(0,1,0)));

  for(int i = 0; i < num_subdivisions; i++)
  {
    std::vector<tri> tsub;
    tsub.reserve(triangles.size() * 4);
    for(unsigned int j = 0; j < triangles.size(); j++)
    {
      tri t = triangles[j];
      point p12 = t.p1.mid(t.p2).normalize();
      point p23 = t.p2.mid(t.p3).normalize();
      point p31 = t.p3.mid(t.p1).normalize();
      tsub.push_back(tri(t.p1, p12, p31));
      tsub.push_back(tri(p12, t.p2, p23));
      tsub.push_back(tri(p23, t.p3, p31));
      tsub.push_back(tri(p12, p23, p31));
    }
    triangles.swap(tsub);
  }
  vertices->resize(triangles.size()*3*3);
  normals->resize(triangles.size()*3*3);
  for(unsigned int i = 0; i < triangles.size(); i++)
  {
    tri t = triangles[i];
    (*vertices)[9*i + 0] = t.p1.x;
    (*vertices)[9*i + 1] = t.p1.y;
    (*vertices)[9*i + 2] = t.p1.z;
    (*vertices)[9*i + 3] = t.p2.x;
    (*vertices)[9*i + 4] = t.p2.y;
    (*vertices)[9*i + 5] = t.p2.z;
    (*vertices)[9*i + 6] = t.p3.x;
    (*vertices)[9*i + 7] = t.p3.y;
    (*vertices)[9*i + 8] = t.p3.z;

    (*normals)[9*i + 0] = t.p1.x;
    (*normals)[9*i + 1] = t.p1.y;
    (*normals)[9*i + 2] = t.p1.z;
    (*normals)[9*i + 3] = t.p2.x;
    (*normals)[9*i + 4] = t.p2.y;
    (*normals)[9*i + 5] = t.p2.z;
    (*normals)[9*i + 6] = t.p3.x;
    (*normals)[9*i + 7] = t.p3.y;
    (*normals)[9*i + 8] = t.p3.z;
  }
}
