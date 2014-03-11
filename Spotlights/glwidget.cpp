#include "glwidget.h"

#include <OPenGL/glext.h>
#if defined(__linux)
#include <GL/glx.h>
#endif
#include <QtGui>
#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shapes.h"


//PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
//PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
//PFNglBindVertexArrayAPPLEPROC glBindVertexArrayAPPLE;

GLWidget::GLWidget(QWidget *parent) :
  QGLWidget(parent), draw_mode(GL_TRIANGLES), angle_x(0.f), angle_y(0.f), spot_position(glm::vec3(2,2,0)), spot_direction(glm::vec3(-1,-1,0)), spot_cos_angle(0.5), spot_exponent(8), linear_attenuation(0.2), camera_pos(glm::vec3(0,3,5)), cube_pos(glm::vec3(0,0,0))
{
  setFocusPolicy(Qt::StrongFocus);
  setFocus();
}

GLWidget::~GLWidget()
{
  //delete the vertex array and vertex buffer
  GLuint loc = glGetAttribLocation(shader->programId(), "position");
  glDisableVertexAttribArray(loc);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &position_buffer_cube);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &index_buffer_cube);

  glBindVertexArrayAPPLE(0);
  glDeleteVertexArrays(1, &vertex_array_cube);

  //delete the shaders
  shader->release();
  shader->removeAllShaders();
  delete shader;
}

void GLWidget::initializeGL()
{
  //load opengl functions that are not in QGLFunctions
#if defined(__linux)
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glXGetProcAddress((GLubyte*)"glDeleteVertexArrays");
  glGenVertexArrays = (PFNglGenVertexArraysAPPLEPROC)glXGetProcAddress((GLubyte*)"glGenVertexArrays");
  glBindVertexArrayAPPLE = (PFNglBindVertexArrayAPPLEAPPLEPROC)glXGetProcAddress((GLubyte*)"glBindVertexArrayAPPLE");
#elif defined(WIN32)
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
  glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
  glBindVertexArrayAPPLE = (PFNglBindVertexArrayAPPLEPROC)wglGetProcAddress("glBindVertexArrayAPPLE");
#endif

  //load opengl functions
  initializeGLFunctions();

  //set the clear color to black
  glClearColor(0,0,0,1);

  //enable depth testing
  glEnable(GL_DEPTH_TEST);

  //enable back face culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  //load and initialize shaders
  initializeShaders();

  //setup vertex and color data and send to GPU
  initializeBuffers();

  view_mat = glm::lookAt(camera_pos, glm::vec3(0,0,0), glm::vec3(0,1,0));
  GLuint loc = glGetUniformLocation(shader->programId(), "view_mat");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view_mat));

  //set the light color
  loc = glGetUniformLocation(shader->programId(), "light_color");
  glUniform4f(loc, 1.f, 1.f, 1.f, 1.f);
  
  //spotlight position
  loc = glGetUniformLocation(shader->programId(), "spot_pos");
  glUniform3f(loc, spot_position.x, spot_position.y, spot_position.z);

  //spotlight direction
  loc = glGetUniformLocation(shader->programId(), "spot_dir");
  glUniform3f(loc, spot_direction.x, spot_direction.y, spot_direction.z);

  //spotlight cutoff
  loc = glGetUniformLocation(shader->programId(), "spot_cutoff");
  glUniform1f(loc, spot_cos_angle);

  //spotlight exponent
  loc = glGetUniformLocation(shader->programId(), "spot_exponent");
  glUniform1f(loc, spot_exponent);

  //linear attenuation
  loc = glGetUniformLocation(shader->programId(), "linear_attenuation");
  glUniform1f(loc, linear_attenuation);

  //print out the OpenGL version
  std::cout << (char *)glGetString(GL_VERSION) << std::endl;
}

void GLWidget::paintGL()
{
  //clear the color and depth buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArrayAPPLE(vertex_array_cube);

  glm::mat4 cube_transform = glm::rotate(glm::mat4(1), glm::pi<float>() * angle_y, glm::vec3(0,1,0));
  cube_transform = glm::translate(cube_transform, cube_pos);
  cube_transform = glm::rotate(cube_transform, glm::pi<float>() * angle_x, glm::vec3(1,0,0));
  cube_transform = glm::scale(cube_transform, glm::vec3(0.5,0.5,0.5));

  model_mat = cube_transform;
  GLuint loc = glGetUniformLocation(shader->programId(), "model_mat");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_mat));

  normal_mat = glm::mat3(view_mat*model_mat);
  GLuint loc_norm = glGetUniformLocation(shader->programId(), "normal_mat");
  glUniformMatrix3fv(loc_norm, 1, GL_FALSE, glm::value_ptr(normal_mat));

  //set a new color
  GLuint loc_color = glGetUniformLocation(shader->programId(), "material_color");
  glUniform4f(loc_color, 0.8f, 0.f, 0.f, 1.f);

  //draw the cube
  glDrawElements(draw_mode, 36, GL_UNSIGNED_BYTE, (GLvoid*)0);

  model_mat = cube_transform;
  model_mat = glm::translate(model_mat, glm::vec3(0.75,0,0));
  model_mat = glm::scale(model_mat, glm::vec3(0.5,0.25,0.25));
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_mat));

  normal_mat = glm::mat3(view_mat*model_mat);
  glUniformMatrix3fv(loc_norm, 1, GL_FALSE, glm::value_ptr(normal_mat));

  //set a new color
  glUniform4f(loc_color, 0.f, 0.8f, 1.f, 1.f);

  //draw the cube
  glDrawElements(draw_mode, 36, GL_UNSIGNED_BYTE, (GLvoid*)0);

  model_mat = cube_transform;
  model_mat = glm::translate(model_mat, glm::vec3(-0.75,0,0));
  model_mat = glm::scale(model_mat, glm::vec3(0.5,0.25,0.25));
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_mat));

  normal_mat = glm::mat3(view_mat*model_mat);
  glUniformMatrix3fv(loc_norm, 1, GL_FALSE, glm::value_ptr(normal_mat));

  //set a new color
  glUniform4f(loc_color, 0.8f, 0.f, 0.8f, 1.f);

  //draw the cube
  glDrawElements(draw_mode, 36, GL_UNSIGNED_BYTE, (GLvoid*)0);

  model_mat = cube_transform;
  model_mat = glm::translate(model_mat, glm::vec3(0.25,-1,0));
  model_mat = glm::scale(model_mat, glm::vec3(0.25,1,0.25));
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_mat));

  normal_mat = glm::mat3(view_mat*model_mat);
  glUniformMatrix3fv(loc_norm, 1, GL_FALSE, glm::value_ptr(normal_mat));

  //set a new color
  glUniform4f(loc_color, 0.2f, 0.5f, 0.8f, 1.f);

  //draw the cube
  glDrawElements(draw_mode, 36, GL_UNSIGNED_BYTE, (GLvoid*)0);

  model_mat = cube_transform;
  model_mat = glm::translate(model_mat, glm::vec3(-0.25,-1,0));
  model_mat = glm::scale(model_mat, glm::vec3(0.25,1,0.25));
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_mat));

  normal_mat = glm::mat3(view_mat*model_mat);
  glUniformMatrix3fv(loc_norm, 1, GL_FALSE, glm::value_ptr(normal_mat));

  //set a new color
  glUniform4f(loc_color, 0.3f, 0.8f, 0.1f, 1.f);

  //draw the cube
  glDrawElements(draw_mode, 36, GL_UNSIGNED_BYTE, (GLvoid*)0);

  //switch to the sphere
  glBindVertexArrayAPPLE(vertex_array_sphere);

  model_mat = cube_transform;
  model_mat = glm::translate(model_mat, glm::vec3(0,1,0));
  model_mat = glm::scale(model_mat, glm::vec3(0.6,0.6,0.4));
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_mat));

  //non-uniform scaling in the model matrix -- need to use inverse transpose of the modelview matrix
  normal_mat = glm::transpose(glm::inverse(glm::mat3(view_mat*model_mat)));
  glUniformMatrix3fv(loc_norm, 1, GL_FALSE, glm::value_ptr(normal_mat));

  //set a new color
  glUniform4f(loc_color, 0.8f, 0.8f, 0.f, 1.f);

  //draw the sphere
  glDrawArrays(draw_mode, 0, num_vertices_sphere);

  //sphere located at light position
  model_mat = glm::translate(glm::mat4(1), spot_position);
  model_mat = glm::scale(model_mat, glm::vec3(0.1,0.1,0.1));
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_mat));

  //normal matrix
  normal_mat = glm::mat3(view_mat*model_mat);
  glUniformMatrix3fv(loc_norm, 1, GL_FALSE, glm::value_ptr(normal_mat));

  //set a new color
  glUniform4f(loc_color, 0.9f, 0.9f, 0.2f, 1.f);

  //draw the sphere
  glDrawArrays(draw_mode, 0, num_vertices_sphere);

  //switch to the plane
  glBindVertexArrayAPPLE(vertex_array_plane);

  model_mat = glm::mat4(1);
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model_mat));

  normal_mat = glm::mat3(view_mat*model_mat);
  glUniformMatrix3fv(loc_norm, 1, GL_FALSE, glm::value_ptr(normal_mat));

  //set a new color
  glUniform4f(loc_color, 0.5f, 0.5f, 0.5f, 1.f);

  //draw the plane
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLWidget::resizeGL(int w, int h)
{
  //set the viewing rectangle to be the entire window
  glViewport(0,0,w,h);
  float aspect_ratio = w/(float)h;
  proj_mat = glm::perspective(glm::pi<float>() * 0.25f, aspect_ratio, 0.1f, 100.f);
  GLuint loc = glGetUniformLocation(shader->programId(), "proj_mat");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(proj_mat));
}

QSize GLWidget::minimumSizeHint() const
{
  return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
  return QSize(500, 500);
}

void GLWidget::initializeShaders()
{
  //load the shaders, link the shader, and set the shader to be active
  shader = new QGLShaderProgram();
  shader->addShaderFromSourceFile(QGLShader::Vertex, ":/lighting.vert");
  shader->addShaderFromSourceFile(QGLShader::Fragment, ":/lighting.frag");
  shader->link();
  shader->bind();
}

void GLWidget::initializeBuffers()
{
  //---------------------------------------------------------------------------
  //cube
  //---------------------------------------------------------------------------
  std::vector<GLfloat> vertices;
  std::vector<GLfloat> normals;
  std::vector<GLubyte> indices;

  //generate vertices, normals, and indices for the cube
  makeCube(&vertices, &normals, &indices);

  //get a unique id for the vertex array
  glGenVertexArrays(1, &vertex_array_cube);
  //set the vertex array to be the active one
  glBindVertexArrayAPPLE(vertex_array_cube);

  //get a unique id for the position vertex buffer
  glGenBuffers(1, &position_buffer_cube);
  //set the position vertex buffer to be active
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer_cube);
  //specify the size and type of the position vertex buffer and load data
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

  //set the size of the position vector (3 component) and connect it to the "position" shader variable
  GLuint loc = glGetAttribLocation(shader->programId(), "position");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //get a unique id for the normal vertex buffer
  glGenBuffers(1, &normal_buffer_cube);
  //set the normal vertex buffer to be active
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_cube);
  //specify the size and type of the normal vertex buffer and load data
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normals.size(), &normals[0], GL_STATIC_DRAW);

  //set the size of the normal vector (3 component) and connect it to the "normal" shader variable
  loc = glGetAttribLocation(shader->programId(), "normal");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &index_buffer_cube);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_cube);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte)*indices.size(), &indices[0], GL_STATIC_DRAW);

  //---------------------------------------------------------------------------
  //sphere
  //---------------------------------------------------------------------------
  vertices.clear();
  normals.clear();

  makeSphere(&vertices, &normals, 3);
  num_vertices_sphere = vertices.size()/3;

  //get a unique id for the vertex array
  glGenVertexArrays(1, &vertex_array_sphere);
  //set the vertex array to be the active one
  glBindVertexArrayAPPLE(vertex_array_sphere);

  //get a unique id for the position vertex buffer
  glGenBuffers(1, &position_buffer_sphere);
  //set the position vertex buffer to be active
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer_sphere);
  //specify the size and type of the position vertex buffer and load data
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

  //set the size of the position vector (3 component) and connect it to the "position" shader variable
  loc = glGetAttribLocation(shader->programId(), "position");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //get a unique id for the normal vertex buffer
  glGenBuffers(1, &normal_buffer_sphere);
  //set the normal vertex buffer to be active
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_sphere);
  //specify the size and type of the normal vertex buffer and load data
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normals.size(), &normals[0], GL_STATIC_DRAW);

  //set the size of the normal vector (3 component) and connect it to the "normal" shader variable
  loc = glGetAttribLocation(shader->programId(), "normal");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //---------------------------------------------------------------------------
  //plane
  //---------------------------------------------------------------------------
  vertices.clear();
  normals.clear();

  vertices.push_back(-2); vertices.push_back(-1); vertices.push_back(2);
  vertices.push_back(2); vertices.push_back(-1); vertices.push_back(2);
  vertices.push_back(-2); vertices.push_back(-1); vertices.push_back(-2);
  vertices.push_back(2); vertices.push_back(-1); vertices.push_back(-2);

  normals.push_back(0); normals.push_back(1); normals.push_back(0);
  normals.push_back(0); normals.push_back(1); normals.push_back(0);
  normals.push_back(0); normals.push_back(1); normals.push_back(0);
  normals.push_back(0); normals.push_back(1); normals.push_back(0);

  //get a unique id for the vertex array
  glGenVertexArrays(1, &vertex_array_plane);
  //set the vertex array to be the active one
  glBindVertexArrayAPPLE(vertex_array_plane);

  //get a unique id for the position vertex buffer
  glGenBuffers(1, &position_buffer_plane);
  //set the position vertex buffer to be active
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer_plane);
  //specify the size and type of the position vertex buffer and load data
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

  //set the size of the position vector (3 component) and connect it to the "position" shader variable
  loc = glGetAttribLocation(shader->programId(), "position");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //get a unique id for the normal vertex buffer
  glGenBuffers(1, &normal_buffer_plane);
  //set the normal vertex buffer to be active
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_plane);
  //specify the size and type of the normal vertex buffer and load data
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*normals.size(), &normals[0], GL_STATIC_DRAW);

  //set the size of the normal vector (3 component) and connect it to the "normal" shader variable
  loc = glGetAttribLocation(shader->programId(), "normal");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
  switch(e->key())
  {
  case Qt::Key_Right:
    {
      angle_y += 0.1f;
      if(angle_y > 2.f)
      {
        angle_y = 0;
      }
      break;
    }

  case Qt::Key_Left:
    {
      angle_y -= 0.1f;
      if(angle_y < 0.f)
      {
        angle_y = 2.f;
      }
      break;
    }

  case Qt::Key_Up:
    {
      angle_x += 0.1f;
      if(angle_x > 2.f)
      {
        angle_x = 0;
      }
      break;
    }

  case Qt::Key_Down:
    {
      angle_x -= 0.1f;
      if(angle_x < 0.f)
      {
        angle_x = 2.f;
      }
      break;
    }

  case Qt::Key_L:
    {
      glPolygonMode(GL_FRONT, GL_LINE);
      break;
    }

  case Qt::Key_F:
    {
      glPolygonMode(GL_FRONT, GL_FILL);
      break;
    }

  case Qt::Key_W:
    {
      cube_pos.y += 0.5;
      break;
    }
  case Qt::Key_S:
    {
      cube_pos.y -= 0.5;
      break;
    }
  case Qt::Key_A:
    {
      cube_pos.x -= 0.5;
      break;
    }
  case Qt::Key_D:
    {
      cube_pos.x += 0.5;
      break;
    }

  default:
    QWidget::keyPressEvent(e);
    break;
  }

  updateGL();
}

void GLWidget::spotX(int value)
{
  spot_position.x = value;
  GLuint loc = glGetUniformLocation(shader->programId(), "spot_pos");
  glUniform3f(loc, spot_position.x, spot_position.y, spot_position.z);
  QString pos_label = "Spotlight position: " + QString::number(spot_position.x, 'g', 4) + ", " + QString::number(spot_position.y, 'g', 4) + ", " + QString::number(spot_position.z, 'g', 4);
  emit spotPosLabel(pos_label);
  updateGL();
}

void GLWidget::spotY(int value)
{
  spot_position.y = value;
  GLuint loc = glGetUniformLocation(shader->programId(), "spot_pos");
  glUniform3f(loc, spot_position.x, spot_position.y, spot_position.z);
  QString pos_label = "Spotlight position: " + QString::number(spot_position.x, 'g', 4) + ", " + QString::number(spot_position.y, 'g', 4) + ", " + QString::number(spot_position.z, 'g', 4);
  emit spotPosLabel(pos_label);
  updateGL();
}

void GLWidget::spotZ(int value)
{
  spot_position.z = value;
  GLuint loc = glGetUniformLocation(shader->programId(), "spot_pos");
  glUniform3f(loc, spot_position.x, spot_position.y, spot_position.z);
  QString pos_label = "Spotlight position: " + QString::number(spot_position.x, 'g', 4) + ", " + QString::number(spot_position.y, 'g', 4) + ", " + QString::number(spot_position.z, 'g', 4);
  emit spotPosLabel(pos_label);
  updateGL();
}

void GLWidget::spotDirX(int value)
{
  spot_direction.x = value;
  GLuint loc = glGetUniformLocation(shader->programId(), "spot_dir");
  glUniform3f(loc, spot_direction.x, spot_direction.y, spot_direction.z);
  QString dir_label = "Spotlight direction: " + QString::number(spot_direction.x, 'g', 4) + ", " + QString::number(spot_direction.y, 'g', 4) + ", " + QString::number(spot_direction.z, 'g', 4);
  emit spotDirLabel(dir_label);
  updateGL();
}

void GLWidget::spotDirY(int value)
{
  spot_direction.y = value;
  GLuint loc = glGetUniformLocation(shader->programId(), "spot_dir");
  glUniform3f(loc, spot_direction.x, spot_direction.y, spot_direction.z);
  QString dir_label = "Spotlight direction: " + QString::number(spot_direction.x, 'g', 4) + ", " + QString::number(spot_direction.y, 'g', 4) + ", " + QString::number(spot_direction.z, 'g', 4);
  emit spotDirLabel(dir_label);
  updateGL();
}

void GLWidget::spotDirZ(int value)
{
  spot_direction.z = value;
  GLuint loc = glGetUniformLocation(shader->programId(), "spot_dir");
  glUniform3f(loc, spot_direction.x, spot_direction.y, spot_direction.z);
  QString dir_label = "Spotlight direction: " + QString::number(spot_direction.x, 'g', 4) + ", " + QString::number(spot_direction.y, 'g', 4) + ", " + QString::number(spot_direction.z, 'g', 4);
  emit spotDirLabel(dir_label);
  updateGL();
}

void GLWidget::spotCutoff(int value)
{
  spot_cos_angle = value/20.f;
  GLuint loc = glGetUniformLocation(shader->programId(), "spot_cutoff");
  glUniform1f(loc, spot_cos_angle);
  QString cutoff_label = "Spotlight cutoff: " + QString::number(spot_cos_angle, 'g', 4);
  emit spotCutoffLabel(cutoff_label);
  updateGL();
}

void GLWidget::spotExponent(int value)
{
  spot_exponent = value*4;
  GLuint loc = glGetUniformLocation(shader->programId(), "spot_exponent");
  glUniform1f(loc, spot_exponent);
  QString exp_label = "Spotlight exponent: " + QString::number(spot_exponent, 'g', 4);
  emit spotExpLabel(exp_label);
  updateGL();
}

void GLWidget::linearAttenuation(int value)
{
  linear_attenuation = value/10.f;
  GLuint loc = glGetUniformLocation(shader->programId(), "linear_attenuation");
  glUniform1f(loc, linear_attenuation);
  QString linear_label = "Linear attenuation: " + QString::number(linear_attenuation, 'g', 4);
  emit linearLabel(linear_label);
  updateGL();
}


