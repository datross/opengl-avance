#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glmlv/ViewController.hpp>

class Application
{
public:
    Application(int argc, char** argv);
    ~Application();

    int run();
private:
    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    
    // Cube && sphere geometry
    GLuint m_vboCube, m_vboSphere;
    GLuint m_vaoCube, m_vaoSphere;   
    GLuint m_iboCube, m_iboSphere;
    glmlv::SimpleGeometry m_geometryCube, m_geometrySphere;
    
    // shaders
    glmlv::GLProgram m_program;

    // uniform matrices
    GLint m_uModelViewProjMatrix;
    GLint m_uModelViewMatrix;
    GLint m_uNormalMatrix;
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_cubeModelMatrix;
    glm::mat4 m_sphereModelMatrix;
    
    GLint m_uDirectionalLightDir;
    GLint m_uDirectionalLightIntensity;
    GLint m_uPointLightPosition;
    GLint m_uPointLightIntensity;
    glm::vec3 m_directionalLightDir;
    glm::vec3 m_directionalLightIntensity;
    glm::vec3 m_pointLightPosition;
    glm::vec3 m_pointLightIntensity;
    
    GLint m_uKd;
    glm::vec3 m_Kd;
    
    GLint m_uKdSampler;
    // TODO
    
    // view controller
    glmlv::ViewController m_viewController;
};
