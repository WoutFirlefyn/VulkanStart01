#pragma once
#include <cassert>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

struct Camera
{
    Camera() = default;

    Camera(const glm::vec3& _origin, float _fovAngle) :
        origin{ _origin },
        fovAngle{ _fovAngle }
    {
    }

    glm::vec3 origin{};
    float fovAngle{ 90.f };
    float fov{};
    float aspectRatio{};

    glm::vec3 forward{ glm::vec3(0.f, 0.f, 1.f) };
    glm::vec3 up{ glm::vec3(0.f, -1.f, 0.f) };
    glm::vec3 right{ glm::vec3(1.f, 0.f, 0.f) };

    float totalPitch{};
    float totalYaw{};

    glm::mat4 viewMatrix{};
    glm::mat4 projectionMatrix{};

    float nearPlane{ 0.1f };
    float farPlane{ 1000.f };

    glm::vec2 dragStart{};

    void Initialize(float _fovAngle = 90.f, glm::vec3 _origin = glm::vec3(0.f), float _aspectRatio = 1.f)
    {
        fovAngle = _fovAngle;
        fov = glm::tan(glm::radians(fovAngle) / 2.f);

        origin = _origin;
        aspectRatio = _aspectRatio;
    }

    void CalculateViewMatrix()
    {
        glm::mat4x4 rotation = glm::rotate(glm::mat4x4(1.f), totalYaw, glm::vec3{ 0,1,0 });
        rotation = glm::rotate(rotation, totalPitch, glm::vec3{ 1,0,0 });
        
        forward = glm::normalize(rotation[2]);
        right = glm::normalize(glm::cross(up, forward));

        viewMatrix = glm::lookAt(origin, origin + forward, up);
    }

    void CalculateProjectionMatrix()
    {
        projectionMatrix = glm::perspective(glm::radians(fovAngle), aspectRatio, nearPlane, farPlane);
    }

    void Update()
    {
        CalculateViewMatrix();
        CalculateProjectionMatrix();
    }

    void KeyEvent(int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            origin += forward * 10.f;
        }
        if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            origin -= forward * 10.f;
        }
        if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            origin += right * 10.f;
        }
        if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            origin -= right * 10.f;
        }
    }

    void MouseMove(GLFWwindow* window, double xpos, double ypos)
    {
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
        if (state == GLFW_PRESS)
        {
            float dx = static_cast<float>(xpos) - dragStart.x;
            totalYaw += dx * 0.01f;
            float dy = static_cast<float>(ypos) - dragStart.y;
            totalPitch += dy * 0.01f;
            constexpr float pi = glm::pi<float>();
            totalYaw = fmodf(totalYaw, 2 * pi);
            totalPitch = std::clamp(totalPitch, -pi / 2 + 0.0001f, pi / 2 - 0.0001f);

            dragStart = glm::vec2(xpos, ypos);
        }
    }

    void MouseEvent(GLFWwindow* window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            dragStart.x = static_cast<float>(xpos);
            dragStart.y = static_cast<float>(ypos);
        }
    }
};
