#pragma once
#include "miniaudio.h"
#include <unordered_map>
#include <string>

class AudioManager {
public:
    void init();
    void shutdown();

    void loadSound(const std::string& name, const std::string& filepath);
    void playSound(const std::string& name);

private:
    ma_engine engine;
    std::unordered_map<std::string, std::string> sounds;
};