#include "LevelA.h"
#include "Utility.h"

constexpr char 
//I made these
SPRITESHEET_FILEPATH[] = "assets/whale.png", 
SHRIMP_FILEPATH[] = "assets/shrimp.png",
BUBBLE_FILEPATH[] = "assets/bubble.png",
CLAM_FILEPATH[] = "assets/clam.png",

MAP_TILESET_FILEPATH[] = "assets/map.png", //From https://opengameart.org/content/tile-map-dark-2d-platformer

BGM_FILEPATH[] = "assets/levelA_theme.mp3", //From https://opengameart.org/content/underwater-theme-ii

BUBBLE_SFX_FILEPATH[] = "assets/bubbles.wav"; //From https://opengameart.org/content/bubble-pop

int LevelA_current_enemies = 0;
float LevelA_timer = 0.0f;

unsigned int LEVEL_A_DATA[] =
{
    36, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 49,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,

    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,

    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,

    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    55, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  55,
    18, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 87,
};


LevelA::~LevelA()
{
    
    delete    m_game_state.player;
    delete m_game_state.bubble;

    for (int i = 0; i < m_number_of_enemies; i++)
    {
        delete m_game_state.enemies[i];
    }
    delete    m_game_state.map;

	Mix_FreeChunk(m_game_state.bubble_sfx);
    Mix_FreeMusic(m_game_state.bgm);
    
}

void LevelA::initialise()
{
    
    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_A_DATA, map_texture_id, 1.0f, 6, 14);

    // ————— GEORGE SET-UP ————— //

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
	GLuint shrimp_texture_id = Utility::load_texture(SHRIMP_FILEPATH);
    GLuint bubble_texture_id = Utility::load_texture(BUBBLE_FILEPATH);

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    std::vector<std::vector <int> > walking_animation =
    {
        { 0, 1, 2, 3 },  // to move to the left,
        { 4, 5, 6, 7 } // to move to the right,
    };

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        2.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // attack damage
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

    m_game_state.bubble = new Entity(bubble_texture_id, m_game_state.player->get_projectile_speed(), m_game_state.player->get_pierce(), m_game_state.player->get_damage(), 0.0f, 0.5f, 0.4f, PROJECTILE, BUBBLE, IDLE);
    m_game_state.bubble->set_position(m_game_state.player->get_position());
    m_game_state.bubble->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.bubble->set_scale(glm::vec3(.50f, .4f, 1.0f));
    m_game_state.bubble->set_rate(m_game_state.player->get_rate());
    attack_rate = m_game_state.bubble->get_rate();
    m_game_state.bubble->set_timer(m_game_state.bubble->get_rate());


    int random_pos;
	for (int i = 0; i < m_number_of_enemies; i++)
	{
		random_pos = rand() % 76; //picks a random position for the enemy to spawn

		m_game_state.enemies.push_back(new Entity(shrimp_texture_id, 1.0f, 10.0f, 25.0f, 0.0f, 0.25f, 0.25f, ENEMY, FOLLOWER, IDLE));
		m_game_state.enemies[i]->set_position(enemy_positions[random_pos]);
        m_game_state.enemies[i]->set_acceleration(glm::vec3(0.0f,0.0f,0.0f));
		m_game_state.enemies[i]->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
        m_game_state.enemies[i]->set_exp(50);

        m_game_state.enemies[i]->set_animation_cols(8);
        m_game_state.enemies[i]->set_animation_rows(1);
        m_game_state.enemies[i]->set_animation_frames(4);
        m_game_state.enemies[i]->set_animation_index(0);
        m_game_state.enemies[i]->set_animation_time(0.0f);
        m_game_state.enemies[i]->set_walking(walking_animation);
	}

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 8.0f);
    Mix_PlayMusic(m_game_state.bgm, -1);

	m_game_state.bubble_sfx = Mix_LoadWAV(BUBBLE_SFX_FILEPATH);
    Mix_VolumeChunk(m_game_state.bubble_sfx, MIX_MAX_VOLUME / 2.0f);

}

void LevelA::update(float delta_time)
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

        if(m_game_state.player->get_kills() >= m_number_of_enemies)
		{
			clear = true;
			return;
		}   

        LevelA_timer += delta_time;
        if (LevelA_timer >= 1.0f)
        {
            if (LevelA_current_enemies < m_number_of_enemies)
            {
                LevelA_current_enemies++;
            }
            LevelA_timer = 0.0f;
        }
        attack_rate -= delta_time;
        if (m_game_state.player->get_is_active())
        {
            m_game_state.player->update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);

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
                    for (int i = 0; i < LevelA_current_enemies; i++)
                    {
                        if (m_game_state.enemies[i]->get_collided_projectile())
                        {
                            m_game_state.enemies[i]->change_projectile();
                        }
                    }
                }
            }

            m_game_state.bubble->ai_bubble(mouse_position);
            m_game_state.bubble->update(delta_time, m_game_state.player, NULL, 0 , m_game_state.map);


            for (int i = 0; i < LevelA_current_enemies; i++)
            {
                m_game_state.player->check_collision_x(m_game_state.enemies[i], 1);
                m_game_state.player->check_collision_y(m_game_state.enemies[i], 1);
                m_game_state.bubble->check_collision_x(m_game_state.enemies[i], 1);
                m_game_state.bubble->check_collision_y(m_game_state.enemies[i], 1);
            }
        }

        for (int i = 0; i < LevelA_current_enemies; i++)
        {
            m_game_state.enemies[i]->update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
        }
    }
    
}


void LevelA::render(ShaderProgram* g_shader_program)
{
    
    //draws collidable entities if they are active
    if (m_game_state.player->get_is_active())
    {
        m_game_state.player->render(g_shader_program);
    }

    for (int i = 0; i < LevelA_current_enemies; i++)
    {
        if (m_game_state.enemies[i]->get_is_active())
        {
            m_game_state.enemies[i]->render(g_shader_program);
        }
    }

    if (m_game_state.bubble->get_is_active())
    {
        m_game_state.bubble->render(g_shader_program);
    }


    m_game_state.map->render(g_shader_program);
    
}

void LevelA::kill()
{
    clear = true;
}

void LevelA::turn_on()
{
    

    for (int i = 0; i < m_number_of_enemies; i++)
    {
        m_game_state.enemies[i]->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
        m_game_state.enemies[i]->set_ai_type(FOLLOWER);
        m_game_state.enemies[i]->unpause();
    }

	m_game_state.player->set_entity_type(PLAYER);
    m_game_state.player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.player->unpause();
	m_game_state.bubble->set_ai_type(BUBBLE);
    m_game_state.bubble->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
	m_game_state.bubble->unpause();
    level_on = true;
    

}
void LevelA::turn_off()
{
    
    level_on = false;
    for (int i = 0; i < m_number_of_enemies; i++)
    {
        m_game_state.enemies[i]->set_movement(glm::vec3(0.0f,0.0f,0.0f));
		m_game_state.enemies[i]->set_ai_type(PAUSED);
        m_game_state.enemies[i]->pause();
    }

    m_game_state.player->set_entity_type(PLATFORM);
    m_game_state.player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
	m_game_state.player->pause();
    m_game_state.bubble->set_ai_type(PAUSED);
	m_game_state.bubble->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
	m_game_state.bubble->pause();
    
}

void LevelA::level_clear()
{
    turn_off();
}