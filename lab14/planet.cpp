#include "planet.h"
#include "vertices.h"

#include <iostream>
#include <cmath>

Planet::Planet(float init_velocity, float init_angle)
{
  planet_rot_velocity_ = 3;
  moon_orbit_velocity_ = 10;
  moon_rot_velocity_ = 50;

  planet_rot_angle_ = moon_orbit_angle_ = moon_rot_angle_ = init_angle;
}

void Planet::Move(float delta_t)
{
  planet_rot_angle_ += delta_t * planet_rot_velocity_;
  moon_orbit_angle_ += delta_t * moon_orbit_velocity_;
  moon_rot_angle_ += delta_t * moon_rot_velocity_;

  if (planet_rot_angle_ > 360)
    planet_rot_angle_ -= 360;
  if (planet_rot_angle_ < -360)
    planet_rot_angle_ += 360;

  if (moon_rot_angle_ > 360)
    moon_rot_angle_ -= 360;
  if (moon_rot_angle_ < -360)
    moon_rot_angle_ += 360;

  if (moon_rot_angle_ > 360)
    moon_rot_angle_ -= 360;
  if (moon_rot_angle_ < -360)
    moon_rot_angle_ += 360;

  model_matrix_planet_.SetUnitMatrix();
  model_matrix_planet_.RotateAboutY(planet_rot_angle_);

  model_matrix_moon_.SetUnitMatrix();
  model_matrix_moon_.Scale(0.5, 0.45, 0.5);
  model_matrix_moon_.RotateAboutY(moon_rot_angle_);
  model_matrix_moon_.Translate(3, 0, 0);
  model_matrix_moon_.RotateAboutY(moon_orbit_angle_);
}

void Planet::Initialize(int n, int m, float r)
{
  n_ = n;
  m_ = m;
  TextureVertex *vertices = new TextureVertex[(m_ + 1) * (n_ + 1)];
  int i, j;
  for (i = 0; i <= n_; i++)
  {
    float phi = (-M_PI / 2) + (M_PI / (float)n_ * i);
    for (j = 0; j <= m_; j++)
    {
      float theta = 2 * M_PI / (float)m_ * j;
      vertices[i * (m_ + 1) + j].position[0] = r * sin(theta) * cos(phi);
      vertices[i * (m_ + 1) + j].position[1] = r * sin(phi);
      vertices[i * (m_ + 1) + j].position[2] = r * cos(theta) * cos(phi);
      vertices[i * (m_ + 1) + j].position[3] = 1.0f;

      vertices[i * (m_ + 1) + j].texture[0] = (float)j / (float)m_;
      vertices[i * (m_ + 1) + j].texture[1] = (float)i / (float)n_;
    }
  }

  GLuint *indices = new GLuint[2 * n_ * (m_ + 1)];

  unsigned int k = 0;

  for (i = 0; i <= n_ - 1; i++)
  {
    for (j = 0; j <= m_; j++)
    {
      indices[2 * (i * (m_ + 1) + j)] = k;
      indices[2 * (i * (m_ + 1) + j) + 1] = k + m_ + 1;
      k++;
    }
  }

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER,
               (m_ + 1) * (n_ + 1) * sizeof(vertices[0]),
               vertices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid *)sizeof(vertices[0].position));
  glEnableVertexAttribArray(2);

  glGenBuffers(1, &index_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               (m_ + 1) * n_ * 2 * sizeof(GLuint),
               indices,
               GL_STATIC_DRAW);
  delete[] vertices;
  delete[] indices;
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Planet::Draw(const TextureCameraProgram &program) const
{

  glBindVertexArray(vao_);
  glUseProgram(program);

  //render planety
  program.SetModelMatrix(model_matrix_planet_);

  glActiveTexture(planet_texture_unit_);
  glBindTexture(GL_TEXTURE_2D, planet_texture_);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);
  for (int i = 0; i < n_; i++)
  {
    glDrawElements(GL_TRIANGLE_STRIP,
                   2 * (m_ + 1),
                   GL_UNSIGNED_INT,
                   (GLvoid *)(sizeof(GLuint) * 2 * i * (m_ + 1)));
  }
  //render księżyca
  program.SetModelMatrix(model_matrix_moon_);

  glActiveTexture(moon_texture_unit_);
  glBindTexture(GL_TEXTURE_2D, moon_texture_);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);
  for (int i = 0; i < n_; i++)
  {
    glDrawElements(GL_TRIANGLE_STRIP,
                   2 * (m_ + 1),
                   GL_UNSIGNED_INT,
                   (GLvoid *)(sizeof(GLuint) * 2 * i * (m_ + 1)));
  }
  glDisable(GL_CULL_FACE);

  glBindVertexArray(0);
  glUseProgram(0);
}
