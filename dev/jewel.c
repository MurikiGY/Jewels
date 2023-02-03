#include "allegro_engine.h"
#include "gameStates.h"





int main (){
  srand( time(NULL) );

  //Variaveis do sistema allegro
  ALLEGRO_ENGINE al_engine;

  //Variaveis do jogo
  GAME_ENGINE game_set;

  GAME_STATE game_status = LOAD_GAME;

  while (1){
    switch ( game_status ){
      case LOAD_GAME:     load_game(al_engine, game_set); break;
      case GAME_MENU:     game_menu(al_engine, game_set); break;
      case GAME_PLAY:     game_play(al_engine, game_set); break;
      case GAME_OVER:     game_over(al_engine, game_set); break;
      case DESTROY_GAME:  destroy_game(al_engine, game_set); break;
      default: break;

  return 0;
}
