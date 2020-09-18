#ifndef PLANETH
#define PLANETH

#include "indexmodel.h"
#include "movablemodel.h"
#include "texturemodel.h"
#include "matma.h"
#include "texturecameraprogram.h"

class Planet : public IndexModel
{
public:
    Planet(float init_velocity = 40, float init_angle = 0);
    void Initialize(int n, int m, float r);
    void Draw(const TextureCameraProgram &program) const;
    void Move(float delta_t);

    void SetPlanetTextureUnit(GLuint t) { planet_texture_unit_ = t; }
    void SetPlanetTexture(GLuint t) { planet_texture_ = t; }
    void SetMoonTextureUnit(GLuint t) { moon_texture_unit_ = t; }
    void SetMoonTexture(GLuint t) { moon_texture_ = t; }

private:
    Mat4 model_matrix_planet_;
    Mat4 model_matrix_moon_;

    GLuint planet_texture_unit_;
    GLuint planet_texture_;
    GLuint moon_texture_unit_;
    GLuint moon_texture_;


    int n_; // mesh parameter
    int m_; // mesh parameter

    float r; //small radius

    float planet_rot_angle_;
    float planet_rot_velocity_;

    float moon_rot_angle_;
    float moon_orbit_angle_;
    float moon_orbit_velocity_;
    float moon_rot_velocity_;
};

#endif // PLANETH
