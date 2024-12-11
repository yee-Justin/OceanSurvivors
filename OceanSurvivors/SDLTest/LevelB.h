#pragma once
#include "Scene.h"

class LevelB : public Scene {
public:
    static const int LEVEL_WIDTH = 20;
    static const int LEVEL_HEIGHT = 20;

    ~LevelB();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    void kill() override;
    void turn_on() override;
    void turn_off() override;
};
