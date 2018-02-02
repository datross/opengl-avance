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

        // Put here rendering code
        
        glm::mat4 MVMatrix;
        glm::mat4 MVPMatrix;
        glm::mat4 NormalMatrix;
        
        // cube
        MVMatrix = m_viewController.getViewMatrix() * m_cubeModelMatrix;
        MVPMatrix = m_projectionMatrix * MVMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(m_uModelViewMatrix, 1, GL_FALSE, &MVMatrix[0][0]);
        glUniformMatrix4fv(m_uModelViewProjMatrix, 1, GL_FALSE, &MVPMatrix[0][0]);
        glUniformMatrix4fv(m_uNormalMatrix, 1, GL_FALSE, &NormalMatrix[0][0]);
        glBindVertexArray(m_vaoCube);
        glDrawElements(GL_TRIANGLES, m_geometryCube.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
        
        // sphere
        MVMatrix = m_viewController.getViewMatrix() * m_sphereModelMatrix;
        MVPMatrix = m_projectionMatrix * MVMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(m_uModelViewMatrix, 1, GL_FALSE, &MVMatrix[0][0]);
        glUniformMatrix4fv(m_uModelViewProjMatrix, 1, GL_FALSE, &MVPMatrix[0][0]);
        glUniformMatrix4fv(m_uNormalMatrix, 1, GL_FALSE, &NormalMatrix[0][0]);
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
    m_geometrySphere = glmlv::makeSphere(8);
        
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
    m_program.use();
    
    // init matrices
    m_projectionMatrix = glm::perspective(glm::radians(70.f), m_nWindowWidth / (float) m_nWindowHeight, 0.1f, 100.f);
    m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    m_cubeModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(1, 0, 0));
    m_sphereModelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-1, 0, 0));
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

