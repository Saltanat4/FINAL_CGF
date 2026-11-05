#include <cstdint>

#include "Window.h"
#include "Camera.h"

#include "Scene.h"
#include "ObjectFactory.h"
#include "Renderer.h"

#include <glad/glad.h>
#include <iostream>
#include <exception>

#include "PhysicsWorld.h"
#include "CharacterController.h"

#include "GameContext.h"
#include "InteractionSystem.h"
namespace
{
    constexpr float kSpawnYaw = -90.0f;
    constexpr float kSpawnPitch = -10.0f;

    GameContext gameContext;

    struct LevelState
    {
        GameObject* platform = nullptr;
    };

    const glm::vec3 kPlayerSpawn(0.0f, 1.5f, 5.0f);

    constexpr float kKillHeight = -10.0f;

    LevelState BuildLevel(
        Scene& scene,
        const ObjectFactory& factory,
        GameContext& context)
    {
        LevelState state;

        factory.CreateBlock(
            scene,
            "Floor",
            glm::vec3(0.0f, -0.75f, 0.0f),
            glm::vec3(16.0f, 0.5f, 16.0f)
        );

        factory.CreateBlock(
            scene,
            "Кубик",
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        factory.CreateBlock(
            scene,
            "Столб",
            glm::vec3(-3.0f, 1.0f, -3.0f),
            glm::vec3(1.0f, 3.0f, 1.0f)
        );

        factory.CreateBlock(
            scene,
            "Столб",
            glm::vec3(-2.0f, 0.5f, -1.0f),
            glm::vec3(1.0f, 2.0f, 1.0f)
        );

        GameObject& platform = factory.CreateBlock(
            scene,
            "Платформа",
            glm::vec3(3.0f, 1.5f, -3.0f),
            glm::vec3(4.0f, 0.4f, 2.0f),
            glm::vec3(0.0f, 35.0f, 15.0f)
        );

        platform.collider->isStatic = false;

        factory.CreateSpikes(
            scene,
            "Шипы",
            glm::vec3(4.0f, -0.35f, 2.0f),
            glm::vec3(2.0f, 0.3f, 2.0f)
        );


        factory.CreateKey(
            scene,
            context,
            "Ключ 1",
            glm::vec3(-5.0f, -0.3f, 2.0f)
        );

        factory.CreateKey(
            scene,
            context,
            "Ключ 2",
            glm::vec3(-5.0f, -0.3f, 4.5f)
        );

        state.platform = &platform;

        return state;
    }

    struct MouseState
    {
        Camera* camera = nullptr;

        double lastX = 0.0;
        double lastY = 0.0;

        bool firstEvent = true;
    };

    void MouseCallback(GLFWwindow* window, double x, double y)
    {
        auto* mouse = static_cast<MouseState*>(
            glfwGetWindowUserPointer(window));

        if (!mouse || !mouse->camera)
        {
            return;
        }

        if (mouse->firstEvent)
        {
            mouse->lastX = x;
            mouse->lastY = y;
            mouse->firstEvent = false;

            return;
        }

        float deltaX = static_cast<float>(x - mouse->lastX);
        float deltaY = static_cast<float>(mouse->lastY - y);

        mouse->lastX = x;
        mouse->lastY = y;

        mouse->camera->Rotate(deltaX, deltaY);
    }

    void FocusCallback(GLFWwindow* window, int)
    {
        auto* mouse = static_cast<MouseState*>(
            glfwGetWindowUserPointer(window));

        if (mouse)
        {
            mouse->firstEvent = true;
        }
    }

    glm::vec3 ReadMoveDirection(
        GLFWwindow* window,
        const Camera& camera)
    {
        if (!glfwGetWindowAttrib(window, GLFW_FOCUSED))
        {
            return glm::vec3(0.0f);
        }

        glm::vec3 forward = camera.GetForward();

        forward.y = 0.0f;
        forward = glm::normalize(forward);

        glm::vec3 right = glm::normalize(
            glm::cross(
                forward,
                glm::vec3(0.0f, 1.0f, 0.0f)
            )
        );

        glm::vec3 direction(0.0f);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            direction += forward;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            direction -= forward;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            direction += right;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            direction -= right;

        return direction;
    }
}

int main()
{
    try
    {
        Window window(1280, 720, "Platformer");

        Camera camera;

        camera.SetRotation(kSpawnYaw, kSpawnPitch);

        Scene scene;

        ObjectFactory objectFactory;

        Renderer renderer;

        MouseState mouse;
        mouse.camera = &camera;

        glfwSetWindowUserPointer(window.GetHandle(), &mouse);

        glfwSetCursorPosCallback(
            window.GetHandle(),
            MouseCallback);

        glfwSetWindowFocusCallback(
            window.GetHandle(),
            FocusCallback);

        glfwSetInputMode(
            window.GetHandle(),
            GLFW_CURSOR,
            GLFW_CURSOR_DISABLED);

        glEnable(GL_DEPTH_TEST);

        LevelState level = BuildLevel(scene, objectFactory, gameContext);

        PhysicsWorld physicsWorld;

        physicsWorld.RebuildColliders(scene);

        CharacterController player(kPlayerSpawn);

        camera.SetPosition(player.GetEyePosition());

        constexpr double fixedDeltaTime = 1.0 / 60.0;

        double physicsAccumulator = 0.0;

        bool jumpWasDown = false;
        bool jumpPending = false;

        auto restartLevel = [&]()
            {
                gameContext.ResetLevelProgress();

                scene.Clear();

                level = BuildLevel(scene, objectFactory, gameContext);

                physicsWorld.RebuildColliders(scene);
                physicsWorld.ResetTriggerState();

                player.Teleport(kPlayerSpawn);

                camera.SetPosition(player.GetEyePosition());
                camera.SetRotation(kSpawnYaw, kSpawnPitch);

                jumpPending = false;
                physicsAccumulator = 0.0;
            };

        double previousTime = glfwGetTime();

        while (!window.ShouldClose())
        {
            window.PollEvents();

            double currentTime = glfwGetTime();

            float deltaTime = static_cast<float>(
                currentTime - previousTime);

            previousTime = currentTime;

            if (deltaTime > 0.05f)
            {
                deltaTime = 0.05f;
            }

            GLFWwindow* handle = window.GetHandle();

            if (glfwGetKey(handle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                window.Close();
            }

            glm::vec3 moveDirection = ReadMoveDirection(
                handle,
                camera
            );

            bool focused =
                glfwGetWindowAttrib(handle, GLFW_FOCUSED) != 0;

            bool jumpDown =
                focused &&
                glfwGetKey(handle, GLFW_KEY_SPACE) == GLFW_PRESS;

            if (jumpDown && !jumpWasDown)
            {
                jumpPending = true;
            }

            jumpWasDown = jumpDown;

            if (!focused)
            {
                jumpPending = false;
            }

            physicsAccumulator += deltaTime;


            while (physicsAccumulator >= fixedDeltaTime)
            {
                physicsWorld.UpdateDynamicColliders();

                player.FixedUpdate(
                    physicsWorld,
                    moveDirection,
                    jumpPending,
                    static_cast<float>(fixedDeltaTime)
                );

                DispatchTriggerEvents(
                    scene,
                    physicsWorld.UpdateTriggers(player.GetCapsule()),
                    gameContext
                );

                if (player.GetCapsule().center.y < kKillHeight)
                {
                    gameContext.RequestRestart();
                }

                jumpPending = false;

                physicsAccumulator -= fixedDeltaTime;

                if (gameContext.RestartRequested())
                {
                    break;
                }
            }

            if (gameContext.ConsumeRestart())
            {
                restartLevel();
            }
            else if (!gameContext.GetDestroyRequests().empty())
            {
                for (std::uint32_t id : gameContext.GetDestroyRequests())
                {
                    scene.RemoveById(id);
                }

                gameContext.ClearDestroyRequests();

                physicsWorld.RebuildColliders(scene);

                std::cout << "Keys: "
                    << gameContext.GetKeysCollected()
                    << " / "
                    << gameContext.GetKeysInLevel()
                    << '\n';
            }

            camera.SetPosition(player.GetEyePosition());

            int width = 0;
            int height = 0;

            glfwGetFramebufferSize(
                window.GetHandle(),
                &width,
                &height);

            if (width == 0 || height == 0)
            {
                glfwWaitEventsTimeout(0.05);
                continue;
            }

            glClearColor(0.08f, 0.10f, 0.14f, 1.0f);

            glClear(
                GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT);

            float aspectRatio =
                static_cast<float>(width) /
                static_cast<float>(height);

            renderer.Draw(scene, camera, aspectRatio);

            if (level.platform)
            {
                level.platform->transform.rotationDegrees.y +=
                    30.0f * deltaTime;
            }

            window.SwapBuffers();
        }

        glfwSetCursorPosCallback(window.GetHandle(), nullptr);
        glfwSetWindowFocusCallback(window.GetHandle(), nullptr);
        glfwSetWindowUserPointer(window.GetHandle(), nullptr);
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}