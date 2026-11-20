#pragma once

#include <memory>

#include "SoundId.h"

class AudioSystem
{
public:
    AudioSystem();
    ~AudioSystem();

    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;

    void Play(SoundId id);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};