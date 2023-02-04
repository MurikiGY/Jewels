// Autor: Muriki Gusmão Yamanaka
// Arquivo de execução principal
#include "utils.h"
#include "allegroEngine.h"
#include "states.h"
#include "libGame.h"


int main (){
  srand( time(NULL) );

  ALLEGRO_ENGINE al_engine;               //Variaveis do sistema allegro
  GAME_ENGINE game_set;                   //Variaveis do jogo
  GAME_STATE game_status = LOAD_GAME;     //Variavel de estado principal

  while ( game_status != FINISH ){
    switch ( game_status ){
      case LOAD_GAME:     load_game(&game_status, &al_engine, &game_set); break;
      case GAME_MENU:     game_menu(&game_status, &al_engine, &game_set); break;
      case GAME_PLAY:     game_play(&game_status, &al_engine, &game_set); break;
      case GAME_OVER:     game_over(&game_status, &al_engine, &game_set); break;
      case DESTROY_GAME:  destroy_game(&game_status, &al_engine, &game_set); break;
      default: break;
    }
  }

  return 0;
}
