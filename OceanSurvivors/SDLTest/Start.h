#pragma once
#include "Scene.h"

class Start : public Scene {
public:
    static const int LEVEL_WIDTH = 14;
    static const int LEVEL_HEIGHT = 8;

    ~Start();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    void kill();
    void turn_on() { level_on = true; }
    void turn_off() { level_on = false; }
};
