#include "Application.hpp"

#include <iostream>

#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rendering
        
        // send object Kd, and lights intensities
        m_directionalLightDir = glm::normalize(m_directionalLightDir);
        glUniform3fv(m_uKd, 1, &m_Kd[0]);
        glUniform3fv(m_uDirectionalLightIntensity, 1, &m_directionalLightIntensity[0]);
        glUniform3fv(m_uPointLightIntensity, 1, &m_pointLightIntensity[0]);
        
        glm::mat4 MVMatrix;
        glm::mat4 MVPMatrix;
        glm::mat4 NormalMatrix;
        glm::vec3 directionnalLightDirViewSpace;
        glm::vec3 pointLightPositionViewSpace;
        
        // cube
        MVMatrix = m_viewController.getViewMatrix() * m_cubeModelMatrix;
        MVPMatrix = m_projectionMatrix * MVMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        directionnalLightDirViewSpace = glm::vec3(MVMatrix * glm::vec4(m_directionalLightDir, 0));
        pointLightPositionViewSpace = glm::vec3(MVMatrix * glm::vec4(m_pointLightPosition, 1));
        glUniformMatrix4fv(m_uModelViewMatrix, 1, GL_FALSE, &MVMatrix[0][0]);
        glUniformMatrix4fv(m_uModelViewProjMatrix, 1, GL_FALSE, &MVPMatrix[0][0]);
        glUniformMatrix4fv(m_uNormalMatrix, 1, GL_FALSE, &NormalMatrix[0][0]);
        glUniform3fv(m_uDirectionalLightDir, 1, &directionnalLightDirViewSpace[0]);
        glUniform3fv(m_uPointLightPosition, 1, &pointLightPositionViewSpace[0]);
        glBindVertexArray(m_vaoCube);
        glDrawElements(GL_TRIANGLES, m_geometryCube.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
        
        // sphere
        MVMatrix = m_viewController.getViewMatrix() * m_sphereModelMatrix;
        MVPMatrix = m_projectionMatrix * MVMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        directionnalLightDirViewSpace = glm::vec3(MVMatrix * glm::vec4(m_directionalLightDir, 0));
        pointLightPositionViewSpace = glm::vec3(MVMatrix * glm::vec4(m_pointLightPosition, 1));
        glUniformMatrix4fv(m_uModelViewMatrix, 1, GL_FALSE, &MVMatrix[0][0]);
        glUniformMatrix4fv(m_uModelViewProjMatrix, 1, GL_FALSE, &MVPMatrix[0][0]);
        glUniformMatrix4fv(m_uNormalMatrix, 1, GL_FALSE, &NormalMatrix[0][0]);
        glUniform3fv(m_uDirectionalLightDir, 1, &directionnalLightDirViewSpace[0]);
        glUniform3fv(m_uPointLightPosition, 1, &pointLightPositionViewSpace[0]);
        glBindVertexArray(m_vaoSphere);
        glDrawElements(GL_TRIANGLES, m_geometrySphere.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
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
            ImGui::SliderFloat3("Kd objects", &m_Kd[0], 0., 1.);
            ImGui::SliderFloat3("pos pointlight", &m_pointLightPosition[0], -5., 5.);
            ImGui::SliderFloat3("intensity pointlight", &m_pointLightIntensity[0], 0., 10);
            ImGui::SliderFloat3("dir dirlight", &m_directionalLightDir[0], -1, 1);
            ImGui::SliderFloat3("intensity dirlight", &m_directionalLightIntensity[0], 0., 2.);
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
    m_viewController(m_GLFWHandle.window())
{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file
    
    
    glEnable(GL_DEPTH_TEST);
    
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_NORMAL = 1;
    const GLuint VERTEX_ATTR_UV = 2;
    
    // Init for cube
    m_geometryCube = glmlv::makeCube();
    
    glGenBuffers(1, &m_vboCube);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboCube);
    glBufferStorage(GL_ARRAY_BUFFER, m_geometryCube.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_geometryCube.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &m_iboCube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboCube);
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, m_geometryCube.indexBuffer.size() * sizeof(uint32_t), m_geometryCube.indexBuffer.data(), 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glGenVertexArrays(1, &m_vaoCube);
    glBindVertexArray(m_vaoCube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboCube);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_UV);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboCube);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL,   3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));
    glVertexAttribPointer(VERTEX_ATTR_UV,       3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Init for sphere
    m_geometrySphere = glmlv::makeSphere(32);
        
    glGenBuffers(1, &m_vboSphere);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSphere);
    glBufferStorage(GL_ARRAY_BUFFER, m_geometrySphere.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_geometrySphere.vertexBuffer.data(), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &m_iboSphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboSphere);
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, m_geometrySphere.indexBuffer.size() * sizeof(uint32_t), m_geometrySphere.indexBuffer.data(), 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glGenVertexArrays(1, &m_vaoSphere);
    glBindVertexArray(m_vaoSphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboSphere);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_UV);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSphere);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL,   3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));
    glVertexAttribPointer(VERTEX_ATTR_UV,       3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // init shader
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "/forward.vs.glsl", m_ShadersRootPath / m_AppName / "/forward.fs.glsl" });
    m_uModelViewProjMatrix = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
    m_uModelViewMatrix = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
    m_uNormalMatrix = glGetUniformLocation(m_program.glId(), "uNormalMatrix");
    m_uDirectionalLightDir = glGetUniformLocation(m_program.glId(), "uDirectionalLightDir");
    m_uDirectionalLightIntensity = glGetUniformLocation(m_program.glId(), "uDirectionalLightIntensity");
    m_uPointLightPosition = glGetUniformLocation(m_program.glId(), "uPointLightPosition");
    m_uPointLightIntensity = glGetUniformLocation(m_program.glId(), "uPointLightIntensity");
    m_uKd = glGetUniformLocation(m_program.glId(), "uKd");
    m_program.use();
    
    // init matrices
    m_projectionMatrix = glm::perspective(glm::radians(70.f), m_nWindowWidth / (float) m_nWindowHeight, 0.1f, 100.f);
    m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    m_cubeModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(1, 0, 0));
    m_sphereModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-1, 0, 0));
    
    // init light & colors uniforms vec3
    m_directionalLightDir = glm::vec3(1, -1, 0);
    m_directionalLightIntensity = glm::vec3(1, 1, 1);
    m_pointLightPosition = glm::vec3(-1, 2, 0);
    m_pointLightIntensity = glm::vec3(10, 10, 10);
    m_Kd = glm::vec3(1, 0, 0);
}

Application::~Application()
{
    glDeleteBuffers(1, &m_vboCube);
    glDeleteBuffers(1, &m_vboSphere);
    glDeleteBuffers(1, &m_iboCube);
    glDeleteBuffers(1, &m_iboSphere);
    glDeleteVertexArrays(1, &m_vaoCube);
    glDeleteVertexArrays(1, &m_vaoSphere);
}

