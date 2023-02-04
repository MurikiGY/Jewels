#include "states.h"
#include "allegroEngine.h"
#include "utils.h"

//Carrega bibliotecas do allegro
//Carrega arquivos do jogo
void load_game(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set){
  //Instaladores
  must_init(al_init(), "allegro");                        //Allegro
  must_init(al_init_primitives_addon(), "primitives");    //Primitivas
  must_init(al_init_image_addon(), "image");              //Imagens
  al_install_audio();                                     //Instala audio
  al_init_acodec_addon();                                 //Acodec de audio
  al_reserve_samples(128);                                //config de audio
  al_init_font_addon();                                   //Fonte
  al_init_ttf_addon();                                    //ttf

  //Configura variaveis do allegro
  al_engine->timer = al_create_timer(1.0 / FRAMES_N); must_init(al_engine->timer, "Timer");
  al_engine->queue = al_create_event_queue();         must_init(al_engine->queue, "Queue");
  display_init(&al_engine->buffer, &al_engine->display);
  keyboard_init(al_engine->key);
  al_engine->mouse = mouse_init();

  //Configura variaveis do jogo
  audio_init(game_set->audio);
  

}

//Carrega menu interativo
void game_menu(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set){

}

//Carrega o jogo
void game_play(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set){

}

//Tela de game over
void game_over(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set){

}

//Destroi tudo
void destroy_game(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set){

}
