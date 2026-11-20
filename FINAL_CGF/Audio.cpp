#include "Audio.h"

#include <miniaudio/miniaudio.h>

#include <cstddef>
#include <iostream>
#include <iterator>

namespace
{
    constexpr const char* kMusicPath = "sounds/Silent_Hill_2_Soundtrack.mp3";

    constexpr const char* kSfxPaths[] =
    {
        "sounds/coinpickup.mp3",
        "sounds/ReZero_Return_By_Death.mp3",
        "sounds/dooropen.mp3",
        "sounds/sonar.mp3",
    };

    static_assert(
        std::size(kSfxPaths) == static_cast<std::size_t>(SoundId::Count),
        "kSfxPaths must match SoundId");

    constexpr float kMusicVolume = 0.35f;
    constexpr float kSfxVolume = 0.8f;
}

struct AudioSystem::Impl
{
    ma_engine engine{};
    ma_sound_group sfxGroup{};
    ma_sound music{};

    bool engineReady = false;
    bool groupReady = false;
    bool musicReady = false;
};

AudioSystem::AudioSystem()
    : impl(std::make_unique<Impl>())
{
    if (ma_engine_init(nullptr, &impl->engine) != MA_SUCCESS)
    {
        std::cerr << "Audio disabled: cannot initialize audio engine\n";
        return;
    }

    impl->engineReady = true;

    if (ma_sound_group_init(
        &impl->engine, 0, nullptr, &impl->sfxGroup) == MA_SUCCESS)
    {
        impl->groupReady = true;
        ma_sound_group_set_volume(&impl->sfxGroup, kSfxVolume);
    }

    ma_resource_manager* resources =
        ma_engine_get_resource_manager(&impl->engine);

    for (const char* path : kSfxPaths)
    {
        if (ma_resource_manager_register_file(
            resources, path,
            MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE) != MA_SUCCESS)
        {
            std::cerr << "Sound not loaded: " << path << '\n';
        }
    }

    if (ma_sound_init_from_file(
        &impl->engine, kMusicPath, MA_SOUND_FLAG_STREAM,
        nullptr, nullptr, &impl->music) == MA_SUCCESS)
    {
        impl->musicReady = true;

        ma_sound_set_looping(&impl->music, MA_TRUE);
        ma_sound_set_volume(&impl->music, kMusicVolume);
        ma_sound_start(&impl->music);
    }
    else
    {
        std::cerr << "Music not loaded: " << kMusicPath << '\n';
    }
}

AudioSystem::~AudioSystem()
{
    if (impl->musicReady) ma_sound_uninit(&impl->music);
    if (impl->groupReady) ma_sound_group_uninit(&impl->sfxGroup);
    if (impl->engineReady) ma_engine_uninit(&impl->engine);
}

void AudioSystem::Play(SoundId id)
{
    if (!impl->engineReady) return;

    ma_engine_play_sound(
        &impl->engine,
        kSfxPaths[static_cast<std::size_t>(id)],
        impl->groupReady ? &impl->sfxGroup : nullptr);
}