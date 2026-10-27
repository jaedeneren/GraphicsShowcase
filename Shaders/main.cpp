/*****************************************************************************
\file main.cpp
\author Vadim Surov (vsurov@digipen.edu)
\co-author Wong E-Ren, Jaeden (erenajeden.w@digipen.edu)
\par Course: CSD2151
\par Assignment: 12 (Final)
\date 29/03/2026
\brief  All-in-one demo app for the final assignment, showcasing multiple shader studies 
in a tabbed interface with interactive controls and child windows.
*******************************************************************************/

#include <iostream>
#include <cmath>
#include <vector>
#include <utility>

#include "windowmanager.h"
#include "trianglemeshes.h"

const int NOMARGIN = 0;
const int NUM_TABS = 3;

// tab buttons (for your main shader UI)
float btnY = 26.f;
float btnRadius = 14.f;
float btnSpacing = 54.f;

int main(int argc, char** argv)
{
    int retcode = 0;
    auto& windowManager = cg::WindowManager::getInstance();

    try
    {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW.");

#ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        GLFWwindow* pMainWindow = nullptr;
        cg::ManagedWindow::MousedoubleclickCallback openChildWindow =
            [](cg::ManagedWindow* pWindow, int, int)
            {
                if (pWindow && pWindow->pParentWindow)
                    cg::WindowManager::getInstance().copyWindow(*pWindow);
            };
        cg::ManagedWindow::FocusCallback assignment1Focus =
            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (focused)
                {
                    std::cout << "\nShaders Framework (version "
                              << __DATE__ << " " << __TIME__ << ")\n"
                              << "Student   : Wong E-Ren, Jaeden\n"
                              << "School    : DigiPen Institute of Technology Singapore\n"
                              << "Instructor: Vadim Surov\n"
                              << R"(
Assignment 1:
   Three interactive 2D shader studies are presented as tabbed mini-demos:
   a scrolling checkerboard, animated corner lights, and a mouse-driven spotlight.
Controls:
   - Select Tab: Click a button or press Tab
   - Interact: Move the mouse over Tab 3
   - Resize: Drag window edges
   - Fullscreen: Space
   - Open Child In New Window: Double-click the child window or press Enter
   - Screenshot: P
   - Exit: Esc
)";
                }
            };
        cg::ManagedWindow::InitCallback assignment1Init =
            [](cg::ManagedWindow* pWindow, int pass)
            {
                if (!pWindow) return;

                // Keep the Assignment 1 windows animating smoothly even when a child
                // window is not focused, otherwise small child windows only repaint on
                // discrete hover events and look choppy.
                pWindow->needUpdate = true;

                int tab = pWindow->context.intparam[0] % NUM_TABS;
                if (tab < 0) tab += NUM_TABS;
                pWindow->context.intparam[0] = tab;

                if (pass >= 0 && pass < (int)pWindow->passes.size())
                {
                    auto& shader = pWindow->passes[pass].renderShader;
                    shader.use();

                    shader.setUniform("u_tab", tab);
                    shader.setUniform("iTime", (float)glfwGetTime());
                    shader.setUniform("u_mouse", pWindow->context.vec2param[1]);

                    shader.setUniform("u_btnY", btnY);
                    shader.setUniform("u_btnRadius", btnRadius);
                    shader.setUniform("u_spacing", btnSpacing);
                }
            };
        cg::ManagedWindow::SizeCallback assignment1Resize =
            [](cg::ManagedWindow* pWindow, int width, int height) {};
        cg::ManagedWindow::SizeCallback mainWindowResize =
            [](cg::ManagedWindow* pWindow, int width, int height)
            {
                if (!pWindow) return;
                cg::WindowManager::getInstance().resizeChildren(pWindow->pWindow, width, height);
            };
        cg::ManagedWindow::KeyCallback assignment1Key =
            [](cg::ManagedWindow* pWindow, int key)
            {
                if (!pWindow) return true;

                if (key == GLFW_KEY_TAB)
                {
                    pWindow->context.intparam[0] = pWindow->context.intparam[0] + 1;
                    pWindow->needUpdate = true;
                }
                return true;
            };
        cg::ManagedWindow::MousebuttonCallback assignment1Mouse =
            [](cg::ManagedWindow* pWindow, int button, int xpos, int ypos)
            {
                if (!pWindow) return;
                if (button != GLFW_MOUSE_BUTTON_LEFT) return;

                float mouseX = (float)xpos;
                float mouseY = (float)(pWindow->height - ypos);
                float centerX = pWindow->width * 0.5f;

                for (int i = 0; i < 3; ++i)
                {
                    float btnCenterX = centerX + (float)(i - 1) * btnSpacing;
                    float btnCenterY = btnY;

                    float dx = mouseX - btnCenterX;
                    float dy = mouseY - btnCenterY;

                    if (std::sqrt(dx * dx + dy * dy) <= btnRadius)
                    {
                        pWindow->context.intparam[0] = i;
                        pWindow->needUpdate = true;
                        break;
                    }
                }
            };
        cg::ManagedWindow::ScrollCallback assignment1Scroll =
            [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods) {};
        cg::ManagedWindow::CursorposCallback assignment1Cursor =
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods)
            {
                if (!pWindow) return;
                float mx = (float)xpos;
                float my = (float)(pWindow->height - ypos);
                pWindow->context.vec2param[1] = glm::vec2(mx, my);
                pWindow->needUpdate = true;
            };

        // =========================
        // Main window
        // =========================
        pMainWindow = windowManager.addWindow({
            cg::State::SHOW,
            nullptr,
            DEFAULT,
            "Shaders Framework",
            NONE,
            NONE,
            NONE,
            {
                {
                    { NOMARGIN },
                    NONE,
                    DISABLE,
                    { { QUAD } },
                    NONE,
                    NONE,
                    DEFAULT,
                    {
                        #include "Shaders/Shader.vert.glsl"
                        ,
                        #include "Shaders/Shader.frag.glsl"
                    }
                }
            },

            assignment1Focus,
            assignment1Init,
            mainWindowResize,
            assignment1Key,
            assignment1Mouse,
            assignment1Scroll,
            assignment1Cursor
            });

        // =========================
        // Child 1: Assignment 1
        // =========================
        windowManager.addWindow({
            cg::State::SHOW,
            pMainWindow,
            { 0, 9, 10 },
            "Assignment 1",
            NONE,
            NONE,
            NONE,
            {
                {
                    { NOMARGIN },
                    NONE,
                    DISABLE,
                    { { QUAD } },
                    NONE,
                    NONE,
                    DEFAULT,
                    {
                        #include "Shaders/Shader.vert.glsl"
                        ,
                        #include "Shaders/Shader.frag.glsl"
                    }
                }
            },

            assignment1Focus,
            assignment1Init,
            assignment1Resize,
            assignment1Key,
            assignment1Mouse,
            assignment1Scroll,
            assignment1Cursor,

            nullptr,
            openChildWindow
            });

        // =========================
        // Child 2: RayCasting
        // =========================
        windowManager.addWindow({
            cg::State::SHOW,
            pMainWindow,
            { 1, 9, 10 },
            "RayCasting",
            NONE,
            NONE,
            NONE,
            {
                {
                    { NOMARGIN },
                    NONE,
                    DISABLE,
                    { { QUAD } },
                    NONE,
                    NONE,
                    DEFAULT,
                    {
                        #include "Shaders/Shader.vert.glsl"
                        ,
                        #include "Shaders/RayCasting.frag.glsl"
                    }
                }
            },

            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (focused && pWindow)
                    std::cout
                        << "\nAssignment 2 (Ray Casting)\n"
                        << "   This demo ray casts a simple room made of walls, floor, ceiling, and boxes\n"
                        << "   to show analytic intersections and reflective floor shading.\n"
                        << "Controls:\n"
                        << "   - Move Camera: W, A, S, D\n"
                        << "   - Turn Camera: Drag with the left mouse button\n";
            },

            [](cg::ManagedWindow* pWindow, int pass)
            {
                if (!pWindow) return;

                if (pass >= 0 && pass < (int)pWindow->passes.size())
                {
                    auto& shader = pWindow->passes[pass].renderShader;
                    shader.use();

                    shader.setUniform("u_res", glm::vec2(
                        (float)pWindow->passes[pass].width,
                        (float)pWindow->passes[pass].height));

                    glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[0]);
                    glm::vec3 target = glm::vec3(pWindow->context.vec4param[1]);

                    if (glm::length(camPos) < 0.001f)
                    {
                        camPos = glm::vec3(0.0f, 4.0f, 5.0f);
                        pWindow->context.vec4param[0] = glm::vec4(camPos, 0.0f);
                    }
                    if (glm::length(target) < 0.001f)
                    {
                        target = glm::vec3(0.0f, 4.0f, 0.0f);
                        pWindow->context.vec4param[1] = glm::vec4(target, 0.0f);
                    }

                    glm::mat4 V = glm::lookAt(camPos, target, glm::vec3(0,1,0));
                    float aspect = (pWindow->passes[pass].height > 0)
                        ? (float)pWindow->passes[pass].width / (float)pWindow->passes[pass].height
                        : 1.0f;
                    glm::mat4 P = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

                    shader.setUniform("u_view", V);
                    shader.setUniform("u_proj", P);
                    shader.setUniform("u_camPos", camPos);
                }
            },

            [](cg::ManagedWindow* pWindow, int width, int height) {},

            [](cg::ManagedWindow* pWindow, int key)
            {
                if (!pWindow) return true;

                if (key == GLFW_KEY_TAB)
                {
                    pWindow->context.intparam[0] = pWindow->context.intparam[0] + 1;
                    pWindow->needUpdate = true;
                    return true;
                }

                glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[0]);
                glm::vec3 target = glm::vec3(pWindow->context.vec4param[1]);

                glm::vec3 forward = glm::normalize(glm::vec3(target.x - camPos.x, 0.0f, target.z - camPos.z));
                glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));

                const float step = 0.3f;
                bool moved = false;
                if (key == GLFW_KEY_W) { camPos += forward * step; target += forward * step; moved = true; }
                else if (key == GLFW_KEY_S) { camPos -= forward * step; target -= forward * step; moved = true; }
                else if (key == GLFW_KEY_A) { camPos -= right * step; target -= right * step; moved = true; }
                else if (key == GLFW_KEY_D) { camPos += right * step; target += right * step; moved = true; }

                if (moved)
                {
                    pWindow->context.vec4param[0] = glm::vec4(camPos, 0.0f);
                    pWindow->context.vec4param[1] = glm::vec4(target, 0.0f);
                    pWindow->needUpdate = true;
                }
                return true;
            },

            [](cg::ManagedWindow* pWindow, int button, int xpos, int ypos)
            {
                if (button != GLFW_MOUSE_BUTTON_LEFT) return;
                pWindow->context.vec2param[2] = glm::vec2(0.0f);
                pWindow->context.floatparam[1] = pWindow->context.floatparam[0];
                pWindow->needUpdate = true;
            },

            [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods) {},

            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods)
            {
                if (!pWindow || !pWindow->context.input.lbutton_down) return;

                float mx = (float)xpos;
                const float sensitivity = 0.005f;

                glm::vec2 anchor = pWindow->context.vec2param[2];
                if (anchor.x == 0.0f)
                {
                    anchor = glm::vec2((float)pWindow->context.input.pressx,
                                     (float)pWindow->context.input.pressy);
                    pWindow->context.vec2param[2] = anchor;
                    pWindow->context.floatparam[1] = pWindow->context.floatparam[0];
                }

                float yaw = pWindow->context.floatparam[1] - (mx - anchor.x) * sensitivity;
                pWindow->context.floatparam[0] = yaw;

                glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[0]);
                glm::vec3 forwardXZ = glm::normalize(glm::vec3(std::sin(yaw), 0.0f, -std::cos(yaw)));
                pWindow->context.vec4param[1] = glm::vec4(camPos + forwardXZ * 5.0f, 0.0f);
                pWindow->needUpdate = true;
            },

            nullptr,
            openChildWindow
            });

        // =========================
        // Child 3: Solar System
        // =========================
        windowManager.addWindow({
            cg::State::SHOW,
            pMainWindow,
            { 2, 9, 10 },
            "SolarSystem",
            NONE,
            NONE,
            NONE,
            {
                {
                    { NOMARGIN },
                    NONE,
                    DISABLE,
                    { { QUAD } },
                    NONE,
                    NONE,
                    DEFAULT,
                    {
                        #include "Shaders/Shader.vert.glsl"
                        ,
                        #include "Shaders/SolarSystem.frag.glsl"
                    }
                }
            },

            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (focused && pWindow)
                    std::cout
                        << "\nAssignment 3 (Solar System)\n"
                        << "   This demo renders an animated procedural solar system with orbiting planets\n"
                        << "   and time-driven motion in the fragment shader.\n"
                        << "Controls:\n"
                        << "   - Animation: Plays automatically\n"
                        << "   - Resize Window: Adjust the viewport dynamically\n";
            },

            [](cg::ManagedWindow* pWindow, int pass)
            {
                if (!pWindow) return;
                if (pass < 0 || pass >= (int)pWindow->passes.size()) return;

                auto& shader = pWindow->passes[pass].renderShader;
                shader.use();
                pWindow->needUpdate = true;

                shader.setUniform("u_res", glm::vec2((float)pWindow->passes[pass].width,
                                                     (float)pWindow->passes[pass].height));
                shader.setUniform("iTime", (float)glfwGetTime());

                glm::vec3 camPos(0.0f, 5.0f, 15.0f);
                glm::vec3 target(0.0f);
                glm::mat4 V = glm::lookAt(camPos, target, glm::vec3(0,1,0));
                float aspect = (pWindow->passes[pass].height > 0)
                    ? (float)pWindow->passes[pass].width / (float)pWindow->passes[pass].height
                    : 1.0f;
                glm::mat4 P = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

                shader.setUniform("u_view", V);
                shader.setUniform("u_proj", P);
                shader.setUniform("u_camPos", camPos);
            },

            [](cg::ManagedWindow* pWindow, int w, int h) {},

            [](cg::ManagedWindow* pWindow, int key)
            {
                if (key == GLFW_KEY_TAB && pWindow)
                {
                    pWindow->context.intparam[0] = pWindow->context.intparam[0] + 1;
                    pWindow->needUpdate = true;
                }
                return true;
            },

            [](cg::ManagedWindow* pWindow, int button, int xpos, int ypos) {},
            [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods) {},
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods)
            {
                if (pWindow) pWindow->needUpdate = true;
            },

            nullptr,
            openChildWindow
            });

        // =========================
        // Child 3: Assignment 4 (your BlinnPhong block stays as-is)
        // =========================
        //Blinn Phong Shading window
        windowManager.addWindow({
            cg::State::SHOW,            // State
            pMainWindow,                // Parent
            { 3, 9, 10 },               // Viewport
            "Assignment 4",             // Title
            NONE,                       // Background color
            NONE,                       // Textures
            NONE,                       // Compute shader
            {                           // Rendering passes
                {                       // Pass 0
                    { NOMARGIN },
                    { 0.1f, 0.1f, 0.1f, 1.0f },
                    ENABLE,                     // Depth Test ENABLED
                    {
                // 1. FRONT: Torus
                {
                    TORUS,
                    cg::Material{
                        {"material.Ka", {0.1f, 0.05f, 0.0f}},
                        {"material.Kd", {1.0f, 0.5f, 0.0f}},
                        {"material.Ks", {1.0f, 1.0f, 1.0f}},
                        {"material.shininess", {32.0f}}
                    },
                    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f))
                },

            // 2. RIGHT: Teapot
            {
                TEAPOT,
                cg::Material{
                    {"material.Ka", {0.1f, 0.05f, 0.0f}},
                    {"material.Kd", {1.0f, 0.5f, 0.0f}},
                    {"material.Ks", {1.0f, 1.0f, 1.0f}},
                    {"material.shininess", {32.0f}}
                },
                glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f)), glm::radians(-90.0f), glm::vec3(0,1,0))
            },

            // 3. BACK: Sphere
            {
                SPHERE,
                cg::Material{
                    {"material.Ka", {0.1f, 0.05f, 0.0f}},
                    {"material.Kd", {1.0f, 0.5f, 0.0f}},
                    {"material.Ks", {1.0f, 1.0f, 1.0f}},
                    {"material.shininess", {32.0f}}
                },
                glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f))
            },

            // 4. LEFT: Cube
            {
                CUBE,
                cg::Material{
                    {"material.Ka", {0.1f, 0.05f, 0.0f}},
                    {"material.Kd", {1.0f, 0.5f, 0.0f}},
                    {"material.Ks", {1.0f, 1.0f, 1.0f}},
                    {"material.shininess", {32.0f}}
                },
                glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f)), glm::radians(90.0f), glm::vec3(0,1,0))
            },

            // 5. FLOOR: Plane
            {
                PLANE,
                cg::Material{
                    {"material.Ka", {0.0f, 0.0f, 0.2f}},
                    {"material.Kd", {0.2f, 0.2f, 1.0f}},
                    {"material.Ks", {0.5f, 0.5f, 0.5f}},
                    {"material.shininess", {16.0f}}
                },
                glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f)), glm::vec3(2.0f, 1.0f, 2.0f))
            }
    },
    NONE, // Camera

            // LIGHT: Start at (0, 10, 0)
            { cg::Light{ glm::vec3(0.0f, 10.0f, 0.0f) } },

            DEFAULT, // Render Target
            {
                #include "Shaders/BlinnPhong.vert.glsl"
                ,
                #include "Shaders/BlinnPhong.frag.glsl"
            }
        }
    },

            // FocusCallback
            [](cg::ManagedWindow* pWindow, int focused) {
                if (!focused) return;
                std::cout
                    << "\nAssignment 4 (Blinn-Phong Shading)\n"
                    << "   This demo shows multiple lit objects with animated transforms and interactive\n"
                    << "   camera and light control using Blinn-Phong shading.\n"
                    << "Controls:\n"
                    << "   - Orbit Camera: Drag with the left mouse button\n"
                    << "   - Zoom Camera: Scroll\n"
                    << "   - Move Light Height: Hold Shift and scroll\n"
                    << "   - Orbit Light: Hold Shift and drag\n";
            },

            // InitCallback
            [](cg::ManagedWindow* pWindow, int pass) {
                if (!pWindow) return;

                float time = (float)glfwGetTime();

                if (pass >= 0 && pass < static_cast<int>(pWindow->passes.size())) {
                    auto& shader = pWindow->passes[pass].renderShader;
                    shader.use();

                    // Transssformation Animations
                    auto& objects = pWindow->passes[pass].objects;

                    if (objects.size() >= 4) {
                        // 0. TORUS: Spin around Arbitrary Axis
                        objects[0].instances[0].modeltransform =
                            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f)) * glm::rotate(glm::mat4(1.0f), time, glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));

                        // 1. TEAPOT: Spin around World Y-Axis
                        objects[1].instances[0].modeltransform =
                            glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0,1,0));

                        // 2. SPHERE: Scale
                        float scale = 1.0f + 0.3f * std::sin(time * 2.0f);
                        objects[2].instances[0].modeltransform =
                            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(scale));

                        // 3. CUBE: 
                        objects[3].instances[0].modeltransform =
                            glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), time, glm::vec3(1.0f, 0.0f, 0.0f)) *
                            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0,1,0));
                    }
                    // ---------------------------------------------------------

                    // --- CAMERA LOGIC ---
                    glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[0]);
                    glm::vec3 target = glm::vec3(pWindow->context.vec4param[1]);

                    if (glm::length(camPos) < 0.001f && glm::length(target) < 0.001f) {
                        camPos = glm::vec3(0.0f, 0.0f, 0.0f);
                        target = glm::vec3(0.0f, 0.0f, -1.0f);
                        pWindow->context.vec4param[0] = glm::vec4(camPos, 0.0f);
                        pWindow->context.vec4param[1] = glm::vec4(target, 0.0f);
                    }

                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
                    glm::mat4 V = glm::lookAt(camPos, target, up);

                    float aspect = (pWindow->passes[pass].height > 0) ?
                        static_cast<float>(pWindow->passes[pass].width) / static_cast<float>(pWindow->passes[pass].height) : 1.0f;
                    glm::mat4 P = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

                    shader.setUniform("u_view", V);
                    shader.setUniform("u_proj", P);

                    // Send Light Uniforms
                    if (!pWindow->passes[pass].lights.empty()) {
                        pWindow->passes[pass].lights[0].setUniforms(&shader, 0);
                    }

                    pWindow->needUpdate = true;
                }
            },

            // SizeCallback
            [](cg::ManagedWindow* pWindow, int w, int h) {},

            // KeyCallback
            [](cg::ManagedWindow* pWindow, int key) {
                if (!pWindow) return true;
                if (key == GLFW_KEY_TAB) {
                    pWindow->context.intparam[0] = pWindow->context.intparam[0] + 1;
                    pWindow->needUpdate = true;
                    return true;
                }
                return true;
            },

            // MouseButtonCallback
            [](cg::ManagedWindow* pWindow, int button, int xpos, int ypos) {
                if (button != GLFW_MOUSE_BUTTON_LEFT) return;
                pWindow->context.vec2param[2] = glm::vec2(0.0f);
                pWindow->context.floatparam[1] = pWindow->context.floatparam[0];
                pWindow->context.floatparam[3] = pWindow->context.floatparam[2];
                pWindow->needUpdate = true;
            },

            // ScrollCallback (SHIFT for Light Height, Normal for Zoom)
            [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods) {
                 if (!pWindow) return;

                 // SHIFT + SCROLL = MOVE LIGHT UP/DOWN
                 if (mods & GLFW_MOD_SHIFT)
                 {
                     if (!pWindow->passes.empty() && !pWindow->passes[0].lights.empty()) {
                         auto& light = pWindow->passes[0].lights[0];
                         light.position.y += (float)yoffset * 1.0f;
                         if (light.position.y < 0.5f) light.position.y = 0.5f;
                         pWindow->needUpdate = true;
                     }
                 }
                 else
                 {
                     // NORMAL ZOOM
                     float speed = 0.5f;
                     glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[0]);
                     glm::vec3 target = glm::vec3(pWindow->context.vec4param[1]);
                     glm::vec3 forward = glm::normalize(target - camPos);
                     glm::vec3 offset = forward * (float)yoffset * speed;
                     camPos += offset;
                     target += offset;
                     pWindow->context.vec4param[0] = glm::vec4(camPos, 0.0f);
                     pWindow->context.vec4param[1] = glm::vec4(target, 0.0f);
                     pWindow->needUpdate = true;
                 }
            },

            // CursorPosCallback (SHIFT for Light Orbit, Normal for Camera Look)
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods) {
                if (!pWindow || !pWindow->context.input.lbutton_down) return;

                float mx = static_cast<float>(xpos);
                float my = static_cast<float>(ypos);
                glm::vec2 anchor = pWindow->context.vec2param[2];

                // anchor on first click
                if (anchor.x == 0.0f) {
                    anchor = glm::vec2(static_cast<float>(pWindow->context.input.pressx), static_cast<float>(pWindow->context.input.pressy));
                    pWindow->context.vec2param[2] = anchor;
                    pWindow->context.floatparam[1] = pWindow->context.floatparam[0];
                    pWindow->context.floatparam[3] = pWindow->context.floatparam[2];
                }

                // SHIFT + DRAG = MOVE LIGHT AROUND (Orbit)
                if (mods & GLFW_MOD_SHIFT)
                {
                    if (!pWindow->passes.empty() && !pWindow->passes[0].lights.empty()) {
                        auto& light = pWindow->passes[0].lights[0];
                        float dx = (mx - anchor.x) * 0.05f;
                        float dz = (my - anchor.y) * 0.05f;

                        light.position.x += dx;
                        light.position.z += dz;

                        // Reset anchor
                        pWindow->context.vec2param[2] = glm::vec2(mx, my);
                        pWindow->needUpdate = true;
                    }
                }
                // NORMAL DRAG = ROTATE CAMERA
                else
                {
                    const float sensitivity = 0.005f;
                    float yaw = pWindow->context.floatparam[1] - (mx - anchor.x) * sensitivity;
                    float pitch = pWindow->context.floatparam[3] - (my - anchor.y) * sensitivity;
                    float pitchLimit = 1.55f;
                    if (pitch > pitchLimit) pitch = pitchLimit;
                    if (pitch < -pitchLimit) pitch = -pitchLimit;

                    pWindow->context.floatparam[0] = yaw;
                    pWindow->context.floatparam[2] = pitch;

                    glm::vec3 forward;
                    forward.x = std::sin(yaw) * std::cos(pitch);
                    forward.y = std::sin(pitch);
                    forward.z = -std::cos(yaw) * std::cos(pitch);
                    forward = glm::normalize(forward);

                    glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[0]);
                    pWindow->context.vec4param[1] = glm::vec4(camPos + forward * 5.0f, 0.0f);
                    pWindow->needUpdate = true;
                }
            },

            nullptr,
            openChildWindow
            });


        // =========================
        // Child 4: Assignment 5 (Environment Mapping)
        // =========================
        windowManager.addWindow({
            cg::State::SHOW,
            pMainWindow,
            { 4, 9, 10 },
            "Environment Mapping (A5)",
            { 0.0f, 0.0f, 0.0f, 1.0f },
            NONE,
            NONE,
            {
                // Pass 0: Skybox
                {
                    { NOMARGIN },
                    { 0.0f, 0.0f, 0.0f, 1.0f },
                    DISABLE,
                    { { SKYBOX, NONE, glm::mat4(1.0f) } },
                    NONE,
                    NONE,
                    DEFAULT,
                    {
                        #include "Shaders/EnvMap.vert.glsl"
                        ,
                        #include "Shaders/EnvMap.frag.glsl"
                    }
                },
            // Pass 1: Interactive Object
            {
                { NOMARGIN },
                NONE,
                ENABLE,
                {
                    { SPHERE, NONE, glm::mat4(1.0f) },
                    { TEAPOT, NONE, glm::mat4(1.0f) },
                    { TORUS,  NONE, glm::mat4(1.0f) }
                },
                NONE,
                NONE,
                DEFAULT,
                {
                    #include "Shaders/EnvMap.vert.glsl"
                    ,
                    #include "Shaders/EnvMap.frag.glsl"
                }
            }
        },

            // FocusCallback 
            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (!pWindow || !focused) return;
                std::cout
                    << "\nAssignment 5 (Environment Mapping)\n"
                    << "   This demo compares reflection and refraction on multiple objects using an\n"
                    << "   environment cubemap and adjustable material response.\n"
                    << "Controls:\n"
                    << "   - Change Object: Press 1, 2, or 3.\n"
                    << "   - Orbit Object: Drag with the left mouse button.\n"
                    << "   - Zoom Camera: Scroll.\n"
                    << "   - Adjust Reflection: Hold Control and scroll.\n"
                    << "   - Adjust Eta: Hold Alt and scroll.\n";
            },

            // InitCallback 
            [](cg::ManagedWindow* pWindow, int pass)
            {
                if (!pWindow) return;
                if (pass < 0 || pass >= (int)pWindow->passes.size()) return;

                auto& passData = pWindow->passes[pass];
                auto& shader = passData.renderShader;
                shader.use();

                if (pWindow->context.floatparam[2] == 0.0f)
                {
                    pWindow->context.floatparam[0] = 0.6f;
                    pWindow->context.floatparam[1] = 0.25f;
                    pWindow->context.floatparam[2] = 10.0f;       // Initial distance
                    pWindow->context.floatparam[3] = 0.5f;
                    pWindow->context.floatparam[4] = 1.0f / 1.33f;
                }

                float yaw = pWindow->context.floatparam[0];
                float pitch = glm::clamp(pWindow->context.floatparam[1], -1.2f, 1.2f);
                float dist = glm::clamp(pWindow->context.floatparam[2], 1.5f, 50.0f);

                glm::vec3 camPos(
                    dist * std::cos(pitch) * std::sin(yaw),
                    dist * std::sin(pitch),
                    dist * std::cos(pitch) * std::cos(yaw)
                );

                glm::mat4 V = glm::lookAt(camPos, glm::vec3(0.0f), glm::vec3(0,1,0));
                float aspect = (passData.height > 0) ? (float)passData.width / (float)passData.height : 1.0f;
                glm::mat4 P = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 200.0f);

                shader.setUniform("u_view", V);
                shader.setUniform("u_proj", P);
                shader.setUniform("u_camPos", camPos);
                shader.setUniform("u_reflectionFactor", glm::clamp(pWindow->context.floatparam[3], 0.0f, 1.0f));
                shader.setUniform("u_eta", glm::clamp(pWindow->context.floatparam[4], 0.05f, 3.0f));
                shader.setUniform("u_isSkybox", (pass == 0) ? 1 : 0);

                auto& objs = passData.objects;
                if (pass == 0)
                {
                    objs[0].instances[0].isVisible = true;
                    objs[0].instances[0].modeltransform =
                        glm::translate(glm::mat4(1.0f), camPos) *
                        glm::scale(glm::mat4(1.0f), glm::vec3(-50.0f, 50.0f, 50.0f));
                }
                else
                {

                    for (auto& obj : objs) shader.setUniform("M", obj.instances[0].modeltransform);
                }
                pWindow->needUpdate = true;
            },

            [](cg::ManagedWindow*, int, int) {},

            // KeyCallback
            [](cg::ManagedWindow* pWindow, int key)
            {
                if (!pWindow) return true;

                // Object Switching for Pass 1
                if (key == GLFW_KEY_1 || key == GLFW_KEY_2 || key == GLFW_KEY_3) {
                    int targetIdx = (key == GLFW_KEY_1) ? 0 : (key == GLFW_KEY_2 ? 1 : 2);
                    auto& objects = pWindow->passes[1].objects;
                    for (int i = 0; i < (int)objects.size(); ++i) {
                        objects[i].instances[0].isVisible = (i == targetIdx);
                    }
                }

                if (key == GLFW_KEY_R) pWindow->context.floatparam[3] += 0.05f;
                if (key == GLFW_KEY_F) pWindow->context.floatparam[3] -= 0.05f;

                pWindow->needUpdate = true;
                return true;
            },

            // MousebuttonCallback
            [](cg::ManagedWindow* pWindow, int button, int, int)
            {
                if (!pWindow || button != GLFW_MOUSE_BUTTON_LEFT) return;
                pWindow->context.vec2param[2] = glm::vec2(0.0f);
                pWindow->context.vec2param[3] = glm::vec2(pWindow->context.floatparam[0], pWindow->context.floatparam[1]);
                pWindow->needUpdate = true;
            },

            // ScrollCallback
            [](cg::ManagedWindow* pWindow, double, double yoffset, int mods)
            {
                if (!pWindow) return;

                if (mods & GLFW_MOD_CONTROL)
                    pWindow->context.floatparam[3] = glm::clamp(pWindow->context.floatparam[3] + (float)yoffset * 0.05f, 0.0f, 1.0f);
                else if (mods & GLFW_MOD_ALT)
                    pWindow->context.floatparam[4] = glm::clamp(pWindow->context.floatparam[4] + (float)yoffset * 0.05f, 0.05f, 3.0f);
                else
                {
                    pWindow->context.floatparam[2] = glm::clamp(
                        pWindow->context.floatparam[2] - (float)yoffset * 1.2f, 1.5f, 50.0f
                    );
                }

                pWindow->needUpdate = true;
            },

            // CursorposCallback
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double, double, int)
            {
                if (!pWindow || !pWindow->context.input.lbutton_down) return;

                glm::vec2 anchor = pWindow->context.vec2param[2];
                if (anchor.x == 0.0f && anchor.y == 0.0f)
                {
                    anchor = glm::vec2((float)pWindow->context.input.pressx, (float)pWindow->context.input.pressy);
                    pWindow->context.vec2param[2] = anchor;
                    pWindow->context.vec2param[3] = glm::vec2(pWindow->context.floatparam[0], pWindow->context.floatparam[1]);
                }

                glm::vec2 start = pWindow->context.vec2param[3];
                float dx = (float)xpos - anchor.x;
                float dy = (float)ypos - anchor.y;

                const float sens = 0.008f;
                pWindow->context.floatparam[0] = start.x - dx * sens;
                pWindow->context.floatparam[1] = glm::clamp(start.y - dy * sens, -1.2f, 1.2f);

                pWindow->needUpdate = true;
            },

            nullptr,
            openChildWindow
            });
        // =========================
        // Child 5: Assignment 8 - Night Sky Skybox
        // =========================
        windowManager.addWindow({
            cg::State::SHOW,
            pMainWindow,
            { 5, 9, 10 },                          // Tab index 4 
            "Assignment 8 (NightSky)",
            { 0.0f, 0.0f, 0.0f, 1.0f },           // Black background
            {                                       // Textures (cubemap - 6 faces)
                {
                    "night/night_negx.png", "night/night_posx.png",
                    "night/night_posy.png", "night/night_negy.png",
                    "night/night_posz.png", "night/night_negz.png"
                }
            },
            NONE,                                   // Compute shader
            {                                       // Rendering passes
                {                                   // Pass 0 - Skybox
                    { NOMARGIN },
                    NONE,
                    DISABLE,
                    { { SKYBOX } },
                    { {{ 6.0f, 0.0f, 4.0f }} },
                    NONE,
                    DEFAULT,
                    {
                        #include "Shaders/Skybox.vert.glsl"
                        ,
                        #include "Shaders/Skybox.frag.glsl"
                    }
                },
                {                                   // Pass 1 - Scene objects
                    { NOMARGIN },
                    NONE,
                    ENABLE,
                    {
                        {
                        PLANE,
                        cg::Material{
                            {"material.Ka",        {0.1f, 0.1f, 0.1f}},
                            {"material.Kd",        {0.8f, 0.8f, 0.8f}},
                            {"material.Ks",        {0.2f, 0.2f, 0.2f}},
                            {"material.Shininess", {32.0f}},
                            {"material.effect",    {3.0f}}
                        },
                        glm::translate(glm::scale(glm::mat4(1.0f), {10.0f, 1.0f, 10.0f}), {0.0f, -1.0f, 0.0f})
                    },
                    {
                        TEAPOT,
                        cg::Material{
                            {"material.Ka",        {0.1f, 0.05f, 0.0f}},
                            {"material.Kd",        {0.8f, 0.4f, 0.1f}},
                            {"material.Ks",        {1.0f, 1.0f, 1.0f}},
                            {"material.Shininess", {32.0f}},
                            {"material.effect",    {2.0f}}
                        },
                        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))
                    },
                    {
                        CUBE,
                        cg::Material{
                            {"material.Ka",        {0.0f, 0.1f, 0.0f}},
                            {"material.Kd",        {0.0f, 0.8f, 0.2f}},
                            {"material.Ks",        {1.0f, 1.0f, 1.0f}},
                            {"material.Shininess", {32.0f}},
                            {"material.effect",    {1.0f}}
                        },
                        glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 0.0f, 0.0f))
                    },
                    {
                        SPHERE,
                        cg::Material{
                            {"material.Ka",        {0.05f, 0.0f, 0.1f}},
                            {"material.Kd",        {0.4f, 0.1f, 0.8f}},
                            {"material.Ks",        {1.0f, 1.0f, 1.0f}},
                            {"material.Shininess", {64.0f}},
                            {"material.effect",    {0.0f}}
                        },
                        glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, 0.0f))
                    },
                    },
                    { {{ 6.0f, 0.0f, 4.0f }} },     // Same camera as skybox pass
                    { {{ 4.0f, 3.0f, 4.0f }} },     // Light
                    DEFAULT,
                    {
                        #include "Shaders/NightSky.vert.glsl"
                        ,
                        #include "Shaders/NightSky.frag.glsl"
                    }
                }
            },

            // FocusCallback
            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (!pWindow || !focused) return;
                std::cout << "\nAssignment 8 (Night Sky)\n"
                          << "   This demo renders a dynamic night-sky scene with switchable walking and\n"
                          << "   orbit camera modes plus interactive light control.\n"
                          << "Controls:\n"
                          << "   - Orbit Camera: Drag with the left mouse button; scroll to zoom.\n"
                          << "   - Orbit Light: Hold Shift and drag; hold Shift and scroll to adjust distance.\n"
                          << "   - Camera Mode: Press W for walking or O for orbiting.\n";
            },

            // InitCallback
            [](cg::ManagedWindow* pWindow, int pass)
            {
                if (!pWindow) return;
                if (pass < 0 || pass >= (int)pWindow->passes.size()) return;

                auto& passData = pWindow->passes[pass];
                auto& shader = passData.renderShader;
                shader.use();

                // Init defaults on first run
                if (pWindow->context.floatparam[2] == 0.0f)
                {
                    pWindow->context.floatparam[0] = 0.6f;   // yaw
                    pWindow->context.floatparam[1] = 0.25f;  // pitch
                    pWindow->context.floatparam[2] = 8.0f;   // dist (used by orbit)
                    pWindow->context.intparam[0] = 0;       // 0=orbiting, 1=walking

                    float y = 0.6f, p = 0.25f, d = 8.0f;
                    pWindow->context.vec4param[2] = glm::vec4(
                        d * std::cos(p) * std::sin(y),
                        d * std::sin(p),
                        d * std::cos(p) * std::cos(y),
                        0.0f
                    );
                }

                float yaw = pWindow->context.floatparam[0];
                float pitch = glm::clamp(pWindow->context.floatparam[1], -1.2f, 1.2f);
                int   mode = pWindow->context.intparam[0];

                glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[2]);

                glm::vec3 target;
                if (mode == 0) // Orbiting
                {
                    target = glm::vec3(0.0f);
                }
                else // Walking
                {
                    glm::vec3 dir(
                        std::cos(pitch) * std::sin(yaw),
                        std::sin(pitch),
                        std::cos(pitch) * std::cos(yaw)
                    );
                    target = camPos + dir;
                }

                float aspect = (passData.height > 0)
                    ? (float)passData.width / (float)passData.height
                    : 16.0f / 9.0f;

                glm::mat4 V = glm::lookAt(camPos, target, glm::vec3(0, 1, 0));
                glm::mat4 P = glm::perspective(glm::radians(75.0f), aspect, 0.1f, 200.0f);

                if (pass == 0) // Skybox
                {
                    shader.setUniform("u_view", V);
                    shader.setUniform("u_proj", P);
                    shader.setUniform("skybox", 0);
                }
                else // Pass 1 - scene objects
                {
                    shader.setUniform("u_view", V);
                    shader.setUniform("u_proj", P);

                    glm::vec3 lightPos = glm::vec3(pWindow->context.vec4param[0]);
                    if (glm::length(lightPos) < 0.001f)
                        lightPos = glm::vec3(4.0f, 3.0f, 4.0f);
                    pWindow->context.vec4param[0] = glm::vec4(lightPos, 0.0f);

                    glm::vec4 lightViewPos = V * glm::vec4(lightPos, 1.0f);
                    shader.setUniform("lights[0].Position", lightViewPos);
                    shader.setUniform("lights[0].La", glm::vec3(0.2f));
                    shader.setUniform("lights[0].Ld", glm::vec3(1.0f));
                    shader.setUniform("lights[0].Ls", glm::vec3(1.0f));
                    shader.setUniform("numLights", 1);
                }

                pWindow->needUpdate = true;
            },

            // SizeCallback
            [](cg::ManagedWindow* pWindow, int, int)
            {
                if (pWindow) pWindow->needUpdate = true;
            },

            // KeyCallback
            [](cg::ManagedWindow* pWindow, int key) -> bool
            {
                if (!pWindow) return true;
                if (key == 'W')
                {
                    if (pWindow->context.intparam[0] == 1) return false; // already walking, do nothing

                    std::cout << "Option W - Walking camera\n";
                    // Convert orbit camPos to walking look direction
                    glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[2]);
                    glm::vec3 toOrigin = glm::normalize(-camPos);
                    pWindow->context.floatparam[0] = std::atan2(toOrigin.x, toOrigin.z);
                    pWindow->context.floatparam[1] = std::asin(glm::clamp(toOrigin.y, -1.0f, 1.0f));
                    pWindow->context.intparam[0] = 1;
                    pWindow->needUpdate = true;
                    return false;
                }
                if (key == 'O')
                {
                    if (pWindow->context.intparam[0] == 0) return false;

                    std::cout << "Option O - Orbiting camera\n";
                    glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[2]);
                    float dist = glm::length(camPos);
                    if (dist < 1.5f) dist = 1.5f;

                    // Sync yaw/pitch to match camPos position on the sphere
                    // so dragging in orbit mode starts from the right angle
                    pWindow->context.floatparam[0] = std::atan2(camPos.x, camPos.z);
                    pWindow->context.floatparam[1] = std::asin(glm::clamp(camPos.y / dist, -1.0f, 1.0f));
                    pWindow->context.floatparam[2] = dist;
                    pWindow->context.intparam[0] = 0;
                    pWindow->needUpdate = true;
                    return false;
                }
                return true;
            },

            // MousebuttonCallback
            [](cg::ManagedWindow* pWindow, int button, int, int)
            {
                if (!pWindow || button != GLFW_MOUSE_BUTTON_LEFT) return;
                pWindow->context.vec2param[2] = glm::vec2(0.0f);
                pWindow->context.vec2param[3] = glm::vec2(
                    pWindow->context.floatparam[0],
                    pWindow->context.floatparam[1]
                );
            },

            // ScrollCallback
            [](cg::ManagedWindow* pWindow, double, double yoffset, int mods)
            {
                if (!pWindow) return;
                if (mods & GLFW_MOD_SHIFT)
                {
                    glm::vec3 lp = glm::vec3(pWindow->context.vec4param[0]);
                    if (glm::length(lp) < 0.001f) lp = glm::vec3(4.0f, 3.0f, 4.0f);
                    lp.y = glm::clamp(lp.y + (float)yoffset * 0.5f, 0.5f, 20.0f);
                    pWindow->context.vec4param[0] = glm::vec4(lp, 0.0f);
                }
                else
                {
                    glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[2]);
                    int mode = pWindow->context.intparam[0];

                    if (mode == 1) // Walking 
                    {
                        float yaw = pWindow->context.floatparam[0];
                        glm::vec3 dir = glm::normalize(glm::vec3(std::sin(yaw), 0.0f, std::cos(yaw)));
                        camPos += dir * (float)yoffset * 0.5f;
                    }
                    else // Orbiting 
                    {
                        glm::vec3 toOrigin = glm::normalize(-camPos);
                        camPos += toOrigin * (float)yoffset * 0.5f;
                        if (glm::length(camPos) < 1.5f)
                            camPos = glm::normalize(camPos) * 1.5f;
                    }

                    pWindow->context.vec4param[2] = glm::vec4(camPos, 0.0f);
                }
                pWindow->needUpdate = true;
            },

            // CursorposCallback
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double, double, int mods)
            {
                if (!pWindow || !pWindow->context.input.lbutton_down) return;

                glm::vec2 anchor = pWindow->context.vec2param[2];
                if (anchor.x == 0.0f && anchor.y == 0.0f)
                {
                    anchor = glm::vec2(
                        (float)pWindow->context.input.pressx,
                        (float)pWindow->context.input.pressy
                    );
                    pWindow->context.vec2param[2] = anchor;
                    pWindow->context.vec2param[3] = glm::vec2(
                        pWindow->context.floatparam[0],
                        pWindow->context.floatparam[1]
                    );
                }

                if (mods & GLFW_MOD_SHIFT)
                {
                    glm::vec3 lp = glm::vec3(pWindow->context.vec4param[0]);
                    if (glm::length(lp) < 0.001f) lp = glm::vec3(4.0f, 3.0f, 4.0f);
                    lp.x += ((float)xpos - anchor.x) * 0.05f;
                    lp.z += ((float)ypos - anchor.y) * 0.05f;
                    pWindow->context.vec4param[0] = glm::vec4(lp, 0.0f);
                    pWindow->context.vec2param[2] = glm::vec2((float)xpos, (float)ypos);
                }
                else
                {
                    float yaw = pWindow->context.vec2param[3].x - ((float)xpos - anchor.x) * 0.005f;
                    float pitch = glm::clamp(
                        pWindow->context.vec2param[3].y - ((float)ypos - anchor.y) * 0.005f,
                        -1.2f, 1.2f
                    );
                    pWindow->context.floatparam[0] = yaw;
                    pWindow->context.floatparam[1] = pitch;

                    int mode = pWindow->context.intparam[0];
                    if (mode == 0) // Orbit 
                    {
                        float dist = glm::length(glm::vec3(pWindow->context.vec4param[2]));
                        if (dist < 1.5f) dist = 1.5f;
                        pWindow->context.vec4param[2] = glm::vec4(
                            dist * std::cos(pitch) * std::sin(yaw),
                            dist * std::sin(pitch),
                            dist * std::cos(pitch) * std::cos(yaw),
                            0.0f
                        );
                    }
                    // Walking
                }

                pWindow->needUpdate = true;
            },

            nullptr,
            openChildWindow
            });

        // =========================
        // Child 6: Assignment 9 (Cartoon)
        // =========================
        GLuint a9PhongTex = 0;
        GLuint a9BlurVTex = 0;
        GLuint a9BlurHTex = 0;
        GLuint a9EdgeTex = 0;
        GLuint a9CartoonTex = 0;

        windowManager.addWindow({
            cg::State::SHOW,
            pMainWindow,
            { 6, 9, 10 },
            "Assignment 9 (Cartoon)",
            { 1.0f, 1.0f, 1.0f, 1.0f },
            {
                { a9PhongTex,   GL_TEXTURE0 },
                { a9BlurVTex,   GL_TEXTURE1 },
                { a9BlurHTex,   GL_TEXTURE2 },
                { a9EdgeTex,    GL_TEXTURE3 },
                { a9CartoonTex, GL_TEXTURE4 }
            },
            NONE,
            {
                // Pass 0: Blinn-Phong scene for texture0
                {
                    { NOMARGIN },
                    { 1.0f, 1.0f, 1.0f, 1.0f },
                    ENABLE,
                    {
                        {
                            PLANE,
                            cg::Material{
                                {"material.Ka",        {0.05f, 0.08f, 0.18f}},
                                {"material.Kd",        {0.20f, 0.45f, 0.95f}},
                                {"material.Ks",        {0.15f, 0.15f, 0.15f}},
                                {"material.shininess", {32.0f}},
                                {"material.effect",    {3.0f}}
                            },
                            glm::translate(
                                glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 2.0f)),
                                glm::vec3(0.0f, -1.0f, 0.0f)
                            )
                        },
                        {
                            TEAPOT,
                            cg::Material{
                                {"material.Ka",        {0.12f, 0.06f, 0.01f}},
                                {"material.Kd",        {1.00f, 0.55f, 0.10f}},
                                {"material.Ks",        {1.00f, 1.00f, 1.00f}},
                                {"material.shininess", {32.0f}},
                                {"material.effect",    {2.0f}}
                            },
                            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
                            glm::scale(glm::mat4(1.0f), glm::vec3(1.8f, 1.8f, 1.8f))
                        }
                    },
                    NONE,
                    { { { 0.0f, 6.0f, 0.0f } } },
                    {
                        cg::FBO::Type::Texture,
                        GL_TEXTURE0,
                        &a9PhongTex
                    },
                    {
                        #include "Shaders/BlinnPhong.vert.glsl"
                        ,
                        #include "Shaders/BlinnPhong.frag.glsl"
                    }
                },

            // Pass 1: Vertical blur for texture1
            {
                { NOMARGIN },
                NONE,
                DISABLE,
                { { QUAD } },
                NONE,
                NONE,
                {
                    cg::FBO::Type::Texture,
                    GL_TEXTURE1,
                    &a9BlurVTex
                },
                {
                    #include "Shaders/PassThrough.vert.glsl"
                    ,
                    #include "Shaders/A9BlurV.frag.glsl"
                }
            },

            // Pass 2: Horizontal blur for texture2
            {
                { NOMARGIN },
                NONE,
                DISABLE,
                { { QUAD } },
                NONE,
                NONE,
                {
                    cg::FBO::Type::Texture,
                    GL_TEXTURE2,
                    &a9BlurHTex
                },
                {
                    #include "Shaders/PassThrough.vert.glsl"
                    ,
                    #include "Shaders/A9BlurH.frag.glsl"
                }
            },

            // Pass 3: Sobel edge for texture3
            {
                { NOMARGIN },
                NONE,
                DISABLE,
                { { QUAD } },
                NONE,
                NONE,
                {
                    cg::FBO::Type::Texture,
                    GL_TEXTURE3,
                    &a9EdgeTex
                },
                {
                    #include "Shaders/PassThrough.vert.glsl"
                    ,
                    #include "Shaders/A9Sobel.frag.glsl"
                }
            },

            // Pass 4: Cartoon scene for texture4
            {
                { NOMARGIN },
                { 1.0f, 1.0f, 1.0f, 1.0f },
                ENABLE,
                {
                    {
                        PLANE,
                        cg::Material{
                            {"material.Ka",        {0.05f, 0.08f, 0.18f}},
                            {"material.Kd",        {0.20f, 0.45f, 0.95f}},
                            {"material.Ks",        {0.15f, 0.15f, 0.15f}},
                            {"material.shininess", {32.0f}},
                            {"material.effect",    {3.0f}}
                        },
                        glm::translate(
                            glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 2.0f)),
                            glm::vec3(0.0f, -1.0f, 0.0f)
                        )
                    },
                    {
                        TEAPOT,
                        cg::Material{
                            {"material.Ka",        {0.12f, 0.06f, 0.01f}},
                            {"material.Kd",        {1.00f, 0.55f, 0.10f}},
                            {"material.Ks",        {1.00f, 1.00f, 1.00f}},
                            {"material.shininess", {32.0f}},
                            {"material.effect",    {2.0f}}
                        },
                        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
                        glm::scale(glm::mat4(1.0f), glm::vec3(1.8f, 1.8f, 1.8f))
                    }
                },
                NONE,
                { { { 0.0f, 6.0f, 0.0f } } },
                {
                    cg::FBO::Type::Texture,
                    GL_TEXTURE4,
                    &a9CartoonTex
                },
                {
                    #include "Shaders/BlinnPhong.vert.glsl"
                    ,
                    #include "Shaders/A9Cartoon.frag.glsl"
                }
            },

            // Pass 5: Composite edge + cartoon 
            {
                { NOMARGIN },
                NONE,
                DISABLE,
                { { QUAD } },
                NONE,
                NONE,
                DEFAULT,
                {
                    #include "Shaders/PassThrough.vert.glsl"
                    ,
                    #include "Shaders/A9Composite.frag.glsl"
                }
            }
        },

            // FocusCallback
            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (!pWindow || !focused) return;

                std::cout
                    << "\nAssignment 9 (Cartoon)\n"
                    << "   This demo showcases a multi-pass cartoon-style rendering pipeline that creates\n"
                    << "   non-photorealistic effects with crisp edge outlines. The process applies a\n"
                    << "   Gaussian blur filter to reduce noise, followed by Sobel edge detection to\n"
                    << "   identify object boundaries. The scene is rendered twice: once with standard\n"
                    << "   Blinn-Phong shading for edge analysis, and again with cartoon-style shading\n"
                    << "   for the final cell-shaded appearance. The final pass composites the edge\n"
                    << "   outlines with the cartoon shading to produce a stylized 2D animation effect.\n"
                    << "Controls:\n"
                    << "   - Orbit Object: Drag with the left mouse button; scroll to zoom.\n"
                    << "   - Orbit Light: Hold Shift and drag; scroll to adjust distance.\n"
                    << "   - Blur Filter: Press 1 to enable and 2 to disable.\n"
                    << "   - Edge Filter: Press 3 to enable and 4 to disable.\n"
                    << "   - Edge Threshold: Hold Alt and scroll to adjust.\n"
                    << "   - Alpha Factor: Hold Control and scroll to adjust.\n";
            },

            // InitCallback
            [](cg::ManagedWindow* pWindow, int pass)
            {
                if (!pWindow) return;
                if (pass < 0 || pass >= (int)pWindow->passes.size()) return;

                auto& passData = pWindow->passes[pass];
                auto& shader = passData.renderShader;
                shader.use();

                // first run defaults
                if (pWindow->context.floatparam[2] == 0.0f)
                {
                    pWindow->context.floatparam[0] = 0.6f;   // yaw
                    pWindow->context.floatparam[1] = 0.25f;  // pitch
                    pWindow->context.floatparam[2] = 8.0f;   // distance
                    pWindow->context.floatparam[3] = 0.05f;  // edge threshold
                    pWindow->context.floatparam[4] = 0.0f;  // alpha factor
                    pWindow->context.intparam[0] = 1;        // blur enabled
                    pWindow->context.intparam[1] = 1;        // edge enabled
                }

                float yaw = pWindow->context.floatparam[0];
                float pitch = pWindow->context.floatparam[1];
                float dist = pWindow->context.floatparam[2];

                if (pitch > 1.55f) pitch = 1.55f;
                if (pitch < -1.55f) pitch = -1.55f;
                if (dist < 2.0f) dist = 2.0f;
                if (dist > 40.0f) dist = 40.0f;

                glm::vec3 camPos(
                    dist * std::cos(pitch) * std::sin(yaw),
                    dist * std::sin(pitch),
                    dist * std::cos(pitch) * std::cos(yaw)
                );

                glm::vec3 target(0.0f, 0.0f, 0.0f);

                float aspect = (passData.height > 0)
                    ? (float)passData.width / (float)passData.height
                    : 16.0f / 9.0f;

                glm::mat4 V = glm::lookAt(camPos, target, glm::vec3(0, 1, 0));
                glm::mat4 P = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

                // scene passes
                if (pass == 0 || pass == 4)
                {
                    shader.setUniform("u_view", V);
                    shader.setUniform("u_proj", P);

                    glm::vec3 lightPos = pWindow->passes[0].lights[0].position;

                    shader.setUniform("light[0].position", lightPos);
                    shader.setUniform("light[0].La", glm::vec3(0.2f));
                    shader.setUniform("light[0].Ld", glm::vec3(1.0f));
                    shader.setUniform("light[0].Ls", glm::vec3(1.0f));
                }

                // post passes
                shader.setUniform("BlurEnabled",  pWindow->context.intparam[0]);
                shader.setUniform("EdgeEnabled",  pWindow->context.intparam[1]);
                shader.setUniform("EdgeThreshold", pWindow->context.floatparam[3]);
                shader.setUniform("AlphaFactor",   pWindow->context.floatparam[4]);

                pWindow->needUpdate = true;
            },

            // SizeCallback
            [](cg::ManagedWindow* pWindow, int, int)
            {
                if (pWindow) pWindow->needUpdate = true;
            },

            // KeyCallback
            [](cg::ManagedWindow* pWindow, int key) -> bool
            {
                if (!pWindow) return true;

                if (key == GLFW_KEY_1) { pWindow->context.intparam[0] = 1; pWindow->needUpdate = true; }
                if (key == GLFW_KEY_2) { pWindow->context.intparam[0] = 0; pWindow->needUpdate = true; }
                if (key == GLFW_KEY_3) { pWindow->context.intparam[1] = 1; pWindow->needUpdate = true; }
                if (key == GLFW_KEY_4) { pWindow->context.intparam[1] = 0; pWindow->needUpdate = true; }

                return true;
            },

            // MouseButtonCallback
            [](cg::ManagedWindow* pWindow, int button, int xpos, int ypos)
            {
                if (!pWindow) return;
                if (button != GLFW_MOUSE_BUTTON_LEFT) return;

                pWindow->context.vec2param[2] = glm::vec2(0.0f, 0.0f);
                pWindow->context.vec2param[3] = glm::vec2(
                    pWindow->context.floatparam[0],
                    pWindow->context.floatparam[1]
                );
                pWindow->needUpdate = true;
            },

            // ScrollCallback
            [](cg::ManagedWindow* pWindow, double, double yoffset, int mods)
            {
                if (!pWindow) return;

                if (mods & GLFW_MOD_SHIFT)
                {
                    if (!pWindow->passes.empty() && !pWindow->passes[0].lights.empty())
                    {
                        auto& light = pWindow->passes[0].lights[0];
                        light.position.y += (float)yoffset * 0.5f;
                        if (light.position.y < 0.5f) light.position.y = 0.5f;
                        pWindow->needUpdate = true;
                    }
                }
                else if (mods & GLFW_MOD_ALT)
                {
                    // scroll up => LOWER threshold => MORE detected edges
                    pWindow->context.floatparam[3] -= (float)yoffset * 0.01f;
                    if (pWindow->context.floatparam[3] < 0.001f) pWindow->context.floatparam[3] = 0.001f;
                    if (pWindow->context.floatparam[3] > 0.25f)  pWindow->context.floatparam[3] = 0.25f;
                    pWindow->needUpdate = true;
                }
                else if (mods & GLFW_MOD_CONTROL)
                {
                    pWindow->context.floatparam[4] += (float)yoffset * 0.05f;
                    if (pWindow->context.floatparam[4] < 0.0f) pWindow->context.floatparam[4] = 0.0f;
                    if (pWindow->context.floatparam[4] > 1.0f) pWindow->context.floatparam[4] = 1.0f;
                    pWindow->needUpdate = true;
                }
                else
                {
                    pWindow->context.floatparam[2] -= (float)yoffset * 0.5f;
                    if (pWindow->context.floatparam[2] < 2.0f)  pWindow->context.floatparam[2] = 2.0f;
                    if (pWindow->context.floatparam[2] > 40.0f) pWindow->context.floatparam[2] = 40.0f;
                    pWindow->needUpdate = true;
                }
            },

            // CursorPosCallback
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double, double, int mods)
            {
                if (!pWindow || !pWindow->context.input.lbutton_down) return;

                float mx = (float)xpos;
                float my = (float)ypos;

                glm::vec2 anchor = pWindow->context.vec2param[2];
                if (anchor.x == 0.0f && anchor.y == 0.0f)
                {
                    anchor = glm::vec2(
                        (float)pWindow->context.input.pressx,
                        (float)pWindow->context.input.pressy
                    );
                    pWindow->context.vec2param[2] = anchor;
                    pWindow->context.vec2param[3] = glm::vec2(
                        pWindow->context.floatparam[0],
                        pWindow->context.floatparam[1]
                    );
                }

                if (mods & GLFW_MOD_SHIFT)
                {
                    if (!pWindow->passes.empty() && !pWindow->passes[0].lights.empty())
                    {
                        auto& light = pWindow->passes[0].lights[0];
                        light.position.x += (mx - anchor.x) * 0.02f;
                        light.position.z += (my - anchor.y) * 0.02f;
                        pWindow->context.vec2param[2] = glm::vec2(mx, my);
                        pWindow->needUpdate = true;
                    }
                }
                else
                {
                    glm::vec2 start = pWindow->context.vec2param[3];
                    float dx = mx - anchor.x;
                    float dy = my - anchor.y;

                    pWindow->context.floatparam[0] = start.x - dx * 0.008f;
                    pWindow->context.floatparam[1] = start.y - dy * 0.008f;

                    if (pWindow->context.floatparam[1] > 1.55f)  pWindow->context.floatparam[1] = 1.55f;
                    if (pWindow->context.floatparam[1] < -1.55f) pWindow->context.floatparam[1] = -1.55f;

                    pWindow->needUpdate = true;
                }
            },

            nullptr,
            openChildWindow
            });
        // =========================
        // =========================
        // Child: A10 Skybox + Portal Projection
        // =========================
        windowManager.addWindow({
            cg::State::SHOW,
            pMainWindow,
            { 7, 9, 10 },
            "A10 Skybox + Portal",
            { 0.0f, 0.0f, 0.0f, 1.0f },
            {
                {
                    "Pisa/pisa_posx.hdr", "Pisa/pisa_negx.hdr",
                    "Pisa/pisa_posy.hdr", "Pisa/pisa_negy.hdr",
                    "Pisa/pisa_posz.hdr", "Pisa/pisa_negz.hdr"
                }
            },
            NONE,
            {
                {
                    { NOMARGIN },
                    NONE,
                    DISABLE,
                    {
                        {
                            SKYBOX,
                            cg::Material{},
                            glm::scale(glm::mat4(1.0f), glm::vec3(0.58f))
                        }
                    },
                    NONE,
                    NONE,
                    DEFAULT,
                    {
                        #include "Shaders/A10Skybox.vert.glsl"
                        ,
                        #include "Shaders/Skybox.frag.glsl"
                    }
                },
                {
                    { NOMARGIN },
                    NONE,
                    ENABLE,
                    {
                        {
                            QUAD,
                            cg::Material{},
                            glm::mat4(1.0f)
                        }
                    },
                    NONE,
                    NONE,
                    DEFAULT,
                    {
                        #include "Shaders/A10Portal.vert.glsl"
                        ,
                        #include "Shaders/A10Portal.frag.glsl"
                    }
                }
            },

            // FocusCallback
            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (focused && pWindow)
                {
                    // Print the Assignment 10 description and controls when this child gains focus.
                    std::cout
                        << "\nAssignment 10 (Portal)\n"
                        << "   This demo showcases the integration of cube mapping and projected texture\n"
                        << "   techniques with dynamic texture rendering. Cube mapping is used to render\n"
                        << "   the skybox, while projected textures visualize a gate (portal) to another\n"
                        << "   dimension or location in the world.\n"
                        << "Controls:\n"
                        << "   - Rotating And Moving Camera: Drag with the left mouse button and scroll\n"
                        << "     the mouse wheel.\n"
                        << "   - Open Portal: Click the left mouse button at the desired location to\n"
                        << "     place the new portal.\n";
                }
            },

            // InitCallback
            [](cg::ManagedWindow* pWindow, int pass)
            {
                if (!pWindow || pass < 0 || pass >= (int)pWindow->passes.size()) return;

                auto& passData = pWindow->passes[pass];
                auto& shader = passData.renderShader;
                shader.use();

                if (pWindow->context.intparam[4] == 0)
                {
                    // Initialize the walking camera state and spawn one portal in front of the viewer.
                    pWindow->context.intparam[4] = 1;
                    pWindow->context.vec4param[0] = glm::vec4(0.0f, 1.6f, 6.0f, 0.0f); // camera
                    pWindow->context.vec4param[2] = glm::vec4(0.0f, 1.6f, -2.0f, 1.0f); // portal
                    pWindow->context.floatparam[0] = 0.0f;       // yaw, look toward -Z
                    pWindow->context.floatparam[1] = -0.1f;      // pitch
                }

                // Rebuild the camera view/projection every frame from the stored yaw, pitch, and position.
                glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[0]);
                float yaw = pWindow->context.floatparam[0];
                float pitch = glm::clamp(pWindow->context.floatparam[1], -1.48f, 1.48f);

                glm::vec3 lookDir(
                    std::sin(yaw) * std::cos(pitch),
                    std::sin(pitch),
                    -std::cos(yaw) * std::cos(pitch)
                );
                lookDir = glm::normalize(lookDir);

                glm::mat4 V = glm::lookAt(camPos, camPos + lookDir, glm::vec3(0.0f, 1.0f, 0.0f));
                float aspect = (passData.height > 0) ? (float)passData.width / (float)passData.height : 1.0f;
                glm::mat4 P = glm::perspective(glm::radians(75.0f), aspect, 0.1f, 250.0f);

                if (pass == 0)
                {
                    // Pass 0 draws the cubemap skybox around the moving camera.
                    shader.setUniform("u_view", V);
                    shader.setUniform("u_proj", P);
                    shader.setUniform("skybox", 0);
                    shader.setUniform("u_skyExposure", 1.8f);
                }
                else
                {
                    // Pass 1 rotates the portal so it always faces the camera in 3D.
                    glm::vec3 portalCenter = glm::vec3(pWindow->context.vec4param[2]);
                    glm::vec3 portalNormal = camPos - portalCenter;
                    if (glm::dot(portalNormal, portalNormal) < 0.0001f)
                        portalNormal = glm::vec3(0.0f, 0.0f, 1.0f);
                    portalNormal = glm::normalize(portalNormal);

                    glm::vec3 upRef = (std::abs(glm::dot(portalNormal, glm::vec3(0.0f, 1.0f, 0.0f))) > 0.98f)
                        ? glm::vec3(0.0f, 0.0f, 1.0f)
                        : glm::vec3(0.0f, 1.0f, 0.0f);
                    glm::vec3 portalRight = glm::normalize(glm::cross(upRef, portalNormal));
                    glm::vec3 portalUp = glm::normalize(glm::cross(portalNormal, portalRight));

                    // Build an orthonormal basis for the portal quad from right/up/forward vectors.
                    glm::mat4 portalRotation(1.0f);
                    portalRotation[0] = glm::vec4(portalRight, 0.0f);
                    portalRotation[1] = glm::vec4(portalUp, 0.0f);
                    portalRotation[2] = glm::vec4(portalNormal, 0.0f);

                    glm::mat4 portalModel =
                        glm::translate(glm::mat4(1.0f), portalCenter) *
                        portalRotation *
                        glm::scale(glm::mat4(1.0f), glm::vec3(3.6f, 3.6f, 1.0f));

                    // The projector matrix turns world positions on the quad into projected portal UVs.
                    glm::vec3 projectorPos = portalCenter + portalNormal * 3.0f;
                    glm::mat4 projectorV = glm::lookAt(projectorPos, portalCenter, portalUp);
                    glm::mat4 projectorP = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 16.0f);
                    glm::mat4 bias = glm::scale(
                        glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)),
                        glm::vec3(0.5f, 0.5f, 0.5f));

                    passData.objects[0].instances[0].modeltransform = portalModel;

                    shader.setUniform("M", portalModel);
                    shader.setUniform("u_view", V);
                    shader.setUniform("u_proj", P);
                    shader.setUniform("u_projectorMatrix", bias * projectorP * projectorV);
                    shader.setUniform("iTime", (float)glfwGetTime());
                }

                pWindow->needUpdate = true;
            },

            // SizeCallback
            [](cg::ManagedWindow* pWindow, int, int)
            {
                if (pWindow) pWindow->needUpdate = true;
            },

            // KeyCallback
            [](cg::ManagedWindow*, int)
            {
                return true;
            },

            // MousebuttonCallback
            [](cg::ManagedWindow* pWindow, int button, int xpos, int ypos)
            {
                if (!pWindow || button != GLFW_MOUSE_BUTTON_LEFT) return;

                // The framework triggers this on release; compare press/release positions to reject drags.
                const double deltaX = (double)xpos - pWindow->context.input.pressx;
                const double deltaY = (double)ypos - pWindow->context.input.pressy;
                const double distanceSq = deltaX * deltaX + deltaY * deltaY;
                const double clickThreshold = 5.0;

                // Reset the drag anchor so the next press starts a fresh camera rotation.
                pWindow->context.vec2param[2] = glm::vec2(0.0f);
                pWindow->context.vec2param[3] = glm::vec2(pWindow->context.floatparam[0], pWindow->context.floatparam[1]);

                if (distanceSq >= clickThreshold * clickThreshold)
                {
                    // Dragging should only rotate the camera
                    pWindow->needUpdate = true;
                    return;
                }

                // Reconstruct the camera matrices and cast a ray through the clicked screen position.
                auto& passData = pWindow->passes[1];
                float aspect = (passData.height > 0) ? (float)passData.width / (float)passData.height : 1.0f;
                glm::mat4 P = glm::perspective(glm::radians(75.0f), aspect, 0.1f, 250.0f);

                glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[0]);
                float yaw = pWindow->context.floatparam[0];
                float pitch = glm::clamp(pWindow->context.floatparam[1], -1.48f, 1.48f);
                glm::vec3 lookDir(
                    std::sin(yaw) * std::cos(pitch),
                    std::sin(pitch),
                    -std::cos(yaw) * std::cos(pitch)
                );
                glm::mat4 V = glm::lookAt(camPos, camPos + glm::normalize(lookDir), glm::vec3(0.0f, 1.0f, 0.0f));

                float ndcX = (2.0f * (float)xpos) / (float)pWindow->width - 1.0f;
                float ndcY = 1.0f - (2.0f * (float)ypos) / (float)pWindow->height;

                glm::mat4 invVP = glm::inverse(P * V);
                glm::vec4 nearPoint = invVP * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
                glm::vec4 farPoint = invVP * glm::vec4(ndcX, ndcY, 1.0f, 1.0f);
                nearPoint /= nearPoint.w;
                farPoint /= farPoint.w;

                glm::vec3 rayDir = glm::normalize(glm::vec3(farPoint - nearPoint));

                // Keep portal placement at a fixed depth
                const float portalDistance = 8.0f;
                glm::vec3 hit = camPos + rayDir * portalDistance;
                pWindow->context.vec4param[2] = glm::vec4(hit, 1.0f);

                pWindow->needUpdate = true;
            },

            // ScrollCallback
            [](cg::ManagedWindow* pWindow, double, double yoffset, int)
            {
                if (!pWindow) return;

                // Scroll moves the camera along a flat plane
                glm::vec3 camPos = glm::vec3(pWindow->context.vec4param[0]);
                float yaw = pWindow->context.floatparam[0];
                float pitch = glm::clamp(pWindow->context.floatparam[1], -1.48f, 1.48f);
                glm::vec3 forward = glm::normalize(glm::vec3(
                    std::sin(yaw) * std::cos(pitch),
                    0.0f,
                    -std::cos(yaw) * std::cos(pitch)
                ));

                camPos += forward * ((float)yoffset * 0.8f);
                camPos.y = 1.6f;
                pWindow->context.vec4param[0] = glm::vec4(camPos, 0.0f);
                pWindow->needUpdate = true;
            },

            // CursorposCallback
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double, double, int)
            {
                if (!pWindow || !pWindow->context.input.lbutton_down) return;

                glm::vec2 anchor = pWindow->context.vec2param[2];
                if (anchor.x == 0.0f && anchor.y == 0.0f)
                {
                    // Store the press position and the starting yaw/pitch once per drag gesture.
                    anchor = glm::vec2((float)pWindow->context.input.pressx, (float)pWindow->context.input.pressy);
                    pWindow->context.vec2param[2] = anchor;
                    pWindow->context.vec2param[3] = glm::vec2(pWindow->context.floatparam[0], pWindow->context.floatparam[1]);
                }

                glm::vec2 start = pWindow->context.vec2param[3];
                float dx = (float)xpos - anchor.x;
                float dy = (float)ypos - anchor.y;

                // Convert mouse deltas into yaw/pitch updates for the walking camera.
                const float sens = 0.005f;
                pWindow->context.floatparam[0] = start.x - dx * sens;
                pWindow->context.floatparam[1] = glm::clamp(start.y - dy * sens, -1.48f, 1.48f);
                pWindow->needUpdate = true;
            },

            nullptr,
            openChildWindow
            });

        // =========================
        // Child 8: Assignment 11 (Deferred Ogres)
        // =========================
        windowManager.addWindow({
            cg::State::SHOW,
            pMainWindow,
            { 8, 9, 10 },
            "Assignment 11 (Ogres)",
            { 0.02f, 0.02f, 0.03f, 1.0f },
            {
                { { "Ogre/ogre_diffuse.png" }, GL_TEXTURE0 },
                { { "Ogre/ogre_normalmap.png" }, GL_TEXTURE1 }
            },
            NONE,
            {
                []()
                {
                    cg::Pass pass(
                        { NOMARGIN },
                        { 0.0f, 0.0f, 0.0f, 0.0f },
                        ENABLE,
                        {},
                        NONE,
                        {
                            {
                                { 8.0f, 7.0f, 8.0f },
                                {
                                    { 0.10f, 0.10f, 0.12f },
                                    { 2.60f, 2.40f, 2.20f },
                                    { 2.60f, 2.40f, 2.20f }
                                }
                            }
                        },
                        {
                            cg::FBO::Type::Deferred,
                            GL_TEXTURE2,
                            nullptr,
                            GL_TEXTURE3,
                            nullptr,
                            GL_TEXTURE4,
                            nullptr
                        },
                        {
                            #include "Shaders/A11GBuffer.vert.glsl"
                            ,
                            #include "Shaders/A11GBuffer.frag.glsl"
                        }
                    );

                    cg::Object ogres(
                        "Ogre/bs_ears.obj",
                        cg::Material{
                            { "material.Ka", { 0.15f, 0.15f, 0.18f } },
                            { "material.Kd", { 1.0f, 1.0f, 1.0f } },
                            { "material.Ks", { 0.40f, 0.40f, 0.40f } },
                            { "material.shininess", { 48.0f } }
                        },
                        glm::mat4(1.0f),
                        INVISIBLE
                    );

                    ogres.instances.clear();

                    const int rows = 5;
                    const int cols = 5;
                    const float spacing = 2.85f;
                    const float scale = 1.5f;
                    const float baseY = 0.66f * scale;

                    for (int row = 0; row < rows; ++row)
                    {
                        for (int col = 0; col < cols; ++col)
                        {
                            const float x = (col - (cols - 1) * 0.5f) * spacing;
                            const float z = (row - (rows - 1) * 0.5f) * spacing;
                            const float yaw = 3.14159265f;

                            const glm::mat4 model =
                                glm::translate(glm::mat4(1.0f), glm::vec3(x, baseY, z)) *
                                glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
                                glm::scale(glm::mat4(1.0f), glm::vec3(scale));

                            ogres.instances.emplace_back(
                                cg::Material{
                                    { "material.Ka", { 0.15f, 0.15f, 0.18f } },
                                    { "material.Kd", { 1.0f, 1.0f, 1.0f } },
                                    { "material.Ks", { 0.40f, 0.40f, 0.40f } },
                                    { "material.shininess", { 48.0f } }
                                },
                                model,
                                VISIBLE
                            );
                        }
                    }

                    pass.objects.push_back(std::move(ogres));
                    return pass;
                }(),
                cg::Pass(
                    { NOMARGIN },
                    { 0.02f, 0.02f, 0.03f, 1.0f },
                    DISABLE,
                    { { QUAD } },
                    NONE,
                    NONE,
                    DEFAULT,
                    {
                        #include "Shaders/Shader.vert.glsl"
                        ,
                        #include "Shaders/A11Deferred.frag.glsl"
                    }
                )
            },

            // FocusCallback
            [](cg::ManagedWindow* pWindow, int focused)
            {
                if (focused && pWindow)
                {
                    std::cout
                        << "\nAssignment 11 (Ogres)\n"
                        << "   This demo showcases both normal mapping and deferred shading techniques.\n"
                        << "   The ogre model is instanced in a centered grid, the g-buffer stores\n"
                        << "   per-pixel scene data, and the final pass applies normal-mapped\n"
                        << "   Blinn-Phong shading.\n"
                        << "Controls:\n"
                        << "   - Orbit Camera: Drag with the left mouse button; scroll to zoom.\n"
                        << "   - Orbit Light: Hold Shift and drag; hold Shift and scroll to zoom.\n"
                        << "   - Rendering Options: 1 = Blinn-Phong, 2 = light directions, 3 = view direction.\n";
                }
            },

            // InitCallback
            [](cg::ManagedWindow* pWindow, int pass)
            {
                if (!pWindow || pass < 0 || pass >= (int)pWindow->passes.size()) return;

                if (pWindow->context.intparam[4] == 0)
                {
                    // Initialize camera/light so both start from the same front-facing orbit.
                    const float frontViewYaw = 3.14159265f;
                    const float startPitch = 0.28f;
                    const float startDistance = 22.0f;

                    pWindow->context.intparam[4] = 1;
                    pWindow->context.intparam[0] = 1;
                    pWindow->context.floatparam[0] = frontViewYaw; // camera yaw
                    pWindow->context.floatparam[1] = startPitch;   // camera pitch
                    pWindow->context.floatparam[2] = startDistance;// camera distance
                    pWindow->context.floatparam[3] = frontViewYaw; // light yaw
                    pWindow->context.floatparam[4] = startPitch;   // light pitch
                    pWindow->context.vec2param[4].x = startDistance;// light distance
                }

                auto& passData = pWindow->passes[pass];
                auto& shader = passData.renderShader;
                shader.use();

                const glm::vec3 sceneCenter(0.0f);
                const float orbitLimit = 1.35f;
                // Camera and light both use the same spherical orbit around the world origin.
                auto orbitPosition = [](float yaw, float pitch, float distance)
                {
                    return glm::vec3(
                        distance * std::cos(pitch) * std::sin(yaw),
                        distance * std::sin(pitch),
                        distance * std::cos(pitch) * std::cos(yaw));
                };
                // Read the g-buffer attachments produced by the deferred FBO.
                auto getPassColorTexture = [](const cg::Pass& pass, GLenum attachment)
                {
                    if (pass.fbo.handle == 0)
                        return GLuint{ 0 };

                    GLint prevFbo = 0;
                    GLint objectType = GL_NONE;
                    GLint textureId = 0;

                    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFbo);
                    glBindFramebuffer(GL_FRAMEBUFFER, pass.fbo.handle);
                    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objectType);

                    if (objectType == GL_TEXTURE)
                        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &textureId);

                    glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(prevFbo));
                    return static_cast<GLuint>(textureId);
                };

                float yaw = pWindow->context.floatparam[0];
                float pitch = glm::clamp(pWindow->context.floatparam[1], -orbitLimit, orbitLimit);
                float distance = glm::clamp(pWindow->context.floatparam[2], 6.0f, 36.0f);

                pWindow->context.floatparam[1] = pitch;
                pWindow->context.floatparam[2] = distance;
                if (!pWindow->passes.empty() && !pWindow->passes[0].lights.empty())
                {
                    pWindow->context.floatparam[4] = glm::clamp(pWindow->context.floatparam[4], -orbitLimit, orbitLimit);
                    pWindow->context.vec2param[4].x = glm::clamp(pWindow->context.vec2param[4].x, 4.0f, 30.0f);

                    pWindow->passes[0].lights[0].position =
                        orbitPosition(
                            pWindow->context.floatparam[3],
                            pWindow->context.floatparam[4],
                            pWindow->context.vec2param[4].x
                        ) + sceneCenter;
                }

                glm::vec3 camPos = orbitPosition(yaw, pitch, distance) + sceneCenter;
                float aspect = (passData.height > 0)
                    ? static_cast<float>(passData.width) / static_cast<float>(passData.height)
                    : 1.0f;

                glm::mat4 V = glm::lookAt(camPos, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 P = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
                const auto& light = pWindow->passes[0].lights[0];
                const glm::vec3 lightPosView = glm::vec3(V * glm::vec4(light.position, 1.0f));

                if (pass == 0)
                {
                    // Geometry pass: render the ogre instances into the g-buffer.
                    shader.setUniform("u_view", V);
                    shader.setUniform("u_proj", P);
                    shader.setUniform("u_lightPosView", lightPosView);
                }
                else
                {
                    // Final pass: sample the g-buffer and apply normal-mapped deferred lighting.
                    const GLuint lightDirTex = getPassColorTexture(pWindow->passes[0], GL_COLOR_ATTACHMENT0);
                    const GLuint viewDirTex = getPassColorTexture(pWindow->passes[0], GL_COLOR_ATTACHMENT1);
                    const GLuint surfaceTex = getPassColorTexture(pWindow->passes[0], GL_COLOR_ATTACHMENT2);

                    if (lightDirTex != 0)
                    {
                        glActiveTexture(GL_TEXTURE2);
                        glBindTexture(GL_TEXTURE_2D, lightDirTex);
                    }
                    if (viewDirTex != 0)
                    {
                        glActiveTexture(GL_TEXTURE3);
                        glBindTexture(GL_TEXTURE_2D, viewDirTex);
                    }
                    if (surfaceTex != 0)
                    {
                        glActiveTexture(GL_TEXTURE4);
                        glBindTexture(GL_TEXTURE_2D, surfaceTex);
                    }

                    // These texture units match the bindings declared in A11Deferred.frag.glsl.
                    shader.setUniform("ColorTex", 0);
                    shader.setUniform("NormalMapTex", 1);
                    shader.setUniform("GLightDirTex", 2);
                    shader.setUniform("GViewDirTex", 3);
                    shader.setUniform("GSurfaceTex", 4);
                    shader.setUniform("u_lightAmbient", light.L.size() > 0 ? light.L[0] : glm::vec3(0.1f));
                    shader.setUniform("u_lightDiffuse", light.L.size() > 1 ? light.L[1] : glm::vec3(1.0f));
                    shader.setUniform("u_lightSpecular", light.L.size() > 2 ? light.L[2] : glm::vec3(1.0f));
                    shader.setUniform("u_shininess", 48.0f);
                    shader.setUniform("u_renderMode", pWindow->context.intparam[0]);
                }

                pWindow->needUpdate = true;
            },

            // SizeCallback
            [](cg::ManagedWindow* pWindow, int width, int height)
            {
                if (pWindow) pWindow->needUpdate = true;
            },

            // KeyCallback
            [](cg::ManagedWindow* pWindow, int key)
            {
                if (!pWindow) return true;

                if (key == GLFW_KEY_1)
                {
                    pWindow->context.intparam[0] = 1;
                    pWindow->needUpdate = true;
                }
                else if (key == GLFW_KEY_2)
                {
                    pWindow->context.intparam[0] = 2;
                    pWindow->needUpdate = true;
                }
                else if (key == GLFW_KEY_3)
                {
                    pWindow->context.intparam[0] = 3;
                    pWindow->needUpdate = true;
                }
                return true;
            },

            // MousebuttonCallback
            [](cg::ManagedWindow* pWindow, int button, int xpos, int ypos)
            {
                if (!pWindow || button != GLFW_MOUSE_BUTTON_LEFT) return;

                pWindow->context.vec2param[2] = glm::vec2(0.0f);
                pWindow->context.vec4param[0] = glm::vec4(
                    pWindow->context.floatparam[0],
                    pWindow->context.floatparam[1],
                    pWindow->context.floatparam[3],
                    pWindow->context.floatparam[4]
                );
                pWindow->needUpdate = true;
            },

            // ScrollCallback
            [](cg::ManagedWindow* pWindow, double xoffset, double yoffset, int mods)
            {
                if (!pWindow) return;

                if (mods & GLFW_MOD_SHIFT)
                {
                    pWindow->context.vec2param[4].x -= static_cast<float>(yoffset) * 0.8f;
                    pWindow->context.vec2param[4].x = glm::clamp(pWindow->context.vec2param[4].x, 4.0f, 30.0f);
                    pWindow->needUpdate = true;
                }
                else
                {
                    pWindow->context.floatparam[2] -= static_cast<float>(yoffset) * 0.8f;
                    if (pWindow->context.floatparam[2] < 6.0f)  pWindow->context.floatparam[2] = 2.0f;
                    if (pWindow->context.floatparam[2] > 36.0f) pWindow->context.floatparam[2] = 36.0f;
                    pWindow->needUpdate = true;
                }
            },

            // CursorposCallback
            [](cg::ManagedWindow* pWindow, int xpos, int ypos, double xoffset, double yoffset, int mods)
            {
                if (!pWindow || !pWindow->context.input.lbutton_down) return;
                const float orbitLimit = 1.35f;

                const float mx = static_cast<float>(xpos);
                const float my = static_cast<float>(ypos);

                glm::vec2 anchor = pWindow->context.vec2param[2];
                if (anchor.x == 0.0f && anchor.y == 0.0f)
                {
                    anchor = glm::vec2(
                        static_cast<float>(pWindow->context.input.pressx),
                        static_cast<float>(pWindow->context.input.pressy)
                    );
                    pWindow->context.vec2param[2] = anchor;
                    pWindow->context.vec4param[0] = glm::vec4(
                        pWindow->context.floatparam[0],
                        pWindow->context.floatparam[1],
                        pWindow->context.floatparam[3],
                        pWindow->context.floatparam[4]
                    );
                }

                if (mods & GLFW_MOD_SHIFT)
                {
                    const float dx = mx - anchor.x;
                    const float dy = my - anchor.y;
                    const glm::vec4 start = pWindow->context.vec4param[0];

                    pWindow->context.floatparam[3] = start.z - dx * 0.008f;
                    pWindow->context.floatparam[4] = glm::clamp(start.w - dy * 0.008f, -orbitLimit, orbitLimit);
                    pWindow->needUpdate = true;
                }
                else
                {
                    const glm::vec4 start = pWindow->context.vec4param[0];
                    const float dx = mx - anchor.x;
                    const float dy = my - anchor.y;

                    pWindow->context.floatparam[0] = start.x - dx * 0.008f;
                    pWindow->context.floatparam[1] = glm::clamp(start.y - dy * 0.008f, -orbitLimit, orbitLimit);
                    pWindow->needUpdate = true;
                }
            },

            // MouseclickCallback
            [](cg::ManagedWindow* pWindow, int xpos, int ypos)
            {
            },

            // MousedoubleclickCallback
            openChildWindow
        });


        // Show everything
        windowManager.show();
        if (pMainWindow) glfwFocusWindow(pMainWindow);
        windowManager.render();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        retcode = -1;
    }

    windowManager.cleanup();
    glfwTerminate();
    return retcode;
}
