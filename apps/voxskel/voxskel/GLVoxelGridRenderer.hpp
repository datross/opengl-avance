#pragma once

#include <glm/glm.hpp>

#include "Grid3D.hpp"
#include "glutils/GLutils.hpp"

namespace voxskel {

using VoxelGrid = Grid3D<uint8_t>;

class GLVoxelGrid {
public:
    GLuint m_VBO;
    GLuint m_VAO;
    size_t m_nVertexCount;
    GLuint m_TextureObject;

public:
    struct Vertex {
        glm::ivec3 m_position;

        Vertex(const glm::ivec3& position): m_position(position) {}
    };

    GLuint getTextureObject() const {
        return m_TextureObject;
    }

    GLVoxelGrid(): m_nVertexCount(0) {
        glGenBuffers(1, &m_VBO);
        glGenVertexArrays(1, &m_VAO);
        glGenTextures(1, &m_TextureObject);
    }

    ~GLVoxelGrid() {
        glDeleteBuffers(1, &m_VBO);
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteTextures(1, &m_TextureObject);
    }

    void init(const VoxelGrid& voxelGrid);

    void render() const {
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_POINTS, 0, m_nVertexCount);
    }
};

class GLVoxelGridRenderer {
public:
    GLVoxelGridRenderer(const std::string & shaderRootPath);

    void setProjectionMatrix(glm::mat4 projectionMatrix) {
        m_ProjectionMatrix = projectionMatrix;
    }

    void setViewMatrix(glm::mat4 viewMatrix) {
        m_ViewMatrix = viewMatrix;
    }

    void render(const glm::mat4& gridToWorld,
                const GLVoxelGrid& voxelGrid,
                GLuint colorTexture,
                bool useNeighborColors = true);

private:
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;

    struct RenderFacesPass {
        GLProgram m_Program;

        GLint m_uMVLocation;
        GLint m_uMVPLocation;
        GLint m_uCubicalComplexLocation;

        VOXSKEL_GLUNIFORM(m_Program, glm::vec3, uColor);
        VOXSKEL_GLUNIFORM(m_Program, GLint, uColorSampler);
        VOXSKEL_GLUNIFORM(m_Program, bool, uUseNeighborColors);

        RenderFacesPass(const std::string & shaderRootPath);
    };
    RenderFacesPass m_RenderFacesPass;
};

}
