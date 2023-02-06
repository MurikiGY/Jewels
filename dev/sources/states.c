#include "utils.h"
#include "states.h"
#include "allegroEngine.h"
#include "libGame.h"
#include "boardOperators.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/keycodes.h>

//Carrega bibliotecas do allegro
//Carrega arquivos do jogo
void load_game(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set){
  //Instaladores
  must_init(al_init(), "allegro");                            //Allegro
  must_init(al_init_primitives_addon(), "primitives");        //Primitivas
  must_init(al_init_image_addon(), "image");                  //Imagens
  must_init(al_install_audio(), "Audio");                     //Instala audio
  must_init(al_init_acodec_addon(), "Acodec");                //Acodec de audio
  must_init(al_reserve_samples(128), "al_reserve_samples");   //config de audio
  must_init(al_init_font_addon(), "Fonte");                   //Fonte
  must_init(al_init_ttf_addon(), "ttf");                      //ttf

  //Configura variaveis do allegro
  al_engine->timer = al_create_timer(1.0 / FRAMES_N); must_init(al_engine->timer, "Timer");
  al_engine->queue = al_create_event_queue();         must_init(al_engine->queue, "Queue");
  display_init(&al_engine->buffer, &al_engine->display);
  keyboard_init(al_engine->key);
  al_engine->mouse = mouse_init();

  //Provavelmente vai dar merda 
  //Configura variaveis do jogo
  audio_init(&game_set->audio);
  font_init(&game_set->font);
  background_init(&game_set->bg);
  stars_init(game_set->stars);
  score_init(&game_set->score);
  mission_init(&game_set->mission);
  game_set->board = board_init(game_set->piece_sprite);
  state_init(&game_set->global_state);

  al_set_audio_stream_playmode(game_set->audio->bg_music, ALLEGRO_PLAYMODE_LOOP);
  al_attach_audio_stream_to_mixer(game_set->audio->bg_music, al_get_default_mixer());

  //Registradores de evento
  al_register_event_source(al_engine->queue, al_get_keyboard_event_source());
  al_register_event_source(al_engine->queue, al_get_mouse_event_source());
  al_register_event_source(al_engine->queue, al_get_timer_event_source(al_engine->timer));
  al_register_event_source(al_engine->queue, al_get_display_event_source(al_engine->display));
 
  //Inicia board consistente
  int ok = jewel_fall(game_set, 0);
  while ( ok )
    ok = jewel_fall(game_set, 0);
  game_set->score->local_score = 0;
  game_set->mission->level = 0;
  game_set->mission->quant = 0;

  //Muda para o menu
  *game_status = GAME_MENU;
}

//Carrega menu interativo
void game_menu(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set){
  bool done = false;    //Fim de jogo
  bool redraw = true;   //Renderizar
  ALLEGRO_EVENT event;
  al_start_timer(al_engine->timer);
  while(1){
    al_wait_for_event(al_engine->queue, &event);
    switch ( event.type ){
      case ALLEGRO_EVENT_TIMER:
        //Update functions
        stars_update(game_set->stars);
        //Se new_game
        if ( al_engine->mouse->x_clk > BUFFER_W/4 && al_engine->mouse->x_clk < 3*BUFFER_W/4 &&
             al_engine->mouse->y_clk > 380  && al_engine->mouse->y_clk < 440 ){
          *game_status = GAME_PLAY;
          gen_new_board(game_set);
          game_set->score->local_score = 0;
          game_set->mission->level = 0;
          game_set->mission->quant = 0;
          game_set->mission->type = between(0, JEWEL_TYPE_N);
          done = true; }
        //Se continue
        if ( al_engine->mouse->x_clk > BUFFER_W/4 && al_engine->mouse->x_clk < 3*BUFFER_W/4 &&
             al_engine->mouse->y_clk > 500  && al_engine->mouse->y_clk < 560 ){
          *game_status = GAME_PLAY;
          done = true; }
        //if ( al_engine->mouse->x > BUFFER_W/4 && al_engine->mouse->x < 3*BUFFER_W/4 &&
        //     al_engine->mouse->y > 380  && al_engine->mouse->y < 440 ){
        //  al_play_sample(game_set->audio->menu_button_effect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0); }
        //if ( al_engine->mouse->x > BUFFER_W/4 && al_engine->mouse->x < 3*BUFFER_W/4 &&
        //     al_engine->mouse->y > 500  && al_engine->mouse->y < 560 ){
        //  al_play_sample(game_set->audio->menu_button_effect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0); }

        redraw = true;
        break;

      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        *game_status = GAME_OVER;
        done = true;
        break;
    } //Switch ( event.type )

    if ( done )
      break;

    keyboard_update(&event, al_engine->key);
    mouse_update(&event, al_engine->mouse);

    if( redraw && al_is_event_queue_empty(al_engine->queue) ){
      display_pre_draw(&al_engine->buffer);

      //Draw functions
      background_draw(game_set->bg);
      stars_draw(game_set->stars);
      al_draw_filled_rounded_rectangle(BUFFER_W/4.0, 380, 3*BUFFER_W/4.0, 440, 20, 20, al_map_rgb(153, 76, 0)); //New game
      al_draw_rounded_rectangle(BUFFER_W/4.0, 380, 3*BUFFER_W/4.0, 440, 20, 20, al_map_rgb(64, 64, 64), 5); //New game
      al_draw_filled_rounded_rectangle(BUFFER_W/4.0, 500, 3*BUFFER_W/4.0, 560, 20, 20, al_map_rgb(153, 76, 0)); //Continue
      al_draw_rounded_rectangle(BUFFER_W/4.0, 500, 3*BUFFER_W/4.0, 560, 20, 20, al_map_rgb(64, 64, 64), 5); //Continue
      al_draw_text(game_set->font->title_font, al_map_rgb(255, 255, 255), DISP_W/2.0, 175, ALLEGRO_ALIGN_CENTER, "ROCK FALL");
      al_draw_text(game_set->font->score_font, al_map_rgb(255, 255, 255), DISP_W/2.0, 395, ALLEGRO_ALIGN_CENTER, "NEW GAME");
      al_draw_text(game_set->font->score_font, al_map_rgb(255, 255, 255), DISP_W/2.0, 515, ALLEGRO_ALIGN_CENTER, "CONTINUE");


      display_post_draw(&al_engine->buffer, &al_engine->display);
      redraw = false;
    } //If (done)
  } //While (1)
  
  //Limpa sujeira do teclado
  memset(al_engine->key, 0, sizeof(unsigned char) * ALLEGRO_KEY_MAX);
}

//Carrega o jogo
void game_play(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set){
  bool done = false;    //Fim de jogo
  bool redraw = true;   //Renderizar
  ALLEGRO_EVENT event;

  while(1){
    al_wait_for_event(al_engine->queue, &event);
    switch ( event.type ){
      case ALLEGRO_EVENT_TIMER:
        //Update functions
        stars_update(game_set->stars);
        board_update(game_status, al_engine, game_set);
        //Back button
        if ( al_engine->mouse->x_clk > 10 && al_engine->mouse->x_clk < 75 &&
             al_engine->mouse->y_clk > 25 && al_engine->mouse->y_clk < 65 ){
          *game_status = GAME_MENU;
          done = true; }

        //ESC
        if ( al_engine->key[ALLEGRO_KEY_ESCAPE] ){
          al_engine->mouse->x_clk = -1; al_engine->mouse->y_clk = -1;
          *game_status = GAME_MENU;
          done = true; }

        redraw = true;
        break;

      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        *game_status = GAME_OVER;
        done = true;
      break;
    } //Switch ( event.type )

    if ( done )
      break;

    keyboard_update(&event, al_engine->key);
    mouse_update(&event, al_engine->mouse);

    if( redraw && al_is_event_queue_empty(al_engine->queue) ){
      display_pre_draw(&al_engine->buffer);

      //Draw functions
      background_draw(game_set->bg);
      stars_draw(game_set->stars);
      score_draw(game_set->score, game_set->font->score_font);
      mission_draw(game_set->mission, game_set->font->score_font, game_set->piece_sprite);
      board_draw(game_set->board, game_set->piece_sprite);
      //Back button
      al_draw_filled_rounded_rectangle(30, 37, 75, 53, 5, 5, al_map_rgb(204, 102, 0));
      al_draw_filled_triangle(10, 45, 35, 25, 35, 65, al_map_rgb(204, 102, 0));


      display_post_draw(&al_engine->buffer, &al_engine->display); redraw = false;
    } //If (done)
  } //While (1)

  //Limpa sujeira do teclado
  memset(al_engine->key, 0, sizeof(unsigned char) * ALLEGRO_KEY_MAX);
}

//Tela de game over
void game_help(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set){
  //Muda para destroy_game
}

//Destroi tudo
void game_over(GAME_STATE *game_status, ALLEGRO_ENGINE *al_engine, GAME_ENGINE *game_set){
  //Destroi game_set
  audio_deinit(&game_set->audio);
  font_deinit(&game_set->font);
  score_deinit(&game_set->score);
  mission_deinit(&game_set->mission);
  background_deinit(&game_set->bg);
  board_deinit(&game_set->board, game_set->piece_sprite);

  //Destroi al_engine
  mouse_deinit(&al_engine->mouse);
  display_deinit(&al_engine->buffer, &al_engine->display);
  al_destroy_timer(al_engine->timer);
  al_destroy_event_queue(al_engine->queue);

  //Muda para finish
  *game_status = FINISH;
}
