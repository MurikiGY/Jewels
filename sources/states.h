//Arquivo de maquina de estados para o loop principal

#ifndef _GAMESTATES_
#define _GAMESTATES_

#include "allegroEngine.h"
#include "libGame.h"

typedef enum state_game {
  LOAD_GAME = 0,
  GAME_MENU,
  GAME_PLAY,
  GAME_HELP,
  GAME_OVER,
  FINISH
} GAME_STATE;


//Carrega bibliotecas do allegro
//Carrega arquivos do jogo
void load_game(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set);
//Carega menu interativo
void game_menu(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set);
//Carrega o jogo
void game_play(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set);
//Tela de game over
void game_help(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set);
//Destroi estruturas do jogo
void game_over(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set);


#endif
