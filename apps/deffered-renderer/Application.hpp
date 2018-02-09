#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <glmlv/load_obj.hpp>

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
    const glmlv::fs::path m_AssetsRootPath;
    
    glmlv::ObjData m_objData;
    
    GLuint m_vaoModel,
           m_vboModel,
           m_iboModel;
           
    // shader
    glmlv::GLProgram m_program;
    
    GLint m_uModelViewProjMatrix;
    GLint m_uModelViewMatrix;
    GLint m_uNormalMatrix;
    glm::mat4 m_projectionMatrix;
    
    // textures & materials
    std::vector<GLuint> m_texIds;
    GLuint m_sampler;
    GLint m_uSamplerKa,
          m_uSamplerKd,
          m_uSamplerKs,
          m_uSamplerShininess;
    GLint m_uKa,
          m_uKd,
          m_uKs,
          m_uShininess;
    glmlv::ObjData::PhongMaterial m_defaultMaterial;
    GLuint m_whiteTexture;
          
    // light
    GLint m_uDirectionalLightDir;
    GLint m_uDirectionalLightIntensity;
    glm::vec3 m_directionalLightDir;
    glm::vec3 m_directionalLightIntensity;
    
    glmlv::ViewController m_viewController;
    
    
    
     // specific to deffered shading
    
    enum GBufferTextureType
    {
        GPosition = 0,
        GNormal,
        GAmbient,
        GDiffuse,
        GGlossyShininess,
        GDepth, // On doit créer une texture de depth mais on écrit pas directement dedans dans le FS. OpenGL le fait pour nous (et l'utilise).
        GBufferTextureCount
    };
    GLuint m_GBufferTextures[GBufferTextureCount];    
    GLuint m_FBO;
    int m_blitPass = 1;
    
    glmlv::GLProgram m_shadingProgram;
    
    GLuint m_vaoTriangleBuffer,
           m_vboTriangleBuffer;
};
