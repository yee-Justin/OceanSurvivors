#include "Start.h"
#include "Utility.h"

constexpr char
TEXT_FILEPATH[] = "assets/font1.png",

BGM_FILEPATH[] = "assets/menu_theme.mp3", //From https://opengameart.org/content/underwater-theme-1
MAP_TILESET_FILEPATH[] = "assets/map.png", //From https://opengameart.org/content/underwater-tileset

MENU_FILEPATH[] = "assets/menu.png"; //From https://opengameart.org/content/deep-ocean-battlemap

unsigned int START_DATA[] =
{
    0
};

Start::~Start()
{
    Mix_FreeMusic(m_game_state.bgm);
    delete    m_game_state.map;
    delete    m_game_state.menu;
}

void Start::initialise()
{
    GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
    m_game_state.map = new Map(1, 1, START_DATA, map_texture_id, 1.0f, 6, 14);

    GLuint menu_texture_id = Utility::load_texture(MENU_FILEPATH);
    m_game_state.menu = new Entity(menu_texture_id, 0.0f, 1, 1, OBJECT);
	m_game_state.menu->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
	m_game_state.menu->set_scale(glm::vec3(50.0f, 50.0f, 1.0f));

    text_texture_id = Utility::load_texture(TEXT_FILEPATH);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2.0f);
}

void Start::update(float delta_time)
{
	m_game_state.menu->update(delta_time, NULL, NULL, 0, m_game_state.map);

}


void Start::render(ShaderProgram* g_shader_program)
{
	m_game_state.map->render(g_shader_program);
    m_game_state.menu->render(g_shader_program);
    //draws "menu"
        Utility::draw_text(g_shader_program, text_texture_id, "Press Enter!", .65, .01, glm::vec3(-3.5,0,0));
}


void Start::kill() {}
void Start::level_clear() {}
