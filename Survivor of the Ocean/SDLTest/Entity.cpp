#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

void Entity::ai_activate(Entity* entities)
{
    switch (m_ai_type)
    {
    case FOLLOWER:
        ai_follower(entities);
        break;
    case PAUSED:
        ai_paused(entities);
        break;

	case CLAM:
		ai_clam(entities);
		break;
    default:
        break;
    }
}

void Entity::ai_follower(Entity* entities)
{
    switch (m_ai_state) {
    case IDLE:
		m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        if (glm::distance(m_position, entities->get_position()) < 1000.0f) { m_ai_state = WALKING; }
        break;

    case WALKING:
        if (m_position.x > entities->get_position().x) 
        {
            m_movement.x = -0.50f;
			face_left();
        }
		else if (m_position.x < entities->get_position().x)
        {
            m_movement.x = 0.50f;
            face_right();
        }

		if (m_position.y > entities->get_position().y)
		{
            m_movement.y = -0.50f;
		}
		else if (m_position.y < entities->get_position().y)
		{
            m_movement.y = 0.50f;
		}

        break;

    default:
        break;
    }
}

void Entity::ai_clam(Entity* entities)
{

    if (glm::distance(m_position, entities->get_position()) > 6.0f)
    {
        if (m_position.x > entities->get_position().x)
        {
            m_movement.x = -0.75f;
            face_left();
        }
        else if (m_position.x < entities->get_position().x)
        {
            m_movement.x = 0.75f;
            face_right();
        }

        if (m_position.y > entities->get_position().y)
        {
            m_movement.y = -0.75f;
        }
        else if (m_position.y < entities->get_position().y)
        {
            m_movement.y = 0.75f;
        }

		m_ai_state = WALKING;
    }
    else
    {
        m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        if (m_position.x > entities->get_position().x)
        {
            face_left();
        }
        else if (m_position.x < entities->get_position().x)
        {
            face_right();
        }

		m_ai_state = IDLE;
    }

}

void Entity::ai_paused(Entity* entities)
{

}


void Entity::ai_bubble(glm::vec3 cursor_pos)
{
    // Calculate the direction vector towards the cursor position
    glm::vec3 direction = cursor_pos - m_position;

    // Normalize the direction vector to get consistent speed
    direction = glm::normalize(direction);


    // Scale the direction vector by the desired movement speed
    m_movement = direction * m_speed;
}


void Entity::ai_enemy_projectile(glm::vec3 player_pos)
{
    // Calculate the direction vector towards the cursor position
    glm::vec3 direction = player_pos - m_position;

    // Normalize the direction vector to get consistent speed
    direction = glm::normalize(direction);


    // Scale the direction vector by the desired movement speed
    m_movement = direction * m_speed;

}

void Entity::levelup(int choice)
{
    switch (choice)
    {
    case 1:
        health_boost();
        break;
    case 2:
        damage_boost();
        break;
    case 3:
        speed_boost();
        break;
    case 4:
        pierce_boost();
        break;
    case 5:
        range_boost();
        break;
    case 6:
        firerate_boost();
        break;
    default:
        break;
    }
}

// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f)
{
    // Initialize m_walking with zeros or any default value
    m_walking = { { 0 } };
}

// Parameterized constructor
Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float damage, std::vector<std::vector<int>> walking, float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_acceleration(acceleration), m_damage(damage), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(EntityType)
{
    m_current_health = m_max_health;
    set_walking(walking);
	face_left();
}

// Simpler constructor for partial initialization
Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(EntityType)
{
    // Initialize m_walking with zeros or any default value
    m_walking = { { 0 } };
}


Entity::Entity(GLuint texture_id, float speed, float max_health, float damage, float projectile_speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_max_health(max_health), m_damage(damage), m_projectile_speed(projectile_speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState)
{
    m_current_health = m_max_health;
    // Initialize m_walking with zeros or any default value
    m_walking = { { 0 } };
}

Entity::~Entity() { }

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const
{
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];
        if (!collidable_entity->m_is_active || collidable_entity->m_collided_projectile)
        {
            continue;
        }

        if (collidable_entity == this)
        {
			continue;
        }

        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));


            if (collidable_entity->m_entity_type == ENEMY && m_entity_type != ENEMY)
            {
                if (m_entity_type == PROJECTILE && m_is_active)
                {
                    if (!collidable_entity->m_collided_projectile)
                    {

                        collidable_entity->change_projectile();
                        m_current_health -= 1;
                        if (m_current_health <= 0.0f)
                        {
                            m_is_active = false;
                        }

                        collidable_entity->take_damage(m_damage);

                        if (collidable_entity->get_health() <= 0.0f)
                        {
                            collidable_entity->deactivate();

                            m_enemies_killed += 1;

                            exp += collidable_entity->get_exp();
                        }
                    }
                }

                else
                {
                    if (m_entity_type == PLAYER)
                    {
                        if (can_take_damage)
                        {
                            m_current_health -= collidable_entity->get_damage();
                            can_take_damage = false;
                        }
                    }
                }
            }

            if (collidable_entity->get_ai_type() == ENEMY_PROJECTILE)
            {

                if (m_entity_type == PLAYER && can_take_damage)
                {
                    collidable_entity->take_damage(100);
                    m_current_health -= collidable_entity->get_damage();
                    can_take_damage = false;
                }
                else
                {
                    collidable_entity->take_damage(m_damage);
                    m_current_health -= collidable_entity->get_damage();
                }
            }

            if (collidable_entity->get_health() <= 0.0f)
            {
                collidable_entity->deactivate();
            }

            if (m_current_health <= 0.0f)
            {
                m_is_active = false;
                reset_projectile();
                return;
            }


            if (m_velocity.y > 0 && m_entity_type != PROJECTILE)
            {
                m_position.y -= y_overlap + .25;

                m_velocity.y = 0;
                

                // Collision!
                m_collided_top = true;
            }

            else if (m_velocity.y < 0 && m_entity_type != PROJECTILE)
            {
                m_position.y += y_overlap + .25;
                
                m_velocity.y = 0;
                
                //Collision!
                m_collided_bottom = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (!collidable_entity->m_is_active || collidable_entity->m_collided_projectile)

        {
            continue;
        }

        if (collidable_entity == this)
        {
            continue;
        }

        if (check_collision(collidable_entity))
        {

            if (collidable_entity->m_entity_type == ENEMY && m_entity_type != ENEMY)
            {
                if (m_entity_type == PROJECTILE && m_is_active)
                {
                    if (!collidable_entity->m_collided_projectile)
                    {

                        collidable_entity->change_projectile();
                        m_current_health -= 1;
                        if (m_current_health <= 0.0f)
                        {
                            m_is_active = false;
                        }

                        collidable_entity->take_damage(m_damage);

                        if (collidable_entity->get_health() <= 0.0f)
                        {
                            collidable_entity->deactivate();

                            m_enemies_killed += 1;

                            exp += collidable_entity->get_exp();
                        }
                    }
                }

                else
                {
                    if (m_entity_type == PLAYER)
                    {
                        if (can_take_damage)
                        {
                            m_current_health -= collidable_entity->get_damage();
                            can_take_damage = false;
                        }
                    }
                }
            }

            if (collidable_entity->get_ai_type() == ENEMY_PROJECTILE)
            {
                if (can_take_damage)
                {
                    collidable_entity->take_damage(100);
                    m_current_health -= collidable_entity->get_damage();
					can_take_damage = false;
                }
            }

            if (collidable_entity->get_health() <= 0.0f)
            {
                collidable_entity->deactivate();
            }

            if (m_current_health <= 0.0f)
            {
                m_is_active = false;
                reset_projectile();
                return;
            }

            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0 && m_entity_type != PROJECTILE)
            {
                m_position.x -= x_overlap + .25;

                m_velocity.x = 0;
                // Collision!
                m_collided_right = true;

            }
            else if (m_velocity.x < 0 && m_entity_type != PROJECTILE)
            {
                m_position.x += x_overlap + .25;
                
                m_velocity.x = 0;
                
                // Collision!
                m_collided_left = true;
            }
        }
    }
}

void const Entity::check_collision_y(Map* map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;

    }

    if (m_collided_top || m_collided_bottom)
    {
        if (m_entity_type == PROJECTILE)
        {
            m_is_active = false;
            reset_projectile();
        }
    }
}

void const Entity::check_collision_x(Map* map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        //For whatever this small value added to the right adjustment fixes a clipping problem
        m_position.x -= penetration_x + .01;
        m_velocity.x = 0;
        m_collided_right = true;
    }


    if (m_collided_left || m_collided_right)
    {
        if (m_entity_type == PROJECTILE)
        {
            m_is_active = false;
            reset_projectile();
        }
    }
}


void Entity::update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map* map)
{
    if (!paused)
    {
        if (!m_is_active) return;

        m_collided_top = false;
        m_collided_bottom = false;
        m_collided_left = false;
        m_collided_right = false;

        if (m_entity_type == ENEMY && player != NULL) ai_activate(player);

        if (m_entity_type == PROJECTILE || m_ai_type == ENEMY_PROJECTILE)
        {
            m_projectile_time -= delta_time;
            if (m_projectile_time <= 0.0f)
            {
                m_is_active = false;
                reset_projectile();
            }
        }

        if (m_entity_type == PLAYER)
        {
            if (!can_take_damage)
            {
                m_damage_time -= delta_time;
                if (m_damage_time <= 0.0f)
                {
                    can_take_damage = true;
                    reset_damage();
                }
            }
        }

        if (!m_animation_indices.empty())
        {
            if (glm::length(m_movement) != 0)
            {
                m_animation_time += delta_time;
                float frames_per_second = (float)1 / SECONDS_PER_FRAME;

                if (m_animation_time >= frames_per_second)
                {
                    m_animation_time = 0.0f;
                    m_animation_index++;

                    if (m_animation_index >= m_animation_frames)
                    {
                        m_animation_index = 0;
                    }
                }
            }
        }


        //non_enemies can pass through objects
        if (m_ai_type != FOLLOWER && m_ai_type != CLAM && m_ai_type != ENEMY_PROJECTILE)
        {

            m_velocity.x = m_movement.x * m_speed;
            m_velocity.y = m_movement.y * m_speed;
            m_velocity += m_acceleration * delta_time;

            m_position.y += m_velocity.y * delta_time;

            check_collision_y(collidable_entities, collidable_entity_count);
            check_collision_y(map);

            m_position.x += m_velocity.x * delta_time;

            check_collision_x(collidable_entities, collidable_entity_count);
            check_collision_x(map);

        }

        else
        {
            m_velocity.x = m_movement.x * m_speed;
            m_velocity.y = m_movement.y * m_speed;

            m_position.x += m_velocity.x * delta_time;
            m_position.y += m_velocity.y * delta_time;
        }

        m_model_matrix = glm::mat4(1.0f);

        m_model_matrix = glm::translate(m_model_matrix, m_position);
        m_model_matrix = glm::scale(m_model_matrix, m_scale);

    }
}


void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    if (!m_animation_indices.empty())
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}