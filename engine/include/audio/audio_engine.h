#pragma once

namespace d2::audio {

class AudioEngine {
public:
    AudioEngine() = default;
    ~AudioEngine() = default;

    bool initialize();
    bool isInitialized() const;

private:
    bool initialized_ = false;
};

} // namespace d2::audio