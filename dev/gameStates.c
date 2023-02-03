#include "gameStates.h"
#include "utils.h"


//Carrega bibliotecas do allegro
//Carrega arquivos do jogo
void load_game(){
  //Instaladores
  must_init(al_init(), "allegro");                        //Allegro
  must_init(al_init_primitives_addon(), "primitives");    //Primitivas
  must_init(al_init_image_addon(), "image");              //Imagens



}

//Carrega menu interativo
void game_menu(){

}

//Carrega o jogo
void game_play(){

}

//Tela de game over
void game_over(){

}

//Destroi tudo
void destroy_game(){

}
