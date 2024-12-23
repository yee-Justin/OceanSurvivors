﻿#include "LevelC.h"
#include "Utility.h"


constexpr char

//I made these
SPRITESHEET_FILEPATH[] = "assets/whale.png",
SHRIMP_FILEPATH[] = "assets/shrimp.png",
BUBBLE_FILEPATH[] = "assets/bubble.png",
CLAM_FILEPATH[] = "assets/clam.png",
CLAMPROJECTILE_FILEPATH[] = "assets/clamprojectile.png",
BOSS_FILEPATH[] = "assets/boss.png",


MAP_TILESET_FILEPATH[] = "assets/tileset.png", //From https://axulart.itch.io/axularts-beach-and-caves-tileset

BGM_FILEPATH[] = "assets/LevelC_theme.mp3", //From Incompetech, "Mountain Emperor"

BOSSBGM_FILEPATH[] = "assets/boss.mp3", //From https://opengameart.org/content/walking-with-poseidon

BUBBLE_SFX_FILEPATH[] = "assets/bubbles.wav"; //From https://opengameart.org/content/bubble-pop

int LevelC_current_enemies = 0;
float LevelC_timer = 0.0f;
float boss_timer = 0.0f;

bool boss_time = false;
bool boss_music = false;

unsigned int LEVEL_C_DATA[] =
{
    48, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 50,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  110,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  110,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  52,  0,  0,  0,  0,  0,  53,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  53,  0,  0,  0,  0,  0,  52, 0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  110,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  110,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    108, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  109,
    78, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 80,
};

LevelC::~LevelC()
{

    delete m_game_state.player;
    delete m_game_state.bubble;
    delete m_game_state.boss;
    for (int i = 0; i < m_number_of_enemies; i++)
    {
        delete m_game_state.enemies[i];
    }

    for (int i = 0; i < m_number_of_enemies; i++)
    {
        delete m_game_state.projectiles[i];
    }

    delete m_game_state.map;

    Mix_FreeChunk(m_game_state.bubble_sfx);

    Mix_FreeMusic(m_game_state.bgm);

    Mix_FreeMusic(m_game_state.boss_bgm);

}

void LevelC::initialise()
{

    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_C_DATA, map_texture_id, 1.0f, 15, 8);

    // ————— ENTITY SET-UP ————— //

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    GLuint shrimp_texture_id = Utility::load_texture(SHRIMP_FILEPATH);
    GLuint clam_texture_id = Utility::load_texture(CLAM_FILEPATH);
    GLuint bubble_texture_id = Utility::load_texture(BUBBLE_FILEPATH);
    GLuint clamprojectile_id = Utility::load_texture(CLAMPROJECTILE_FILEPATH);
	GLuint boss_texture_id = Utility::load_texture(BOSS_FILEPATH);

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


    //generates either a clam or a shrimp at random positions
    int random_pos;
    int random_enemy;
    for (int i = 0; i < m_number_of_enemies; i++)
    {
        random_pos = rand() % 76; //picks a random position for the enemy to spawn
        random_enemy = rand() % 2; //picks a random enemy to spawn

        if (random_enemy == 0)
        {
            m_game_state.enemies.push_back(new Entity(shrimp_texture_id, 1.50f, 25.0f, 20.0f, 0.0f, 0.25f, 0.25f, ENEMY, FOLLOWER, IDLE));
            m_game_state.enemies[i]->set_position(enemy_positions[random_pos]);
            m_game_state.enemies[i]->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
            m_game_state.enemies[i]->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
            m_game_state.enemies[i]->set_exp(50);

            m_game_state.enemies[i]->set_animation_cols(8);
            m_game_state.enemies[i]->set_animation_rows(1);
            m_game_state.enemies[i]->set_animation_frames(4);
            m_game_state.enemies[i]->set_animation_index(0);
            m_game_state.enemies[i]->set_animation_time(0.0f);
            m_game_state.enemies[i]->set_walking(walking_animation);
        }
        if (random_enemy == 1)
        {
            m_game_state.enemies.push_back(new Entity(clam_texture_id, 1.50f, 20.0f, 10.0f, 2.0f, 0.5f, 0.5f, ENEMY, CLAM, WALKING));
            m_game_state.enemies[i]->set_position(enemy_positions[random_pos]);
            m_game_state.enemies[i]->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
            m_game_state.enemies[i]->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
            m_game_state.enemies[i]->set_exp(50);

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
        }

        //enemies still each get their own projectile so we can match the projectile to the enemy type that will be shooting
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

    //initializes boss rather far away
    m_game_state.boss = new Entity(boss_texture_id, 3.0f, 500.0f, 30.0f, 2.0f, 3.0f, 2.0f, ENEMY, PAUSED, IDLE);
    m_game_state.boss->set_position(glm::vec3(-20.0f, -10.0f, 0.0f));
    m_game_state.boss->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.boss->set_scale(glm::vec3(7.0f, 7.0f, 1.0f));
    m_game_state.boss->set_exp(0);
    m_game_state.boss->set_animation_cols(8);
    m_game_state.boss->set_animation_rows(1);
    m_game_state.boss->set_animation_frames(4);
    m_game_state.boss->set_animation_index(0);
    m_game_state.boss->set_animation_time(0.0f);
    m_game_state.boss->set_walking(walking_animation);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
	m_game_state.boss_bgm = Mix_LoadMUS(BOSSBGM_FILEPATH);

    Mix_VolumeMusic(MIX_MAX_VOLUME / 4.0f);
    Mix_PlayMusic(m_game_state.bgm, -1);

    m_game_state.bubble_sfx = Mix_LoadWAV(BUBBLE_SFX_FILEPATH);

    Mix_VolumeChunk(m_game_state.bubble_sfx, MIX_MAX_VOLUME / 2.0f);

    turn_off();

}

void LevelC::update(float delta_time)
{

    if (!level_on)
    {
        return;
    }
    else
    {
		//if the boss dies, stop the music and pause the game
        if (!m_game_state.boss->get_is_active())
        {
            win = true;
			Mix_HaltMusic();
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

        //increments player kills/exp based on bubble kills/exp and sets bubble kills/exp to 0
        m_game_state.player->add_kills(m_game_state.bubble->get_kills());
        m_game_state.bubble->set_kills(0);
        m_game_state.player->add_exp(m_game_state.bubble->get_current_exp());
        m_game_state.bubble->set_current_exp(0);

        LevelC_timer += delta_time;
        boss_timer += delta_time;

        //allows more enemies to spawn every second
        if (LevelC_timer >= 1.0f)
        {
            if (LevelC_current_enemies < m_number_of_enemies)
            {
                LevelC_current_enemies++;
            }
            LevelC_timer = 0.0f;
        }

        //spawns the boss after 30 seconds and plays boss music
        if (boss_timer >= 30.0f && !boss_music)
        {
            boss_time = true;
			m_game_state.boss->set_ai_type(BOSS);
            Mix_HaltMusic();
            Mix_PlayMusic(m_game_state.boss_bgm, -1);
			boss_music = true;
        }

        attack_rate -= delta_time;
        enemy_attack_rate -= delta_time;

        if (m_game_state.player->get_is_active())
        {
            m_game_state.player->update(delta_time, m_game_state.player, NULL, 0,
                m_game_state.map);


            //bubble logic
            if (attack_rate <= 0.0f)
            {
                //shoots at intervals and resets if running out of pierce, of projectile time, or collision with map wall
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
                    for (int i = 0; i < LevelC_current_enemies; i++)
                    {
                        if (m_game_state.enemies[i]->get_collided_projectile())
                        {
                            m_game_state.enemies[i]->change_projectile();
                        }
                    }
                    if (m_game_state.boss->get_collided_projectile())
                    {
                        m_game_state.boss->change_projectile();
                    }
                }
            }


            //enemy projectile logic, only clams shoot
            if (enemy_attack_rate <= 0.0f)
            {
                //shoot at the last known position of the player at set intervals
                player_position = m_game_state.player->get_position();
                for (int i = 0; i < LevelC_current_enemies; i++)
                {
                    //only shoots if the clam is alive
                    if (m_game_state.enemies[i]->get_is_active())
                    {
                        if (m_game_state.enemies[i]->get_ai_type() == CLAM && m_game_state.enemies[i]->get_ai_state() == IDLE && !m_game_state.projectiles[i]->get_is_active())
                        {
                            m_game_state.projectiles[i]->set_position(m_game_state.enemies[i]->get_position());
                            m_game_state.projectiles[i]->set_health(m_game_state.enemies[i]->get_pierce());
                            m_game_state.projectiles[i]->set_damage(m_game_state.enemies[i]->get_damage());
                            m_game_state.projectiles[i]->set_rate(m_game_state.enemies[i]->get_rate());
                            m_game_state.projectiles[i]->set_timer(m_game_state.enemies[i]->get_timer());
                            m_game_state.projectiles[i]->set_speed(m_game_state.enemies[i]->get_projectile_speed());
                            m_game_state.projectiles[i]->activate();
                            
                            //resets player projectile collision so they can take damage again
                            if (m_game_state.player->get_collided_projectile())
                            {
                                m_game_state.player->change_projectile();
                            }
                        }
                    }
                }
                //resets enemy attack timer
                enemy_attack_rate = 2.5f;
            }


            //updates bubble
            m_game_state.bubble->ai_bubble(mouse_position);
            m_game_state.bubble->update(delta_time, m_game_state.player, NULL, 0,
                m_game_state.map);


            //checks for collision from the player and bubble side
            for (int i = 0; i < LevelC_current_enemies; i++)
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

            m_game_state.player->check_collision_x(m_game_state.boss, 1);
            m_game_state.player->check_collision_y(m_game_state.boss, 1);
            m_game_state.bubble->check_collision_x(m_game_state.boss, 1);
            m_game_state.bubble->check_collision_y(m_game_state.boss, 1);
        }


        //moves enemies and projectiles
        m_game_state.boss->update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
        for (int i = 0; i < LevelC_current_enemies; i++)
        {
            m_game_state.enemies[i]->update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
            m_game_state.projectiles[i]->ai_enemy_projectile(player_position);
            m_game_state.projectiles[i]->update(delta_time, NULL, NULL, 0, m_game_state.map);
        }
    }

}


void LevelC::render(ShaderProgram* g_shader_program)
{

    //draws collidable entities if they are active
    if (m_game_state.player->get_is_active())
    {
        m_game_state.player->render(g_shader_program);
    }

    for (int i = 0; i < LevelC_current_enemies; i++)
    {
        if (m_game_state.enemies[i]->get_is_active())
        {
            m_game_state.enemies[i]->render(g_shader_program);
        }
    }


    for (int i = 0; i < LevelC_current_enemies; i++)
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

	if (m_game_state.boss->get_is_active() && boss_time)
	{
		m_game_state.boss->render(g_shader_program);
	}


    m_game_state.map->render(g_shader_program);

}

//kills the boss to instant win
void LevelC::kill()
{
    m_game_state.boss->deactivate();
}

//sets all enemies to their normal state
void LevelC::turn_on()
{

    for (int i = 0; i < m_number_of_enemies; i++)
    {
        m_game_state.enemies[i]->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
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
	
    //if the boss has not "spawned" yet, do not make it move
    if (boss_time)
    {
        m_game_state.boss->set_ai_type(BOSS);
    }
    m_game_state.boss->unpause();

    level_on = true;
}

//sets everything to not move
void LevelC::turn_off()
{

    level_on = false;
    for (int i = 0; i < m_number_of_enemies; i++)
    {
        m_game_state.enemies[i]->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
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
	m_game_state.boss->set_ai_type(PAUSED);
	m_game_state.boss->pause();

}