#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include <QGLFunctions>
#include <glm/glm.hpp>
#include <vector>

class GLWidget : public QGLWidget, protected QGLFunctions
{
  Q_OBJECT
public:
  explicit GLWidget(QWidget *parent = 0);
  ~GLWidget();
  QSize minimumSizeHint() const;
  QSize sizeHint() const;

signals:
  void spotPosLabel(QString q);
  void spotDirLabel(QString q);
  void spotCutoffLabel(QString q);
  void spotExpLabel(QString q);
  void linearLabel(QString q);

public slots:
  void spotX(int value);
  void spotY(int value);
  void spotZ(int value);
  void spotDirX(int value);
  void spotDirY(int value);
  void spotDirZ(int value);
  void spotCutoff(int value);
  void spotExponent(int value);
  void linearAttenuation(int value);

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);
  void keyPressEvent(QKeyEvent *e);

private:
  void initializeShaders();
  void initializeBuffers();

  QGLShaderProgram* shader;
  GLuint vertex_array_cube;
  GLuint position_buffer_cube;
  GLuint normal_buffer_cube;
  GLuint index_buffer_cube;
  GLuint vertex_array_sphere;
  GLuint position_buffer_sphere;
  GLuint normal_buffer_sphere;
  GLuint vertex_array_plane;
  GLuint position_buffer_plane;
  GLuint normal_buffer_plane;
  GLuint draw_mode;
  glm::mat4 proj_mat;
  glm::mat4 view_mat;
  glm::mat4 model_mat;
  glm::mat3 normal_mat;
  float angle_x;
  float angle_y;
  int num_vertices_sphere;
  glm::vec3 spot_position;
  glm::vec3 spot_direction;
  GLfloat spot_cos_angle;
  GLfloat spot_exponent;
  GLfloat linear_attenuation;
  glm::vec3 camera_pos;
  glm::vec3 cube_pos;
};

#endif // GLWIDGET_H
