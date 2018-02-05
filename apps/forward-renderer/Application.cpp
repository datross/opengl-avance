#include "Application.hpp"

#include <iostream>

#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/load_obj.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rendering
        
        
        glm::mat4 MVMatrix;
        glm::mat4 MVPMatrix;
        glm::mat4 NormalMatrix;
        
        MVMatrix = m_viewController.getViewMatrix();
        MVPMatrix = m_projectionMatrix * MVMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(m_uModelViewMatrix, 1, GL_FALSE, &MVMatrix[0][0]);
        glUniformMatrix4fv(m_uModelViewProjMatrix, 1, GL_FALSE, &MVPMatrix[0][0]);
        glUniformMatrix4fv(m_uNormalMatrix, 1, GL_FALSE, &NormalMatrix[0][0]);
        
        m_directionalLightDir = glm::normalize(m_directionalLightDir);
        glm::vec3 directionnalLightDirViewSpace = glm::vec3(m_viewController.getViewMatrix() * glm::vec4(m_directionalLightDir, 0));
        glUniform3fv(m_uDirectionalLightIntensity, 1, &m_directionalLightIntensity[0]);
        glUniform3fv(m_uDirectionalLightDir, 1, &directionnalLightDirViewSpace[0]);
        
        glBindVertexArray(m_vaoModel);
        
        for(GLuint i = 0; i < 4; ++i)
            glBindSampler(i, m_sampler);
        
        glUniform1i(m_uSamplerKa, 0);
        glUniform1i(m_uSamplerKd, 1);
        glUniform1i(m_uSamplerKs, 2);
        glUniform1i(m_uSamplerShininess, 3);
        
        auto indexOffset = 0;
        int shape = 0; // num of current shape
        for (const auto indexCount: m_objData.indexCountPerShape)
        {
            auto & material = m_objData.materialIDPerShape[shape] >= 0 ? 
            m_objData.materials[m_objData.materialIDPerShape[shape]] : m_defaultMaterial;
                 
            glUniform3fv(m_uKa, 1, &material.Ka[0]);
            glUniform3fv(m_uKd, 1, &material.Kd[0]);
            glUniform3fv(m_uKs, 1, &material.Ks[0]);
            glUniform1f(m_uShininess, material.shininess);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texIds[material.KaTextureId]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_texIds[material.KdTextureId]);            
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, m_texIds[material.KsTextureId]);            
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, m_texIds[material.shininessTextureId]);
            
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (const GLvoid*) (indexOffset * sizeof(GLuint)));
            indexOffset += indexCount;
            ++shape;
        }
        
        for(GLuint i = 0; i < 4; ++i)
            glBindSampler(i, 0);
        
        glBindVertexArray(0);
        
        // GUI code:
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
            if (ImGui::ColorEdit3("clearColor", clearColor)) {
                glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
            }
            ImGui::SliderFloat3("dir dirlight", &m_directionalLightDir[0], -1, 1);
            ImGui::SliderFloat3("intensity dirlight", &m_directionalLightIntensity[0], 0., 1.);
            ImGui::End();
        }

        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        ImGui::Render();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            m_viewController.update(float(ellapsedTime));
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
    m_AssetsRootPath { m_AppPath.parent_path() / "assets" },
    m_viewController(m_GLFWHandle.window())
{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file
    
    
    glEnable(GL_DEPTH_TEST);
    
    glmlv::loadObj(m_AssetsRootPath / m_AppName / "models/crytek-sponza/sponza.obj", m_objData);
    
    glGenBuffers(1, &m_vboModel);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboModel);
    glBufferStorage(GL_ARRAY_BUFFER, m_objData.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_objData.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &m_iboModel);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboModel);
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, m_objData.indexBuffer.size() * sizeof(uint32_t), m_objData.indexBuffer.data(), 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_NORMAL = 1;
    const GLuint VERTEX_ATTR_UV = 2;
    
    glGenVertexArrays(1, &m_vaoModel);
    glBindVertexArray(m_vaoModel);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboModel);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);  
    glEnableVertexAttribArray(VERTEX_ATTR_UV);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboModel);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL,   3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));
    glVertexAttribPointer(VERTEX_ATTR_UV,       2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // init shader
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "/forward.vs.glsl", m_ShadersRootPath / m_AppName / "/forward.fs.glsl" });
    m_uModelViewProjMatrix = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
    m_uModelViewMatrix = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
    m_uNormalMatrix = glGetUniformLocation(m_program.glId(), "uNormalMatrix");
    m_uSamplerKa = glGetUniformLocation(m_program.glId(), "uSamplerKa ");
    m_uSamplerKd = glGetUniformLocation(m_program.glId(), "uSamplerKd");
    m_uSamplerKs = glGetUniformLocation(m_program.glId(), "uSamplerKs");
    m_uSamplerShininess = glGetUniformLocation(m_program.glId(), "uSamplerShininess");
    m_uKa = glGetUniformLocation(m_program.glId(), "uKa");
    m_uKd = glGetUniformLocation(m_program.glId(), "uKd");
    m_uKs = glGetUniformLocation(m_program.glId(), "uKs");
    m_uShininess = glGetUniformLocation(m_program.glId(), "uShininess");
    m_uDirectionalLightDir = glGetUniformLocation(m_program.glId(), "uDirectionalLightDir");
    m_uDirectionalLightIntensity = glGetUniformLocation(m_program.glId(), "uDirectionalLightIntensity");
    m_program.use();
    
    // init matrices
    const auto sceneDiagonalSize = glm::length(m_objData.bboxMax - m_objData.bboxMin);
    m_projectionMatrix = glm::perspective(glm::radians(70.f), m_nWindowWidth / (float) m_nWindowHeight, 0.01f * sceneDiagonalSize, 100.f * sceneDiagonalSize);
    m_viewController.setSpeed(sceneDiagonalSize * 0.1f);
    m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    
    // textures
    m_texIds = std::vector<GLuint>(m_objData.textures.size());
    glGenTextures(m_texIds.size(), m_texIds.data());
    for(auto i = 0; i < m_texIds.size(); ++i) {
        glBindTexture(GL_TEXTURE_2D, m_texIds[i]);
        auto & tex = m_objData.textures[i];
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, tex.width(), tex.height());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex.width(), tex.height(), GL_RGBA, GL_UNSIGNED_BYTE, tex.data());
    }
    glGenTextures(1, &m_whiteTexture);
    glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1, 1);
    glm::vec4 white(1.f, 1.f, 1.f, 1.f);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, &white);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // make textureless materials pointing toward whiteTexture
    m_texIds.push_back(m_whiteTexture);
    for(auto & material : m_objData.materials) {
        if(material.KaTextureId < 0) material.KaTextureId = m_texIds.size() - 1;
        if(material.KdTextureId < 0) material.KdTextureId = m_texIds.size() - 1;
        if(material.KsTextureId < 0) material.KsTextureId = m_texIds.size() - 1;
        if(material.shininessTextureId < 0) material.shininessTextureId = m_texIds.size() - 1;
    }
    
    // samplers
    glGenSamplers(1, &m_sampler);
    glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   
    
    // lights
    m_directionalLightDir = glm::vec3(1, -1, 0);
    m_directionalLightIntensity = glm::vec3(1, 1, 1);
    
    // build default material
    auto whiteK = glm::vec3(1, 1, 1);
    m_defaultMaterial.Ka = m_defaultMaterial.Kd = m_defaultMaterial.Ks = whiteK;
    m_defaultMaterial.shininess = 0;
    m_defaultMaterial.KaTextureId 
        = m_defaultMaterial.KdTextureId 
        = m_defaultMaterial.KsTextureId
        = m_defaultMaterial.shininessTextureId = m_texIds.size() - 1;
}

Application::~Application()
{
    glDeleteBuffers(1, &m_vboModel);
    glDeleteBuffers(1, &m_iboModel);
    glDeleteVertexArrays(1, &m_vaoModel);

}

