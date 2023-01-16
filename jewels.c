//Jewels by Muriki Gusmão Yamanaka

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/display.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/bitmap_io.h>
#include <allegro5/events.h>
#include <allegro5/mouse.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <unistd.h>

//Teste de inicialização
void must_init(bool test, const char *description){

  if(test) return;

  printf("couldn't initialize %s\n", description);
  exit(1);

}

//Inteiros aleatorios
int between(int lo, int hi){

  return lo + (rand() % (hi - lo));

}

//Float aleatorios
float between_f(float lo, float hi){

  return lo + ((float)rand() / (float)RAND_MAX) * (hi - lo);

}

// --- ALLEGRO SYSTEM ---

// --- DISPLAY ---

#define BUFFER_W 720
#define BUFFER_H 720
#define FRAMES_N 60.0

#define DISP_SCALE 1
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

//Inicia variaveis de buffer e display
void display_init(ALLEGRO_BITMAP **buff, ALLEGRO_DISPLAY **disp){

  al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

  *buff = al_create_bitmap(BUFFER_W, BUFFER_H);
  must_init(*buff, "Buffer");

  *disp = al_create_display(DISP_W, DISP_H);
  must_init(*disp, "Display");
  
}

//Destroy variaveis de buffer e display
void display_deinit(ALLEGRO_BITMAP **buff, ALLEGRO_DISPLAY **disp){

  al_destroy_bitmap(*buff);
  al_destroy_display(*disp);

}

//Buffer target
void display_pre_draw(ALLEGRO_BITMAP **buff){

  al_set_target_bitmap(*buff);

}

//Buffer commit
void display_post_draw(ALLEGRO_BITMAP **buff, ALLEGRO_DISPLAY **disp){

  al_set_target_backbuffer(*disp);
  al_draw_scaled_bitmap(*buff, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);

  al_flip_display();

}

// --- FONTE ---

//Inicializa fonte
void font_init(ALLEGRO_FONT **font){
  
  al_init_font_addon();
  al_init_ttf_addon();

  *font = al_load_font("resources/fonts/MASLITE.otf", 36, 0);
  must_init(*font, "Fonte MASLITE");

}

//Destroi variavel de fonte
void font_deinit(ALLEGRO_FONT **font){

  al_destroy_font(*font);

}

// --- AUDIO ---

//Inicia variaveis de audio
void audio_init(ALLEGRO_AUDIO_STREAM **bg_music){

  al_install_audio();
  al_init_acodec_addon();
  al_reserve_samples(128);

  *bg_music = al_load_audio_stream("resources/sound/music.opus", 2, 2048);
  must_init(*bg_music, "Background music");
  
  al_set_audio_stream_playmode(*bg_music, ALLEGRO_PLAYMODE_LOOP);
  al_attach_audio_stream_to_mixer(*bg_music, al_get_default_mixer());

}

void audio_deinit(ALLEGRO_AUDIO_STREAM **bg_music){

  al_destroy_audio_stream(*bg_music);

}

// --- KEYBOARD ---

#define KEY_SEEN     1
#define KEY_RELEASED 2

void keyboard_init(unsigned char key[]){

  memset(key, 0, sizeof(unsigned char));

}

void keyboard_update(ALLEGRO_EVENT* event, unsigned char key[]){

  switch(event->type){

    case ALLEGRO_EVENT_TIMER:
      for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
        key[i] &= KEY_SEEN;
      break;

    case ALLEGRO_EVENT_KEY_DOWN:
      key[event->keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
      break;

    case ALLEGRO_EVENT_KEY_UP:
      key[event->keyboard.keycode] &= KEY_RELEASED;
      break;
  }

}

// --- MOUSE ---

typedef struct mouse {
  int x;            //Posição de movimento
  int y;
  int x_click;      //Posição do click
  int y_click;
  int x_release;    //Posição do release
  int y_release;
} mouse_t;

void mouse_init()
{
  //al_hide_mouse_cursor(disp);
}

void mouse_update(ALLEGRO_EVENT* event, mouse_t mouse){

  switch(event->type){
    case ALLEGRO_EVENT_TIMER:
      //mouse.x_click = -1;
      //mouse.y_click = -1;
      mouse.x_release = -1;
      mouse.y_release = -1;
      break;

    case ALLEGRO_EVENT_MOUSE_AXES:
      mouse.x = event->mouse.x;
      mouse.y = event->mouse.y;
      break;

    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
      mouse.x_click = event->mouse.x;
      mouse.y_click = event->mouse.y;
      break;

    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
      mouse.x_release = event->mouse.x;
      mouse.y_release = event->mouse.y;
      break;
  }

}

// --- Configurações de tema e gameplay ---

// --- BACKGROUND ---

void background_init(ALLEGRO_BITMAP **bg){

  *bg = al_load_bitmap("resources/background/bg.jpg");
  must_init(*bg, "Background");

}

void background_deinit(ALLEGRO_BITMAP **bg){

  al_destroy_bitmap(*bg);

}

void background_draw(ALLEGRO_BITMAP **bg){

  al_draw_bitmap(*bg, 0, 0, 0);

}

// --- STARS ---

typedef struct STAR{
  float y;
  float speed;
} STAR;

#define STARS_N ((BUFFER_W / 2) - 1)  //Numero de estrelas

//Inicia estrelas
void stars_init(STAR stars[]){

  for(int i = 0; i < STARS_N; i++){
    stars[i].y = between_f(0, BUFFER_H);
    stars[i].speed = between_f(0.1, 1);
  }

}

//Destroi estrelas
void stars_update(STAR stars[]){

  for(int i = 0; i < STARS_N; i++){
    stars[i].y += stars[i].speed;

    //Se estrela fora o buffer de display
    if(stars[i].y >= BUFFER_H){
      stars[i].y = 0;
      stars[i].speed = between_f(0.1, 1);
    }
  }

}

void stars_draw(STAR stars[]){

  int star_x = 1;
  for(int i = 0; i < STARS_N; i++){
    al_draw_pixel(star_x, stars[i].y, al_map_rgb_f(255,255,255));
    star_x += 2;
  }

}

// --- SCORE ---

typedef struct SCORE {
  int   score;
  char  str_score[20];
  int   global_score;
  char  str_global_score[20];
  int   x1, y1, x2, y2;
  //ALLEGRO_BITMAP *score_map;
} SCORE;

void score_init(SCORE game_score){

  game_score.score = 0;
  strcpy(game_score.str_score, "0");
  game_score.global_score = 0;
  strcpy(game_score.str_global_score, "0");
  game_score.x1 = 160; game_score.y1 = 70;
  game_score.x2 = 370; game_score.y2 = 70;
  //game_score.score_map = al_load_bitmap("resources/score/score.png");

}

void score_deinit(SCORE game_score){

  //al_destroy_bitmap(game_score.score_map);

}

void score_draw (SCORE game_score, ALLEGRO_FONT *font){

  al_draw_text(font, al_map_rgb(255, 255, 255), 190, 30, 0, "SCORE       RECORD");
  al_draw_text(font, al_map_rgb(255, 255, 255), 190, 90, 0, game_score.str_score);

}

// --- BOARD ---

typedef struct candy {
  int x, y;       //Coordenadas
  int type;       //Tipo do doce
  int draw;       //Booleano de renderizar
} CANDY;

//Board_x [-- 80 (offset) -- : -- 560 (board) -- : -- 80 (offset) --]
//Board_y [-- 140 (score) -- : -- 560 (board) -- : -- 20 (offset) --]
#define X_OFFSET      80             //Limites do tabuleiro
#define Y_OFFSET      140            //Espaçamento do score
#define CANDY_SIZE    70             //Tamanho ocupado pelo doce
#define BOARD_N       8              //Tamanho da matriz
#define CANDY_TYPE_N  5              //Tipos diferentes de doces

void board_init (){


}

void board_deinit(){


}

void board_update(){


}

void board_draw(){


}



// --- Main ---

int main(){
  srand(time(NULL));

  //Instaladores
  must_init(al_init(), "allegro");                          //Allegro
  must_init(al_install_keyboard(), "keyboard");             //Teclado 
  must_init(al_install_mouse(), "mouse");                   //Mouse
  must_init(al_init_image_addon(), "image");                //Imagens
  must_init(al_init_primitives_addon(), "primitives");      //Primitivas

  //Variaveis de allegro engine
  ALLEGRO_TIMER* timer = al_create_timer(1.0 / FRAMES_N);   //Variavel de tempo
  must_init(timer, "timer");
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();     //Variavel de eventos
  must_init(queue, "queue");
  ALLEGRO_BITMAP *buffer; ALLEGRO_DISPLAY *display;         //Variaveis de display
  display_init(&buffer, &display);
  ALLEGRO_FONT *font;                                       //Variaveis de fonte
  font_init(&font);
  ALLEGRO_AUDIO_STREAM *bg_music;                           //Variaveis de audio
  audio_init(&bg_music);
  unsigned char key[ALLEGRO_KEY_MAX];                       //Variavel de teclado
  keyboard_init(key);
  mouse_t mouse;                                            //Variavel de mouse
  mouse_init();

  //Variaveis de funcionamento
  ALLEGRO_BITMAP *background;                               //Variavel de background
  background_init(&background);
  STAR stars[STARS_N];                                      //Variavel de estrelas de fundo
  stars_init(stars);
  SCORE game_score;                                         //Variavel de score
  score_init(game_score);
  //
  //
  //
  //
  //
  //

  //Registradores de evento
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_mouse_event_source());
  al_register_event_source(queue, al_get_timer_event_source(timer));
  al_register_event_source(queue, al_get_display_event_source(display));

  bool done = false;
  bool redraw = true;
  ALLEGRO_EVENT event;

  al_start_timer(timer);
  while(1){

    al_wait_for_event(queue, &event);

    switch ( event.type ){
      case ALLEGRO_EVENT_TIMER:
        //Update functions
        stars_update(stars);
        //
        //
        //
        //
        //
        //

        if ( key[ALLEGRO_KEY_ESCAPE] )
          done = true;

        redraw = true;
        break;

      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        done = true;
        break;
    } //Switch ( event.type )

    if ( done )
      break;

    keyboard_update(&event, key);
    mouse_update(&event, mouse);

    if( redraw && al_is_event_queue_empty(queue) ){
      display_pre_draw(&buffer);

      //Draw functions
      background_draw(&background);
      stars_draw(stars);
      score_draw(game_score, font);
      //
      //
      //
      //
      //
      //
      //
      //

      display_post_draw(&buffer, &display);
      redraw = false;
    } //If (done)
  } //While (1)

  //Destroi temas
  //score_deinit(game_score);         //Score com falha de segmentação no bitmap
  background_deinit(&background);
  //
  //
  //
  //
  //
  //
  //
  
  //Destroi resto
  audio_deinit(&bg_music);
  font_deinit(&font);
  display_deinit(&buffer, &display);
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);

  return 0;
}

