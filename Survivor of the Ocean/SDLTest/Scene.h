#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"



// ————— GAME STATE ————— //
struct GameState
{
    Entity* player;

    std::vector<Entity*> enemies;
    std::vector<Entity*> projectiles;

    Entity* bubble;
    Entity* menu;
    Entity* boss;

    Entity* text;

    Map* map;

    Mix_Music* bgm;
    Mix_Music* boss_bgm;

	Mix_Chunk* lose_sfx;
	Mix_Chunk* win_sfx;
	Mix_Chunk* bubble_sfx;
};

class Scene {
protected:
    GameState m_game_state;
    
public:

    int m_number_of_enemies = 10;
    int m_enemies_defeated = 0;

    float max_health;
    float current_health;
    float damage;
    float speed;
	float pierce;
	float projectile_speed;
	float projectile_rate;
    float projectile_timer;
	float projectile_time;
    int current_exp;

    // ————— ATTRIBUTES ————— //
    GLuint text_texture_id;

    glm::vec3 mouse_position;
	glm::vec3 player_position;

    std::vector<glm::vec3> enemy_positions =
    {
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.0f),
        glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(7.0f, 0.0f, 0.0f),
        glm::vec3(8.0f, 0.0f, 0.0f), glm::vec3(9.0f, 0.0f, 0.0f), glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(11.0f, 0.0f, 0.0f),
        glm::vec3(12.0f, 0.0f, 0.0f), glm::vec3(13.0f, 0.0f, 0.0f), glm::vec3(14.0f, 0.0f, 0.0f), glm::vec3(15.0f, 0.0f, 0.0f),
        glm::vec3(16.0f, 0.0f, 0.0f), glm::vec3(17.0f, 0.0f, 0.0f), glm::vec3(18.0f, 0.0f, 0.0f), glm::vec3(19.0f, 0.0f, 0.0f),

        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(19.0f, -1.0f, 0.0f), glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(19.0f, -2.0f, 0.0f),
        glm::vec3(0.0f, -3.0f, 0.0f), glm::vec3(19.0f, -3.0f, 0.0f), glm::vec3(0.0f, -4.0f, 0.0f), glm::vec3(19.0f, -4.0f, 0.0f),
        glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(19.0f, -5.0f, 0.0f), glm::vec3(0.0f, -6.0f, 0.0f), glm::vec3(19.0f, -6.0f, 0.0f),
        glm::vec3(0.0f, -7.0f, 0.0f), glm::vec3(19.0f, -7.0f, 0.0f), glm::vec3(0.0f, -8.0f, 0.0f), glm::vec3(19.0f, -8.0f, 0.0f),
        glm::vec3(0.0f, -9.0f, 0.0f), glm::vec3(19.0f, -9.0f, 0.0f), glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(19.0f, -10.0f, 0.0f),
        
        glm::vec3(0.0f, -11.0f, 0.0f), glm::vec3(19.0f, -11.0f, 0.0f), glm::vec3(0.0f, -12.0f, 0.0f), glm::vec3(19.0f, -12.0f, 0.0f),
        glm::vec3(0.0f, -13.0f, 0.0f), glm::vec3(19.0f, -13.0f, 0.0f), glm::vec3(0.0f, -14.0f, 0.0f), glm::vec3(19.0f, -14.0f, 0.0f),
        glm::vec3(0.0f, -15.0f, 0.0f), glm::vec3(19.0f, -15.0f, 0.0f), glm::vec3(0.0f, -16.0f, 0.0f), glm::vec3(19.0f, -16.0f, 0.0f),
        glm::vec3(0.0f, -17.0f, 0.0f), glm::vec3(19.0f, -17.0f, 0.0f), glm::vec3(0.0f, -18.0f, 0.0f), glm::vec3(19.0f, -18.0f, 0.0f),
        glm::vec3(0.0f, -19.0f, 0.0f), glm::vec3(1.0f, -19.0f, 0.0f), glm::vec3(2.0f, -19.0f, 0.0f), glm::vec3(3.0f, -19.0f, 0.0f),  
        
        glm::vec3(4.0f, -19.0f, 0.0f), glm::vec3(5.0f, -19.0f, 0.0f), glm::vec3(6.0f, -19.0f, 0.0f), glm::vec3(7.0f, -19.0f, 0.0f),
        glm::vec3(8.0f, -19.0f, 0.0f), glm::vec3(9.0f, -19.0f, 0.0f), glm::vec3(10.0f, -19.0f, 0.0f), glm::vec3(11.0f, -19.0f, 0.0f),
        glm::vec3(12.0f, -19.0f, 0.0f), glm::vec3(13.0f, -19.0f, 0.0f), glm::vec3(14.0f, -19.0f, 0.0f), glm::vec3(15.0f, -19.0f, 0.0f), 
        glm::vec3(16.0f, -19.0f, 0.0f), glm::vec3(17.0f, -19.0f, 0.0f), glm::vec3(18.0f, -19.0f, 0.0f), glm::vec3(19.0f, -19.0f, 0.0f),
    };
    
    bool level_on = false;
    bool clear = false;
    bool win = false;
    bool levelup = false;

    float attack_rate = 0;
    float enemy_attack_rate = 0;

    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    virtual void kill() = 0;
    virtual void turn_on() = 0;
	virtual void turn_off() = 0;
    virtual void level_clear() = 0;
    
    // ————— GETTERS ————— //
    GameState const get_state() const { return m_game_state;             }
    int const get_number_of_enemies() const { return m_number_of_enemies; }
};
