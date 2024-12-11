#include "LevelB.h"
#include "Utility.h"


constexpr char

//I made these
SPRITESHEET_FILEPATH[] = "assets/whale.png",
SHRIMP_FILEPATH[] = "assets/shrimp.png",
BUBBLE_FILEPATH[] = "assets/bubble.png",
CLAM_FILEPATH[] = "assets/clam.png",
CLAMPROJECTILE_FILEPATH[] = "assets/clamprojectile.png",


MAP_TILESET_FILEPATH[] = "assets/tileset.png", //From https://axulart.itch.io/axularts-beach-and-caves-tileset

BGM_FILEPATH[] = "assets/levelB_theme.mp3", //From Incompetech, "On the Shore"

BUBBLE_SFX_FILEPATH[] = "assets/bubbles.wav"; //From https://opengameart.org/content/bubble-pop

int LevelB_current_enemies = 0;
float LevelB_timer = 0.0f;

unsigned int LEVEL_B_DATA[] =
{
    45, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 47,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  107,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  107,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  107,  0,  0,  106,
    105, 0,  0,  0,  107,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  107,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  107,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  107,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  107,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  107,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    105, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  106,
    75, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 77,
};

LevelB::~LevelB()
{
    
    delete    m_game_state.player;
    delete m_game_state.bubble;

    for (int i = 0; i < m_number_of_enemies; i++)
    {
        delete m_game_state.enemies[i];
    }

    for (int i = 0; i < m_number_of_enemies; i++)
    {
        delete m_game_state.projectiles[i];
    }

    delete    m_game_state.map;

    Mix_FreeChunk(m_game_state.bubble_sfx);
    Mix_FreeMusic(m_game_state.bgm);
    
}

void LevelB::initialise()
{
    
    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_B_DATA, map_texture_id, 1.0f, 15, 8);

    // ————— GEORGE SET-UP ————— //

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    GLuint shrimp_texture_id = Utility::load_texture(SHRIMP_FILEPATH);
    GLuint clam_texture_id = Utility::load_texture(CLAM_FILEPATH);
    GLuint bubble_texture_id = Utility::load_texture(BUBBLE_FILEPATH);
	GLuint clamprojectile_id = Utility::load_texture(CLAMPROJECTILE_FILEPATH);

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    std::vector<std::vector <int> > walking_animation =
    {
        { 0, 1, 2, 3 },  // to move to the left,
        { 4, 5, 6, 7 } // to move to the right,
    };

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        speed,                      // speed
        acceleration,              // acceleration
        damage,                      // attack damage
        walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        8,                         // animation column amount
        1,                         // animation row amount
        0.5f,                      // width
        0.5f,                       // height
        PLAYER
    );
    m_game_state.player->set_position(glm::vec3(10.0f, -10.0f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(1.5f, 1.0f, 0.0f));
    m_game_state.player->set_max_health(max_health);
	m_game_state.player->set_health(current_health);
	m_game_state.player->set_pierce(pierce);
	m_game_state.player->set_projectile_speed(projectile_speed);
    m_game_state.player->set_projectile_time(projectile_time);
	m_game_state.player->set_rate(projectile_rate);
	m_game_state.player->set_timer(projectile_timer);
	m_game_state.player->set_current_exp(current_exp);

    m_game_state.bubble = new Entity(bubble_texture_id, m_game_state.player->get_projectile_speed(), m_game_state.player->get_pierce(), m_game_state.player->get_damage(), 0.0f, 0.5f, 0.4f, PROJECTILE, BUBBLE, IDLE);
    m_game_state.bubble->set_position(m_game_state.player->get_position());
    m_game_state.bubble->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.bubble->set_scale(glm::vec3(.50f, .4f, 1.0f));
    m_game_state.bubble->set_rate(m_game_state.player->get_rate());
    attack_rate = m_game_state.bubble->get_rate();
    m_game_state.bubble->set_timer(m_game_state.bubble->get_rate());
	m_game_state.bubble->set_projectile_time(m_game_state.player->get_projectile_time());

    int random_pos;
    for (int i = 0; i < m_number_of_enemies; i++)
    {
        random_pos = rand() % 76; //picks a random position for the enemy to spawn

        m_game_state.enemies.push_back(new Entity(clam_texture_id, 1.0f, 10.0f, 10.0f, 2.0f, 0.5f, 0.5f, ENEMY, CLAM, WALKING));
        m_game_state.enemies[i]->set_position(enemy_positions[random_pos]);
        m_game_state.enemies[i]->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
        m_game_state.enemies[i]->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
        m_game_state.enemies[i]->set_exp(20);

        m_game_state.enemies[i]->set_animation_cols(8);
        m_game_state.enemies[i]->set_animation_rows(1);
        m_game_state.enemies[i]->set_animation_frames(4);
        m_game_state.enemies[i]->set_animation_index(0);
        m_game_state.enemies[i]->set_animation_time(0.0f);
        m_game_state.enemies[i]->set_walking(walking_animation);

        m_game_state.enemies[i]->set_rate(2.5f);
		m_game_state.enemies[i]->set_timer(2.0f);
		m_game_state.enemies[i]->set_projectile_time(2.0f);
		m_game_state.enemies[i]->set_pierce(5.0f);

        m_game_state.projectiles.push_back(new Entity(clamprojectile_id, 5.0f, 1.0, m_game_state.enemies[i]->get_damage(), 0.0f, 0.5f, 0.5f, PROJECTILE, ENEMY_PROJECTILE, IDLE));
        m_game_state.projectiles[i]->deactivate();
        m_game_state.projectiles[i]->set_position(m_game_state.enemies[i]->get_position());
		m_game_state.projectiles[i]->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
		m_game_state.projectiles[i]->set_scale(glm::vec3(.4f, .6f, 1.0f));
		m_game_state.projectiles[i]->set_rate(1.0f);
        enemy_attack_rate = m_game_state.enemies[i]->get_rate();
		m_game_state.projectiles[i]->set_timer(m_game_state.enemies[i]->get_timer());
        m_game_state.projectiles[i]->set_projectile_time(m_game_state.enemies[i]->get_projectile_time());
    }

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4.0f);
    Mix_PlayMusic(m_game_state.bgm, -1);

    m_game_state.bubble_sfx = Mix_LoadWAV(BUBBLE_SFX_FILEPATH);

    Mix_VolumeChunk(m_game_state.bubble_sfx, MIX_MAX_VOLUME / 2.0f);

}

void LevelB::update(float delta_time)
{
    
    if (!level_on)
    {
        return;
    }
    else
    {
        m_game_state.player->add_kills(m_game_state.bubble->get_kills());
        m_game_state.bubble->set_kills(0);
        m_game_state.player->add_exp(m_game_state.bubble->get_current_exp());
        m_game_state.bubble->set_current_exp(0);

        if (m_game_state.player->get_kills() >= m_number_of_enemies)
        {
            clear = true;
            return;
        }

        LevelB_timer += delta_time;
        if (LevelB_timer >= 1.0f)
        {
            if (LevelB_current_enemies < m_number_of_enemies)
            {
                LevelB_current_enemies++;
            }
            LevelB_timer = 0.0f;
        }

        attack_rate -= delta_time;
		enemy_attack_rate -= delta_time;
        
        if (m_game_state.player->get_is_active())
        {
            m_game_state.player->update(delta_time, m_game_state.player, NULL, 0,
                m_game_state.map);

            if (attack_rate <= 0.0f)
            {
                if (!m_game_state.bubble->get_is_active())
                {
                    m_game_state.bubble->set_position(m_game_state.player->get_position());
                    m_game_state.bubble->set_health(m_game_state.player->get_pierce());
                    m_game_state.bubble->set_damage(m_game_state.player->get_damage());
                    m_game_state.bubble->set_rate(m_game_state.player->get_rate());
                    m_game_state.bubble->set_timer(m_game_state.player->get_timer());
                    m_game_state.bubble->set_speed(m_game_state.player->get_projectile_speed());
                    attack_rate = m_game_state.bubble->get_rate();
                    m_game_state.bubble->activate();
                    Mix_PlayChannel(-1, m_game_state.bubble_sfx, 0);
                    for (int i = 0; i < LevelB_current_enemies; i++)
                    {
                        if (m_game_state.enemies[i]->get_collided_projectile())
                        {
                            m_game_state.enemies[i]->change_projectile();
                        }
                    }
                }
            }
            
            
            if (enemy_attack_rate <= 0.0f)
            {
                player_position = m_game_state.player->get_position();
                for (int i = 0; i < LevelB_current_enemies ; i++)
                {
                    if (m_game_state.enemies[i]->get_is_active())
                    {
                        if (m_game_state.enemies[i]->get_ai_state() == IDLE && !m_game_state.projectiles[i]->get_is_active())
                        {
                            m_game_state.projectiles[i]->set_position(m_game_state.enemies[i]->get_position());
                            m_game_state.projectiles[i]->set_health(m_game_state.enemies[i]->get_pierce());
                            m_game_state.projectiles[i]->set_damage(m_game_state.enemies[i]->get_damage());
                            m_game_state.projectiles[i]->set_rate(m_game_state.enemies[i]->get_rate());
                            m_game_state.projectiles[i]->set_timer(m_game_state.enemies[i]->get_timer());
                            m_game_state.projectiles[i]->set_speed(m_game_state.enemies[i]->get_projectile_speed());
                            m_game_state.projectiles[i]->activate();
                            if (m_game_state.player->get_collided_projectile())
                            {
                                m_game_state.player->change_projectile();
                            }
                        }
                    }
                }
                enemy_attack_rate = m_game_state.enemies[0]->get_rate();
            }
            
            
            
            m_game_state.bubble->ai_bubble(mouse_position);
            m_game_state.bubble->update(delta_time, m_game_state.player, NULL, 0,
                m_game_state.map);


            for (int i = 0; i < LevelB_current_enemies; i++)
            {
                m_game_state.player->check_collision_x(m_game_state.enemies[i], 1);
                m_game_state.player->check_collision_y(m_game_state.enemies[i], 1);
                m_game_state.bubble->check_collision_x(m_game_state.enemies[i], 1);
                m_game_state.bubble->check_collision_y(m_game_state.enemies[i], 1);
                
                m_game_state.player->check_collision_x(m_game_state.projectiles[i], 1);
                m_game_state.player->check_collision_y(m_game_state.projectiles[i], 1);
                m_game_state.bubble->check_collision_x(m_game_state.projectiles[i], 1);
                m_game_state.bubble->check_collision_y(m_game_state.projectiles[i], 1);
             
            }
        }

        for (int i = 0; i < LevelB_current_enemies; i++)
        {
            m_game_state.enemies[i]->update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
            m_game_state.projectiles[i]->ai_enemy_projectile(player_position);
            m_game_state.projectiles[i]->update(delta_time, NULL, NULL, 0, m_game_state.map);
        }
    }
    
}


void LevelB::render(ShaderProgram* g_shader_program)
{
    
    //draws collidable entities if they are active
    if (m_game_state.player->get_is_active())
    {
        m_game_state.player->render(g_shader_program);
    }

    for (int i = 0; i < LevelB_current_enemies; i++)
    {
        if (m_game_state.enemies[i]->get_is_active())
        {
            m_game_state.enemies[i]->render(g_shader_program);
        }
    }
    
    
    for (int i = 0; i < LevelB_current_enemies; i++)
    {
        if (m_game_state.enemies[i]->get_is_active())
        {
            if (m_game_state.projectiles[i]->get_is_active())
            {
                m_game_state.projectiles[i]->render(g_shader_program);
            }
        }
    }
    

    if (m_game_state.bubble->get_is_active())
    {
        m_game_state.bubble->render(g_shader_program);
    }


    m_game_state.map->render(g_shader_program);
    
}

void LevelB::kill()
{
    clear = true;
}

void LevelB::turn_on()
{
    

    for (int i = 0; i < m_number_of_enemies; i++)
    {
        m_game_state.enemies[i]->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
        m_game_state.enemies[i]->set_ai_type(CLAM);
        m_game_state.enemies[i]->unpause();
		m_game_state.projectiles[i]->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
		m_game_state.projectiles[i]->set_ai_type(ENEMY_PROJECTILE);
		m_game_state.projectiles[i]->unpause();
    }

    m_game_state.player->set_entity_type(PLAYER);
    m_game_state.player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.player->unpause();
    m_game_state.bubble->set_ai_type(BUBBLE);
    m_game_state.bubble->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.bubble->unpause();
    level_on = true;
    

}
void LevelB::turn_off()
{
    
    level_on = false;
    for (int i = 0; i < m_number_of_enemies; i++)
    {
        m_game_state.enemies[i]->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
        m_game_state.enemies[i]->set_ai_type(PAUSED);
        m_game_state.enemies[i]->pause();
		m_game_state.projectiles[i]->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
		m_game_state.projectiles[i]->set_ai_type(PAUSED);
		m_game_state.projectiles[i]->pause();
    }

    m_game_state.player->set_entity_type(PLATFORM);
    m_game_state.player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.player->pause();
    m_game_state.bubble->set_ai_type(PAUSED);
    m_game_state.bubble->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.bubble->pause();   
}