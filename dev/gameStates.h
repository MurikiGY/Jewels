#ifndef _GAMESTATES_
#define _GAMESTATES_

typedef enum state_game {
  LOAD_GAME = 0,
  GAME_MENU,
  GAME_PLAY,
  GAME_OVER,
  DESTROY_GAME
} GAME_STATE;


//Carrega bibliotecas do allegro
//Carrega arquivos do jogo
void load_game();

//Carega menu interativo
void game_menu();

//Carrega o jogo
void game_play();

//Tela de game over
void game_over();

//Destroi estruturas do jogo
void destroy_game();


#endif
