#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"
enum EntityType { PLATFORM, PLAYER, ENEMY, HEALTH, PROJECTILE, OBJECT};
enum AIType { PAUSED, FOLLOWER, CLAM, BUBBLE, ENEMY_PROJECTILE, BOSS };
enum AIState { WALKING, IDLE, ATTACK };


enum AnimationDirection { LEFT, RIGHT};

class Entity
{
private:
    bool m_is_active = true;
    std::vector<std::vector<int>> m_walking; // vector containing vector to enable any array as input as long as you know the correct columns and rows


    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;

    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    glm::mat4 m_model_matrix;


    // ————— TEXTURES ————— //
    GLuint    m_texture_id;

    // ————— ANIMATION ————— //
    int m_animation_cols;
    int m_animation_frames,
        m_animation_index,
        m_animation_rows;

    std::vector<int> m_animation_indices;
    float m_animation_time = 0.0f;

    float m_width = 1.0f,
        m_height = 1.0f;

    // ————— COLLISIONS ————— //
    bool m_collided_top = false;
    bool m_collided_bottom = false;
    bool m_collided_left = false;
    bool m_collided_right = false;
    bool m_collided_projectile = false;
    bool can_take_damage = true;
    bool paused = false;
    bool m_attacking = false;

    // ----- STATS ----- //
    float m_projectile_speed = 3.0f;
    float m_speed;
    float m_current_health;
    float m_max_health = 100.0f;
    float m_damage;
    float pierce = 3;
    float m_projectile_rate = 1.0f;
    float m_projectile_timer = .5f;
    float m_projectile_time = .5f;
    float m_damage_timer = .5f;
	float m_damage_time = .5f;

    int m_enemies_killed = 0;
    int exp = 0;
    int exp_amount;

public:
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 4;

    // ————— METHODS ————— //
    Entity();
    Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float attack_damage, std::vector<std::vector<int>> walking, float animation_time,
        int animation_frames, int animation_index, int animation_cols,
        int animation_rows, float width, float height, EntityType EntityType);
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType); // Simpler constructor
    Entity(GLuint texture_id, float speed, float health, float damage, float projectile_speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState); // AI constructor
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;

    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);

    // Overloading our methods to check for only the map
    void const check_collision_y(Map* map);
    void const check_collision_x(Map* map);

    void update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map* map);
    void render(ShaderProgram* program);

    void ai_activate(Entity* entities);
    void ai_follower(Entity* entities);
    void ai_clam(Entity* entities);
    void ai_paused(Entity* entities);
    void ai_bubble(glm::vec3 cursor_pos);
    void ai_enemy_projectile(glm::vec3 player_pos);
	void ai_boss(Entity* entities);

	void levelup(int choice);
    
    void pierce_damage() { m_current_health -= 1.0f; }
    void normalise_movement() { m_movement = 5.0f * glm::normalize(m_movement); }

    void face_left() { m_animation_indices = m_walking[LEFT]; }
    void face_right() { m_animation_indices = m_walking[RIGHT]; }

    void move_left() { m_movement.x = -.75f; face_left(); }
    void move_right() { m_movement.x = .75f;  face_right(); }
    void move_down() { m_movement.y = -.75f;}
    void move_up() { m_movement.y = .75f;}
    void const change_projectile() { m_collided_projectile = !m_collided_projectile; }
    void take_damage(float damage) { m_current_health -= damage; }
    void reset_projectile() { m_projectile_time = m_projectile_timer; }
    void reset_damage() { m_damage_time = m_damage_timer; }
	void pause() { paused = true; }
	void unpause() { paused = false; }
	void add_kills(int kills) { m_enemies_killed += kills;}
	void add_exp(int experience) { exp += experience; }

    //----UPGRADES----//
    void health_boost() { float health_percentage = get_health();  m_max_health += 25; m_current_health = health_percentage * m_max_health; }
    void damage_boost() { m_damage += 2; }
    void speed_boost() { m_speed *= 1.25;}
    void pierce_boost() { pierce += 2; }
    void range_boost() { m_projectile_timer *= 1.25; m_projectile_time *= 1.25; }
    void firerate_boost() { m_projectile_rate *= .8; m_projectile_timer *= .8; m_projectile_time *= .8; m_projectile_speed *= 1.1; }
    
    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type; };
    AIType     const get_ai_type()        const { return m_ai_type; };
    AIState    const get_ai_state()       const { return m_ai_state; };
    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    glm::vec3 const get_scale()        const { return m_scale; }
    GLuint    const get_texture_id()   const { return m_texture_id; }
    float     const get_speed()        const { return m_speed; }
    float get_damage() const { return m_damage; }
	float get_pierce() const { return pierce; }
    float get_health() const { return m_current_health/m_max_health;}
    int get_max_health() const { return m_max_health; }
    int get_current_health() const { return m_current_health; }
    float get_rate() const { return m_projectile_rate;}
	float get_timer() const { return m_projectile_time; }
	float get_projectile_speed() const { return m_projectile_speed; }
	float get_projectile_time() const { return m_projectile_time; }
	int get_kills() const { return m_enemies_killed; }
	int get_current_exp() const { return exp; }
	int get_exp() const { return exp_amount; }

    
    bool 	  const get_is_active() const { return m_is_active; }
    
    std::vector<std::vector<int>>      const get_m_walking() const { return m_walking; }
    std::vector<int>      const get_animation_indices() const { return m_animation_indices; }
    int get_animation_index() const { return m_animation_index;}
	int get_animation_cols() const { return m_animation_cols; }
	int get_animation_rows() const { return m_animation_rows; }
    bool      const get_collided_top() const { return m_collided_top; }
    bool      const get_collided_bottom() const { return m_collided_bottom; }
    bool      const get_collided_right() const { return m_collided_right; }
    bool      const get_collided_left() const { return m_collided_left; }
	bool      const get_collided_projectile() const { return m_collided_projectile; }
	bool      const get_paused() const { return paused; }

    void activate() { m_is_active = true; };
    void deactivate() { m_is_active = false; };

    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type) { m_entity_type = new_entity_type; };
    void const set_ai_type(AIType new_ai_type) { m_ai_type = new_ai_type; };
    void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_texture_id(GLuint new_texture_id) { m_texture_id = new_texture_id; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_animation_cols(int new_cols) { m_animation_cols = new_cols; }
    void const set_animation_rows(int new_rows) { m_animation_rows = new_rows; }
    void const set_animation_frames(int new_frames) { m_animation_frames = new_frames; }
    void const set_animation_index(int new_index) { m_animation_index = new_index; }
    void const set_animation_time(float new_time) { m_animation_time = new_time; }
    void const set_width(float new_width) { m_width = new_width; }
    void const set_height(float new_height) { m_height = new_height; }
    void const set_rate(float rate) { m_projectile_rate = rate; }
    void const set_timer(float timer) { m_projectile_timer = timer; }
	void const set_health(float health) { m_current_health = health; }
    void const set_max_health(float health) { m_max_health = health; }
    void const set_damage(float damage) { m_damage = damage; }
	void const set_pierce(float pierce) { pierce = pierce; }
	void const set_projectile_speed(float speed) { m_projectile_speed = speed; }
	void const set_projectile_time(float time) { m_projectile_time = time; }
	void const set_kills(int kills) { m_enemies_killed = kills; }
	void const set_current_exp(int experience) { exp = experience; }
	void const set_exp(int experience) { exp_amount = experience; }

    // Setter for m_walking
    void set_walking(std::vector<std::vector<int>> walking)
    {
        m_walking = walking;
    }
};

#endif // ENTITY_H
