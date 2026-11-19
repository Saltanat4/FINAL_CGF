#include "LevelLoader.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <unordered_map>

#include "Mover.h"
#include "Rotator.h"

using json = nlohmann::json;

namespace
{
    [[noreturn]] void Fail(const std::string& ctx, const std::string& message)
    {
        throw std::runtime_error(ctx + ": " + message);
    }

    void RejectUnknownKeys(
        const json& j,
        std::initializer_list<const char*> allowed,
        const std::string& ctx)
    {
        for (auto it = j.begin(); it != j.end(); ++it)
        {
            bool known = false;

            for (const char* key : allowed)
            {
                if (it.key() == key) { known = true; break; }
            }

            if (!known)
            {
                Fail(ctx, "unknown key \"" + it.key() + "\"");
            }
        }
    }

    float ReadFloat(
        const json& j, const char* key, float fallback,
        const std::string& ctx)
    {
        auto it = j.find(key);

        if (it == j.end()) return fallback;

        if (!it->is_number())
        {
            Fail(ctx, std::string("\"") + key + "\" must be a number");
        }

        return it->get<float>();
    }

    std::string ReadString(
        const json& j, const char* key, const std::string& fallback,
        const std::string& ctx)
    {
        auto it = j.find(key);

        if (it == j.end()) return fallback;

        if (!it->is_string())
        {
            Fail(ctx, std::string("\"") + key + "\" must be a string");
        }

        return it->get<std::string>();
    }

    glm::vec3 ReadVec3(
        const json& j, const char* key, const glm::vec3& fallback,
        const std::string& ctx)
    {
        auto it = j.find(key);

        if (it == j.end()) return fallback;

        bool ok = it->is_array() && it->size() == 3;

        if (ok)
        {
            for (const json& n : *it)
            {
                if (!n.is_number()) ok = false;
            }
        }

        if (!ok)
        {
            Fail(ctx, std::string("\"") + key + "\" must be [x, y, z]");
        }

        return glm::vec3(
            (*it)[0].get<float>(),
            (*it)[1].get<float>(),
            (*it)[2].get<float>()
        );
    }


    using BehaviorBuilder = std::function<void(
        GameObject&, const json&, const std::string&)>;

    const std::unordered_map<std::string, BehaviorBuilder>& Behaviors()
    {
        static const std::unordered_map<std::string, BehaviorBuilder> map =
        {
            {
                "Rotator",
                [](GameObject& object, const json& j, const std::string& ctx)
                {
                    RejectUnknownKeys(j, { "type", "speed" }, ctx);

                    float speed = ReadFloat(j, "speed", 90.0f, ctx);

                    object.AddBehavior<Rotator>(speed);
                }
            },
            {
                "Mover",
                [](GameObject& object, const json& j, const std::string& ctx)
                {
                    RejectUnknownKeys(
                        j, { "type", "offset", "speed", "wait" }, ctx);

                    glm::vec3 offset =
                        ReadVec3(j, "offset", glm::vec3(0.0f, 1.0f, 0.0f), ctx);

                    float speed = ReadFloat(j, "speed", 1.0f, ctx);
                    float wait = ReadFloat(j, "wait", 0.0f, ctx);

                    if (speed <= 0.0f) Fail(ctx, "\"speed\" must be > 0");
                    if (wait < 0.0f) Fail(ctx, "\"wait\" must be >= 0");

                    object.AddBehavior<Mover>(offset, speed, wait);
                }
            }
        };

        return map;
    }

    void AddBehaviors(
        GameObject& object, const json& list, const std::string& ctx)
    {
        if (!list.is_array())
        {
            Fail(ctx, "\"behaviors\" must be an array");
        }

        int index = 0;

        for (const json& entry : list)
        {
            const std::string bctx =
                ctx + " behaviors[" + std::to_string(index++) + "]";

            if (!entry.is_object())
            {
                Fail(bctx, "must be an object");
            }

            std::string type = ReadString(entry, "type", "", bctx);

            auto it = Behaviors().find(type);

            if (it == Behaviors().end())
            {
                Fail(bctx, "unknown behavior \"" + type + "\"");
            }

            it->second(object, entry, bctx);
        }
    }


    void LoadObject(
        const json& entry,
        int index,
        Scene& scene,
        const ObjectFactory& factory,
        GameContext& context)
    {
        std::string ctx = "objects[" + std::to_string(index) + "]";

        if (!entry.is_object())
        {
            Fail(ctx, "must be an object");
        }

        RejectUnknownKeys(
            entry,
            { "type", "name", "position", "size", "rotation",
              "model", "behaviors" },
            ctx
        );

        const std::string type = ReadString(entry, "type", "", ctx);
        const std::string name = ReadString(entry, "name", type, ctx);

        ctx += " \"" + name + "\"";

        const glm::vec3 position =
            ReadVec3(entry, "position", glm::vec3(0.0f), ctx);

        const glm::vec3 rotation =
            ReadVec3(entry, "rotation", glm::vec3(0.0f), ctx);

        auto sizeOf = [&](const glm::vec3& fallback)
            {
                return ReadVec3(entry, "size", fallback, ctx);
            };

        GameObject* object = nullptr;

        try
        {
            if (type == "block")
            {
                object = &factory.CreateBlock(
                    scene, name, position,
                    sizeOf(glm::vec3(1.0f)), rotation);
            }
            else if (type == "spikes")
            {
                object = &factory.CreateSpikes(
                    scene, name, position,
                    sizeOf(glm::vec3(1.0f)), rotation,
                    ReadString(entry, "model", ObjectFactory::kSpikeModel, ctx));
            }
            else if (type == "key")
            {
                object = &factory.CreateKey(
                    scene, context, name, position,
                    sizeOf(glm::vec3(0.3f, 0.6f, 0.1f)),
                    ReadString(entry, "model", ObjectFactory::kKeyModel, ctx));

                object->transform.rotationDegrees = rotation;
            }
            else if (type == "door")
            {
                object = &factory.CreateDoor(
                    scene, name, position,
                    sizeOf(glm::vec3(1.5f, 2.5f, 0.3f)), rotation,
                    ReadString(entry, "model", ObjectFactory::kDoorModel, ctx));
            }
            else
            {
                Fail(ctx, "unknown type \"" + type +
                    "\" (expected block, spikes, key, door)");
            }
        }
        catch (const std::invalid_argument& error)
        {
            Fail(ctx, error.what());
        }

        auto behaviors = entry.find("behaviors");

        if (behaviors != entry.end())
        {
            AddBehaviors(*object, *behaviors, ctx);

            if (!behaviors->empty() &&
                object->collider &&
                object->collider->type != ColliderType::Trigger)
            {
                object->collider->isStatic = false;
            }
        }
    }
}

LevelData LevelLoader::Load(
    const std::string& path,
    Scene& scene,
    const ObjectFactory& factory,
    GameContext& context)
{
    std::ifstream file(path);

    if (!file)
    {
        throw std::runtime_error("Cannot open level file: " + path);
    }

    json root;

    try
    {
        root = json::parse(file);
    }
    catch (const json::parse_error& error)
    {
        throw std::runtime_error(
            "Level JSON syntax error in " + path + ": " + error.what());
    }

    const std::string ctx = path;

    if (!root.is_object())
    {
        Fail(ctx, "root must be an object");
    }

    RejectUnknownKeys(
        root, { "name", "playerSpawn", "playerYaw", "objects" }, ctx);

    LevelData data;

    data.name = ReadString(root, "name", "", ctx);

    data.playerSpawn = 
        ReadVec3(root, "playerSpawn", glm::vec3(0.0f, 1.5f, -5.0f), ctx);

    data.playerYaw = ReadFloat(root, "playerYaw", -90.0f, ctx);

    auto objects = root.find("objects");

    if (objects == root.end() || !objects->is_array())
    {
        Fail(ctx, "\"objects\" array is required");
    }

    int index = 0;

    for (const json& entry : *objects)
    {
        LoadObject(entry, index++, scene, factory, context);
    }

    return data;
}

