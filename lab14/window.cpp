#include "window.h"

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdlib>

#include "glerror.h"

const char* kVertexShader = "TextureShader.vertex.glsl";
const char* kFragmentShader = "TextureShader.fragment.glsl";

const char* kColorTexureFile = "texture.tga";
const char* kTexturePlanet = "mars_1k_color.tga";
const char* kTextureMoon = "215.tga";
const char* kTextureSpace = "space.tga";

const int kPlaneM = 30;
const int kPlaneN = 40;

const int kTorusM = 20;
const int kTorusN = 30;
const float kTorusR = 2;
const float kTorus_r = 0.75;

Window::Window(const char* title, int width, int height)
{
    title_ = (char*)title;
    width_ = width;
    height_ = height;
    last_time_ = 0;
}

void Window::Initialize(int argc, char* argv[], int major_gl_version, int minor_gl_version)
{

    InitGlutOrDie(argc, argv, major_gl_version, minor_gl_version);
    InitGlewOrDie();

    std::cout << "OpenGL initialized: OpenGL version: " << glGetString(GL_VERSION) << " GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    InitTextures();
    InitModels();
    InitPrograms();

    view_matrix_.Translate(0, 0, -10);
    SetViewMatrix();

    projection_matrix_ = Mat4::CreateProjectionMatrix(60, (float)width_ / (float)height_, 0.1f, 100.0f);
    SetProjectionMatrix();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0, 0, 0, 0.0f);
}

void Window::InitGlutOrDie(int argc, char* argv[], int major_gl_version, int minor_gl_version)
{
    glutInit(&argc, argv);

    glutInitContextVersion(major_gl_version, minor_gl_version);
    glutInitContextProfile(GLUT_CORE_PROFILE);
#ifdef DEBUG
    glutInitContextFlags(GLUT_DEBUG);
#endif

    glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutInitWindowSize(width_, height_);

    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    int window_handle = glutCreateWindow(title_);

    if (window_handle < 1)
    {
        std::cerr << "ERROR: Could not create a new rendering window" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Window::InitGlewOrDie()
{
    GLenum glew_init_result;
    glewExperimental = GL_TRUE;
    glew_init_result = glewInit();

    if (GLEW_OK != glew_init_result)
    {
        std::cerr << "Glew ERROR: " << glewGetErrorString(glew_init_result) << std::endl;
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    if (glDebugMessageCallback)
    {
        std::cout << "Register OpenGL debug callback " << std::endl;
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback((GLDEBUGPROC)OpenglCallbackFunction, NULL);
        GLuint unused_ids = 0;
        glDebugMessageControl(GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            &unused_ids,
            GL_FALSE);
    }
    else
        std::cout << "glDebugMessageCallback not available" << std::endl;
#endif
}

void Window::InitTextures()
{
    color_texture_.Initialize(kColorTexureFile);
    planet_texture_.Initialize(kTexturePlanet);
    moon_texture_.Initialize(kTextureMoon);
    space_texture_.Initialize(kTextureSpace);
}

void Window::InitModels()
{
    plane_.Initialize(5, 5);
    plane_.SetTexture(space_texture_);
    plane_.SetTextureUnit(GL_TEXTURE0);

    torus_.Initialize(kTorusM, kTorusN, kTorusR, kTorus_r);
    torus_.SetTexture(color_texture_);
    torus_.SetTextureUnit(GL_TEXTURE0);

    planet_.Initialize(30, 30, 1);
    planet_.SetPlanetTexture(planet_texture_);
    planet_.SetPlanetTextureUnit(GL_TEXTURE0);
    planet_.SetMoonTexture(moon_texture_);
    planet_.SetMoonTextureUnit(GL_TEXTURE0);
}

void Window::InitPrograms()
{
    program_.Initialize(kVertexShader, kFragmentShader);
    program_.SetTextureUnit(0);
}

void Window::SetViewMatrix() const
{
    glUseProgram(program_);
    program_.SetViewMatrix(view_matrix_);
}

void Window::SetProjectionMatrix() const
{
    glUseProgram(program_);
    program_.SetProjectionMatrix(projection_matrix_);
}

void Window::Resize(int new_width, int new_height)
{
    width_ = new_width;
    height_ = new_height;
    projection_matrix_ = Mat4::CreateProjectionMatrix(60, (float)width_ / (float)height_, 0.1f, 100.0f);
    SetProjectionMatrix();
    glViewport(0, 0, width_, height_);
    glutPostRedisplay();
}

void Window::KeyPressed(unsigned char key, int /*x_coord*/, int /*y_coord*/)
{
    switch (key)
    {
    case 27:
        glutLeaveMainLoop();
        break;
    case ' ':
        torus_.ToggleAnimated();
        break;
    default:
        break;
    }
}

void Window::SpecialKeyPressed(int key, int /*x_coord*/, int /*y_coord*/)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        torus_.SlowDown();
        break;
    case GLUT_KEY_RIGHT:
        torus_.SpeedUp();
        break;
    default:
        break;
    }
}

void Window::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    clock_t now = clock();
    if (last_time_ == 0)
        last_time_ = now;
    torus_.Move((float)(now - last_time_) / CLOCKS_PER_SEC);
    planet_.Move((float)(now - last_time_) / CLOCKS_PER_SEC);
    last_time_ = now;

    plane_.Draw(program_);
    //torus_.Draw(program_);
    planet_.Draw(program_);

    glutSwapBuffers();
    glutPostRedisplay();
}
