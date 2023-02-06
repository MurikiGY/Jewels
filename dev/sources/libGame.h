//Arquivo de inicialização das estruturas do jogo

#ifndef _LIBGAME_
#define _LIBGAME_

#include "allegroEngine.h"
#include "utils.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/bitmap.h>

// --- AUDIO ---
typedef struct audio {
  ALLEGRO_AUDIO_STREAM    *bg_music;              //Musica de fundo
  ALLEGRO_SAMPLE          *fall_snd_effect;       //Efeito da queda
  ALLEGRO_SAMPLE          *special1_snd_effect;   //Explosão do especial 1
  ALLEGRO_SAMPLE          *special2_snd_effect;   //Explosão do especial 2
  ALLEGRO_SAMPLE          *level_up_sound;
} AUDIO_T;


// --- FONTE ---
typedef struct font {
  ALLEGRO_FONT            *score_font;
  ALLEGRO_FONT            *title_font;
  ALLEGRO_FONT            *help_font;
} FONT_T;


// --- BACKGROUND ---


// --- STARS ---
typedef struct STAR {
  float y;
  float speed;
} STAR;
#define STARS_N ((BUFFER_W / 2) - 1)


// --- SCORE ---
typedef struct SCORE {
  int   local_score;                //Score
  int   global_score;               //Global score
} SCORE;


// --- MISSION/LEVEL ---
typedef struct mission {
  int type;                         //Type of the mission
  int quant;                        //Quantity
  int level;                        //Level
} MISSION;


// --- BOARD ---
typedef struct jewel {
  int   x, y;                       //Coordenadas
  int   type;                       //Tipo do doce
  int   draw;                       //Booleano de renderizar
  int   special_gen_flag;           //Flag para saber se acabou de virar um especial
} JEWEL;
// Board_x [-- 80 (offset) -- : -- 560 (board) -- : -- 80 (offset) --]
// Board_y [-- 140 (score) -- : -- 560 (board) -- : -- 20 (offset) --]
//        [       70 (jewel) :                                      ]
#define X_OFFSET      80            //Limites do tabuleiro
#define Y_OFFSET      140           //Espaçamento do score
#define JEWEL_SIZE    70            //Tamanho ocupado pelo doce
#define BOARD_N       8             //Tamanho da matriz
#define JEWEL_TYPE_N  6             //Tipos diferentes de doces
#define FALL_SPEED    5             //Velocidade de queda das peças


// --- Maquina de estados ---
typedef enum state_board {          //Estados do board
  BOARD_NEW_PLAY = 0,
  BOARD_SWITCH_JEWEL,
  BOARD_JEWEL_FALL
} STATE_BOARD;
typedef enum state_jewel {          //Estados da troca de peças
  JEWEL_GO = 0,
  JEWEL_BACK
} STATE_JEWEL;
typedef enum state_fall {           //Estado da queda de peças
  TEST_FALL = 0,
  RENDER_FALL
} STATE_FALL;
typedef struct states {
  STATE_BOARD board_state;          //Maquina de estado do tabuleiro
  STATE_JEWEL jewel_state;          //Maquina de estado da joia
  STATE_FALL  fall_state;           //Maquina de estado da queda do tabuleiro
  int x_jewel_clk, y_jewel_clk;     //Coordenadas da joia clicada
  int x_jewel_rls, y_jewel_rls;     //Coordenadas da joia solta
  int i_jewel_fall;                 //coordenadas da queda
  int fall_flag;
} STATES;


// Conjunto de Variaveis do jogo
typedef struct engine_game {
  AUDIO_T                 *audio;                 //audios
  FONT_T                  *font;                  //Fonte
  ALLEGRO_BITMAP          *bg;                    //imagem de fundo
  STAR                    stars[STARS_N];         //Estrelas
  SCORE                   *score;                 //Pontuacao
  MISSION                 *mission;               //Missão
  JEWEL                   **board;                //Tabuleiro 
  ALLEGRO_BITMAP          *piece_sprite[18];      //Sprites de peças
  STATES                  global_state;           //Variaveis de estado
} GAME_ENGINE;


// --- AUDIO ---
// Inicia variaveis de audio
void audio_init(AUDIO_T **audio);
// Destroi variaveis de audio
void audio_deinit(AUDIO_T **audio);


// --- FONTE ---
// Inicializa variavel de fonte
void font_init(FONT_T **font);
// Destroi variavel de fonte
void font_deinit(FONT_T **font);


// --- background ---
// Inicia background
void background_init(ALLEGRO_BITMAP **bg);
// Destroi background
void background_deinit(ALLEGRO_BITMAP **bg);
// Desenha background
void background_draw(ALLEGRO_BITMAP *bg);


// --- STARS ---
// Inicia estrelas
void stars_init(STAR stars[]);
// Destroi estrelas
void stars_update(STAR stars[]);
// Renderiza estrelas
void stars_draw(STAR stars[]);


// --- SCORE ---
// Inicia score
void score_init(SCORE **score);
// Destroi score
void score_deinit(SCORE **score);
// Renderiza score
void score_draw (SCORE *score, ALLEGRO_FONT *font);


// --- MISSION ---
// Inicia missão
void mission_init(MISSION **mission);
// Destroi missão
void mission_deinit(MISSION **mission);
// Renderiz missão
void mission_draw(MISSION *mission, ALLEGRO_FONT *font, ALLEGRO_BITMAP **piece_sprite);


// --- BOARD ---
// Inicia board
JEWEL **board_init (ALLEGRO_BITMAP **candy_sprite);
// Destroi board
void board_deinit(JEWEL ***board, ALLEGRO_BITMAP **candy_sprite);
// Renderiza board
void board_draw(JEWEL **board, ALLEGRO_BITMAP **piece_sprite);


// --- Maquina de estados ---
// Inicia maquina de estados
void state_init(STATES *global_states);


#endif
