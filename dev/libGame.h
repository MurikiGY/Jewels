//Arquivo de inicialização das estruturas do jogo

#ifndef _LIBGAME_
#define _LIBGAME_

#include "allegroEngine.h"
#include "utils.h"
#include <allegro5/allegro_font.h>

// --- AUDIO ---
typedef struct audio {
  ALLEGRO_AUDIO_STREAM    *bg_music;              //Musica de fundo
  ALLEGRO_SAMPLE          *fall_snd_effect;       //Efeito da queda
  ALLEGRO_SAMPLE          *special1_snd_effect;   //Explosão do especial 1
  ALLEGRO_SAMPLE          *special2_snd_effect;   //Explosão do especial 2
} AUDIO_T;

// --- FONTE ---
typedef struct font {
  ALLEGRO_FONT            *score_font;
} FONT_T;

// --- STARS ---
typedef struct STAR {
  float y;
  float speed;
} STAR;

#define STARS_N ((BUFFER_W / 2) - 1)                //Numero de estrelas

// --- SCORE ---
typedef struct SCORE {
  int   score;        char  str_score[20];          //Score
  int   global_score; char  str_global_score[20];   //Global score
  int   x_score, y_score;
  int   x_global, y_global;
} SCORE;

// --- BOARD ---
typedef struct jewel {
  int   x, y;                       //Coordenadas
  int   type;                       //Tipo do doce
  int   draw;                       //Booleano de renderizar
  int   special_gen_flag;           //Flag para saber se acabou de virar um especial
} JEWEL;

//Board_x [-- 80 (offset) -- : -- 560 (board) -- : -- 80 (offset) --]
//Board_y [-- 140 (score) -- : -- 560 (board) -- : -- 20 (offset) --]
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


//Conjunto de Variaveis do jogo
typedef struct engine_game {
  AUDIO_T                 *audio;
  FONT_T                  *font;                  //Fonte
  ALLEGRO_BITMAP          *background;            //imagem de fundo
  STAR                    stars[STARS_N];         //Estrelas
  SCORE                   *game_score;            //Pontuacao
  JEWEL                   **board;                //Tabuleiro 
  ALLEGRO_BITMAP          *piece_sprite[18];      //Sprites de peças
  STATES                  global_state;           //Variaveis de estado
} GAME_ENGINE;


// --- AUDIO ---
//Inicia variaveis de audio
void audio_init(AUDIO_T *audio);
//Destroi variaveis de audio
void audio_deinit(AUDIO_T *audio);


// --- FONTE ---
//Inicializa variavel de fonte
void font_init(FONT_T *font);
//Destroi variavel de fonte
void font_deinit(FONT_T *font);


// --- STARS ---
//Inicia estrelas
void stars_init(STAR stars[]);
//Destroi estrelas
void stars_update(STAR stars[]);
//Renderiza estrelas
void stars_draw(STAR stars[]);


// --- SCORE ---
//Inicia score
SCORE *score_init();
//Destroi score
void score_deinit(SCORE *game_score);
//Renderiza score
void score_draw (SCORE *game_score, ALLEGRO_FONT *font);


// --- Maquina de estados ---
//Inicia maquina de estados
void state_init(STATES *global_states);


// --- BOARD ---
//Inicia board
JEWEL **board_init (ALLEGRO_BITMAP **candy_sprite);
//Destroi board
void board_deinit(JEWEL ***board, ALLEGRO_BITMAP **candy_sprite);


#endif
