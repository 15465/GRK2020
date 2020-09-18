#include "planet.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>

#include "movablemodel.h"
#include "vertices.h"

Planet::Planet(float init_angle)
{
    star_rot_velocity_ = 3;

    planet_rot_velocity_ = 30;
    planet_orbit_velocity_ = 30;
    planet_distance_ = 3;

    moon_rot_velocity_ = 30;
    moon_orbit_velocity_ = 150;
    moon_distance_ = 1;

    star_rot_angle_ = planet_rot_angle_ = moon_orbit_angle_ = moon_rot_angle_ = planet_orbit_angle_ = init_angle;
}

void Planet::Move(float delta_t)
{
    star_rot_angle_ += delta_t * star_rot_velocity_;
    
    planet_rot_angle_ += delta_t * planet_rot_velocity_;
    planet_orbit_angle_ += delta_t * planet_orbit_velocity_;

    moon_rot_angle_ += delta_t * moon_rot_velocity_;
    moon_orbit_angle_ += delta_t * moon_orbit_velocity_;
    
    if (star_rot_angle_ > 360)
        star_rot_angle_ -= 360;
    if (star_rot_angle_ < -360)
        star_rot_angle_ += 360;

    if (planet_rot_angle_ > 360)
        planet_rot_angle_ -= 360;
    if (planet_rot_angle_ < -360)
        planet_rot_angle_ += 360;

    if (planet_orbit_angle_ > 360)
        planet_orbit_angle_ -= 360;
    if (planet_orbit_angle_ < -360)
        planet_orbit_angle_ += 360;

    if (moon_rot_angle_ > 360)
        moon_rot_angle_ -= 360;
    if (moon_rot_angle_ < -360)
        moon_rot_angle_ += 360;

    if (moon_orbit_angle_ > 360)
        moon_orbit_angle_ -= 360;
    if (moon_orbit_angle_ < -360)
        moon_orbit_angle_ += 360;

    //star
    model_matrix_star_.SetUnitMatrix();
    model_matrix_star_.RotateAboutY(star_rot_angle_);

    normal_matrix_star_.SetUnitMatrix();
    normal_matrix_star_.RotateAboutY(-star_rot_angle_);

    //planet
    model_matrix_planet_.SetUnitMatrix();
    model_matrix_planet_.RotateAboutY(planet_rot_angle_);
    model_matrix_planet_.Scale(0.3, 0.3, 0.3);
    model_matrix_planet_.Translate(planet_distance_, 0, 0);
    model_matrix_planet_.RotateAboutY(planet_orbit_angle_);

    normal_matrix_planet_.SetUnitMatrix();
    normal_matrix_planet_.RotateAboutY(-planet_orbit_angle_ - planet_rot_angle_);
    normal_matrix_planet_.Scale(3, 3, 3);

    //moon
    
    model_matrix_moon_.SetUnitMatrix();
    model_matrix_moon_.RotateAboutY(moon_rot_angle_);
    model_matrix_moon_.Scale(0.15, 0.15, 0.15);
    model_matrix_moon_.Translate(moon_distance_, 0, 0);
    model_matrix_moon_.RotateAboutY(moon_orbit_angle_);
    model_matrix_moon_.Translate(planet_distance_, 0, 0);
    model_matrix_moon_.RotateAboutY(planet_orbit_angle_);

    normal_matrix_moon_.SetUnitMatrix();
    normal_matrix_moon_.RotateAboutY(-planet_orbit_angle_ - moon_orbit_angle_ - moon_rot_angle_);
    normal_matrix_moon_.Scale(6, 6, 6);
}

void Planet::Initialize(int n, int m, float r, float distance)
{
    planet_distance_ = distance;
    n_ = n;
    m_ = m;
    NormalTextureVertex* vertices = new NormalTextureVertex[(m_ + 1) * (n_ + 1)];
    //TextureVertex *vertices = new TextureVertex[(m_ + 1) * (n_ + 1)];
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

            vertices[i * (m_ + 1) + j].normal[0] = cos(phi) * sin(theta);
            vertices[i * (m_ + 1) + j].normal[1] = sin(phi);
            vertices[i * (m_ + 1) + j].normal[2] = cos(phi) * cos(theta);
        }
    }

    GLuint* indices = new GLuint[2 * n_ * (m_ + 1)];

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
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
        (GLvoid*)sizeof(vertices[0].position));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
        (GLvoid*)(sizeof(vertices[0].position) + sizeof(vertices[0].texture)));
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &index_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        (m_ + 1) * n_ * 2 * sizeof(GLuint),
        indices,
        GL_STATIC_DRAW
    );
    delete[] vertices;
    delete[] indices;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Planet::Draw(const LightProgram& program) const
{

    glBindVertexArray(vao_);
    glUseProgram(program);
    //render gwiazdy
    program.SetModelMatrix(model_matrix_star_);
    program.SetMaterial(star_material_);
    program.SetNormalMatrix(normal_matrix_star_);
    glActiveTexture(star_texture_unit_);
    glBindTexture(GL_TEXTURE_2D, star_texture_);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    for (int i = 0; i < n_; i++)
    {
        glDrawElements(GL_TRIANGLE_STRIP,
            2 * (m_ + 1),
            GL_UNSIGNED_INT,
            (GLvoid*)(sizeof(GLuint) * 2 * i * (m_ + 1)));
    }
    //render planety
    program.SetModelMatrix(model_matrix_planet_);
    program.SetMaterial(material_);
    program.SetNormalMatrix(normal_matrix_planet_);
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
            (GLvoid*)(sizeof(GLuint) * 2 * i * (m_ + 1)));
    }
    //render ksiê¿yca
    program.SetModelMatrix(model_matrix_moon_);
    program.SetMaterial(material_);
    program.SetNormalMatrix(normal_matrix_moon_);
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
            (GLvoid*)(sizeof(GLuint) * 2 * i * (m_ + 1)));
    }
    glDisable(GL_CULL_FACE);

    glBindVertexArray(0);
    glUseProgram(0);
}
