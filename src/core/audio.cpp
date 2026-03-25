#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "audio.h"

void AudioManager::init() {
    ma_engine_init(NULL, &engine);
};

void AudioManager::shutdown() {
    ma_engine_uninit(&engine);
};

void AudioManager::loadSound(const std::string &name, const std::string &filepath) {
    sounds[name] = filepath;
};

void AudioManager::playSound(const std::string &name) {
    if (sounds.count(name)) {
        ma_engine_play_sound(&engine, sounds[name].c_str(), NULL);
    }
};