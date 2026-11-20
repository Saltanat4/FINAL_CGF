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
#include "EchoSystem.h"
#include "Rotator.h"
#include "Mover.h"
#include "LevelLoader.h"
#include <filesystem>
#include "HUD.h"
#include "Audio.h"

namespace
{
    constexpr float kSpawnPitch = -10.0f;


    constexpr float kKillHeight = -10.0f;

    void SpawnEchoes(
        EchoSystem& echo,
        AudioSystem& audio,
        const CharacterController& player,
        const CharacterStepResult& step,
        float time)
    {
        const CapsuleCollider& capsule = player.GetCapsule();

        const glm::vec3 feet(
            capsule.center.x,
            capsule.center.y - capsule.height * 0.5f + 0.05f,
            capsule.center.z
        );

        if (step.jumped)
        {
            echo.Spawn(feet, time, 12.0f, 1.0f);
            audio.Play(SoundId::Echo);
        }

        if (step.landed)
        {
            float k = glm::clamp(step.landingSpeed / 12.0f, 0.3f, 1.0f);

            echo.Spawn(feet, time, 6.0f + 8.0f * k, 0.5f + 0.5f * k);
            audio.Play(SoundId::Echo);
        }

        if (step.footstep)
        {
            echo.Spawn(feet, time, 3.5f, 0.7f);
        }
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
        Window window(1280, 720, "Platformer", true);

        Camera camera;

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

        Hud hud(window.GetHandle());
        AudioSystem audio;
        int previousKeys = 0;

        PhysicsWorld physicsWorld;
        GameContext gameContext;
        EchoSystem echo;

        LevelData level = LevelLoader::Load(
            GameContext::LevelPath(gameContext.GetCurrentLevel()), 
            scene, objectFactory, gameContext);

        physicsWorld.RebuildColliders(scene);

        CharacterController player(level.playerSpawn);

        camera.SetPosition(player.GetEyePosition());
        camera.SetRotation(level.playerYaw, kSpawnPitch);

        constexpr double fixedDeltaTime = 1.0 / 60.0;

        double physicsAccumulator = 0.0;

        bool jumpWasDown = false;
        bool jumpPending = false;

        bool f11WasDown = false;

        auto restartLevel = [&]()
            {
                gameContext.ResetLevelProgress();

                scene.Clear();

                level = LevelLoader::Load(
                    GameContext::LevelPath(gameContext.GetCurrentLevel()),
                    scene, objectFactory, gameContext);

                physicsWorld.RebuildColliders(scene);
                physicsWorld.ResetTriggerState();

                player.Teleport(level.playerSpawn);

                camera.SetPosition(player.GetEyePosition());
                camera.SetRotation(level.playerYaw, kSpawnPitch);

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

            bool f11Down = glfwGetKey(handle, GLFW_KEY_F11) == GLFW_PRESS;

            if (f11Down && !f11WasDown)
            {
                window.ToggleFullscreen();
                mouse.firstEvent = true;
            }

            f11WasDown = f11Down;

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
                scene.UpdateBehaviors(
                gameContext,
                    static_cast<float>(fixedDeltaTime));

                physicsWorld.UpdateDynamicColliders();

                CharacterStepResult step = player.FixedUpdate(
                    physicsWorld,
                    moveDirection,
                    jumpPending,
                    static_cast<float>(fixedDeltaTime)
                );

                SpawnEchoes(
                    echo,
                    audio,
                    player,
                    step,
                    static_cast<float>(currentTime)
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


                if (gameContext.RestartRequested() ||
                    gameContext.NextLevelRequested())
                {
                    break;
                }   
            }



            bool restart = gameContext.ConsumeRestart();
            bool nextLevel = gameContext.ConsumeNextLevel();

            if (nextLevel)
            {
                int target = gameContext.GetCurrentLevel() + 1;

                if (std::filesystem::exists(GameContext::LevelPath(target)))
                {
                    gameContext.SetCurrentLevel(target);
                }
                else
                {
                    std::cout << "All levels completed!\n";
                    break;
                }
            }

            if (restart || nextLevel)
            {
                restartLevel();

                audio.Play(nextLevel ? SoundId::NextLevel : SoundId::Respawn);
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

            int keys = gameContext.GetKeysCollected();

            if (keys > previousKeys)
            {
                audio.Play(SoundId::Pickup);
            }

            previousKeys = keys;

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

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            glClear(
                GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT);

            float aspectRatio =
                static_cast<float>(width) /
                static_cast<float>(height);

            echo.Update(static_cast<float>(currentTime));

            renderer.Draw(
                scene,
                camera,
                aspectRatio,
                echo,
                static_cast<float>(currentTime)
            );

            hud.Draw(
                gameContext.GetKeysCollected(),
                gameContext.GetKeysInLevel(),
                height);

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