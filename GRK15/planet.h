#ifndef PLANETH
#define PLANETH

#include <GL/glew.h>

#include "lightprogram.h"
#include "movablemodel.h"
#include "lightablemodel.h"
#include "materialmodel.h"
#include "texturemodel.h"
#include "indexmodel.h"
#include "matma.h"

class Planet : public IndexModel, public TextureModel, public MovableModel, public LightableModel, public MaterialModel
{
public:
    Planet(float init_angle = 0);
    void Initialize(int n, int m, float r, float distance);
    void Draw(const LightProgram& program) const;
    void Move(float delta_t);

    void SetStarMaterial(Material t) { star_material_ = t; }
    void SetStarTextureUnit(GLuint t) { star_texture_unit_ = t; }
    void SetStarTexture(GLuint t) { star_texture_ = t; }
    void SetPlanetTextureUnit(GLuint t) { planet_texture_unit_ = t; }
    void SetPlanetTexture(GLuint t) { planet_texture_ = t; }
    void SetMoonTextureUnit(GLuint t) { moon_texture_unit_ = t; }
    void SetMoonTexture(GLuint t) { moon_texture_ = t; }
    

private:

    //star
    Mat4 model_matrix_star_;
    Mat3 normal_matrix_star_;
    Material star_material_;

    float star_rot_angle_;
    float star_rot_velocity_;

    GLuint star_texture_unit_;
    GLuint star_texture_;

    //planet
    Mat4 model_matrix_planet_;
    Mat3 normal_matrix_planet_;
    
    float planet_rot_angle_;
    float planet_rot_velocity_;
    float planet_orbit_angle_;
    float planet_orbit_velocity_;
    float planet_distance_;

    GLuint planet_texture_unit_;
    GLuint planet_texture_;

    //moon
    Mat4 model_matrix_moon_;
    Mat3 normal_matrix_moon_;

    float moon_rot_angle_;
    float moon_rot_velocity_;
    float moon_orbit_angle_;
    float moon_orbit_velocity_;
    float moon_distance_;

    GLuint moon_texture_unit_;
    GLuint moon_texture_;


    int n_; // mesh parameter
    int m_; // mesh parameter

    float r; //small radius
};

#endif // PLANETH
