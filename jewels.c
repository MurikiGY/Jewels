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

  must_init(al_install_keyboard(), "keyboard");
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
  int x, y;                   //Bit do movimento
  int x_clk, y_clk;           //Bit do click
  int i_clk, j_clk;           //Posição do click
  int x_rls, y_rls;           //Bit do release
  int i_rls, j_rls;           //Posição do release
} MOUSE;

MOUSE *mouse_init(){

  must_init(al_install_mouse(), "Mouse");
  MOUSE *mouse = malloc( sizeof(MOUSE) );
  must_init(mouse, "Mouse");
  mouse->x_clk = -1; mouse->y_clk = -1;
  mouse->i_clk = -1; mouse->j_clk = -1;
  mouse->x_rls = -1; mouse->y_rls = -1;
  mouse->i_rls = -1; mouse->j_rls = -1;

  return mouse;

}

void mouse_deinit(MOUSE *mouse){

  free(mouse);

}

void mouse_update(ALLEGRO_EVENT* event, MOUSE *mouse){

  switch(event->type){
    case ALLEGRO_EVENT_TIMER:
      mouse->x_rls = -1;
      mouse->y_rls = -1;
      break;

    case ALLEGRO_EVENT_MOUSE_AXES:
      mouse->x = event->mouse.x;
      mouse->y = event->mouse.y;
      break;

    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
      mouse->x_clk = event->mouse.x;
      mouse->y_clk = event->mouse.y;
      break;

    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
      mouse->x_rls = event->mouse.x;
      mouse->y_rls = event->mouse.y;
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

void background_draw(ALLEGRO_BITMAP *bg){

  al_draw_bitmap(bg, 0, 0, 0);

}

// --- STARS ---

typedef struct STAR {
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

    if(stars[i].y >= BUFFER_H){   //Se estrela fora do display
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
  int   x_score, y_score;
  int   x_global, y_global;
} SCORE;

SCORE *score_init(){

  SCORE *game_score = malloc( sizeof(SCORE) );
  must_init(game_score, "Score");

  game_score->score = 0;
  strcpy(game_score->str_score, "0");
  game_score->global_score = 0;
  strcpy(game_score->str_global_score, "0");
  game_score->x_score = 160; game_score->y_score = 70;
  game_score->x_global = 370; game_score->y_global = 70;

  return game_score;

}

void score_deinit(SCORE *game_score){

  free(game_score);

}

void score_draw (SCORE *game_score, ALLEGRO_FONT *font){

  al_draw_text(font, al_map_rgb(255, 255, 255), 190, 30, 0, "SCORE       RECORD");
  al_draw_text(font, al_map_rgb(255, 255, 255), 190, 90, 0, game_score->str_score);

}

// --- BOARD ---

typedef struct jewel {
  int x, y;       //Coordenadas
  int type;       //Tipo do doce
  int draw;       //Booleano de renderizar
} JEWEL;

typedef enum state_board {
  BOARD_NEW_PLAY = 0,
  BOARD_SWITCH_JEWEL,
  BOARD_JEWEL_FALL
} STATE_BOARD;

//Board_x [-- 80 (offset) -- : -- 560 (board) -- : -- 80 (offset) --]
//Board_y [-- 140 (score) -- : -- 560 (board) -- : -- 20 (offset) --]
#define X_OFFSET      80             //Limites do tabuleiro
#define Y_OFFSET      140            //Espaçamento do score
#define JEWEL_SIZE    70             //Tamanho ocupado pelo doce
#define BOARD_N       8              //Tamanho da matriz
#define JEWEL_TYPE_N  5              //Tipos diferentes de doces

JEWEL **board_init (ALLEGRO_BITMAP **candy_sprite){

  //Inicia vetor de sprites
  candy_sprite[0] = al_load_bitmap("resources/candy/candy1.png");   //Ponteiro de sprite 0
  candy_sprite[1] = al_load_bitmap("resources/candy/candy2.png");   //Ponteiro de sprite 1
  candy_sprite[2] = al_load_bitmap("resources/candy/candy3.png");   //Ponteiro de sprite 2
  candy_sprite[3] = al_load_bitmap("resources/candy/candy4.png");   //Ponteiro de sprite 3
  candy_sprite[4] = al_load_bitmap("resources/candy/candy5.png");   //Ponteiro de sprite 4

  JEWEL **board;  //Altura BOARD_N+1 e comprimento BOARD_N

  board = malloc( sizeof(JEWEL *) * (BOARD_N+1) );            //Vetor de ponteiros
  must_init(board, "Board Pointers");
  board[0] = malloc( sizeof(JEWEL) * BOARD_N * (BOARD_N+1) ); //Matriz com todos os doces
  must_init(board[0], "Board game matrix");

  //Ajusta vetor de ponteiros
  for (int i=1; i<BOARD_N+1 ;i++)
    board[i] = board[0] + i*BOARD_N;

  //Percorre iniciando matriz
  int x_aux = 0, y_aux = -JEWEL_SIZE;
  for (int i=0; i<BOARD_N+1 ;i++){
    for (int j=0; j<BOARD_N ;j++){
      board[i][j].x = X_OFFSET + x_aux;
      board[i][j].y = Y_OFFSET + y_aux;
      board[i][j].type = between(0, JEWEL_TYPE_N);
      if ( i > 0 )
        board[i][j].draw = 1;
      else
        board[i][j].draw = 0;
      x_aux += JEWEL_SIZE;
    }
    x_aux = 0;
    y_aux += JEWEL_SIZE;
  }
 
  return board;

}

void board_deinit(JEWEL ***board, ALLEGRO_BITMAP **candy_sprite){

  //Destroi vetor de sprites
  al_destroy_bitmap(candy_sprite[0]);
  al_destroy_bitmap(candy_sprite[1]);
  al_destroy_bitmap(candy_sprite[2]);
  al_destroy_bitmap(candy_sprite[3]);
  al_destroy_bitmap(candy_sprite[4]);

  //Desaloca memoria
  free(*board[0]);
  free(*board);

}

//Troca joias do board de posicao
void switch_jewel(JEWEL **board, int x, int y, int z, int w){

  int tipo = board[x][y].type;
  board[x][y].type = board[z][w].type;
  board[z][w].type = tipo;

}


void board_update(JEWEL **board, STATE_BOARD *state, MOUSE *mouse){

  switch ( *state ){
    case BOARD_NEW_PLAY:    //Carrega nova jogada
      //Calcula coordenadas do doce clicado na matriz
      mouse->i_clk = (mouse->y_clk - Y_OFFSET)/JEWEL_SIZE;
      mouse->j_clk = (mouse->x_clk - X_OFFSET)/JEWEL_SIZE;
      mouse->i_rls = (mouse->y_rls - Y_OFFSET)/JEWEL_SIZE;
      mouse->j_rls = (mouse->x_rls - X_OFFSET)/JEWEL_SIZE;

      //Se o click foi no tabuleiro
      if ((mouse->i_clk > -1 && mouse->i_clk < BOARD_N) && 
          (mouse->j_clk > -1 && mouse->j_clk < BOARD_N))
        //Se o release foi no tabuleiro
        if ((mouse->i_rls > -1 && mouse->i_rls < BOARD_N) && 
            (mouse->j_rls > -1 && mouse->j_rls < BOARD_N))
          //Se o release foi ao lado do click
          if ((mouse->i_rls==mouse->i_clk && mouse->j_rls==mouse->j_clk-1) || 
              (mouse->i_rls==mouse->i_clk && mouse->j_rls==mouse->j_clk+1) ||
              (mouse->i_rls==mouse->i_clk-1 && mouse->j_rls==mouse->j_clk) || 
              (mouse->i_rls==mouse->i_clk+1 && mouse->j_rls==mouse->j_clk)){
            (mouse->i_clk)++; (mouse->i_rls)++;     //Correção da linha 0 invisível
            *state = BOARD_SWITCH_JEWEL;
          }
      break;

    case BOARD_SWITCH_JEWEL:    //Troca joias de posição
      switch_jewel(board, mouse->i_clk, mouse->j_clk, mouse->i_rls, mouse->j_rls);
      *state = BOARD_JEWEL_FALL;
      break;

    case BOARD_JEWEL_FALL:      //Desce joias do matchpoint
      mouse->i_clk = -1; mouse->j_clk = -1;
      mouse->i_rls = -1; mouse->j_rls = -1;
      *state = BOARD_NEW_PLAY;
      break;
  }

}

void board_draw(JEWEL **board, ALLEGRO_BITMAP **candy_sprite){

  for(int i=0; i<BOARD_N+1 ;i++)    //Ignora a primeira fileira de peças
    for(int j=0; j<BOARD_N ;j++)
      if ( board[i][j].draw )
        al_draw_bitmap(candy_sprite[board[i][j].type], board[i][j].x, board[i][j].y, 0);

}



// --- Main ---

int main(){
  srand(time(NULL));

  //Instaladores
  must_init(al_init(), "allegro");                        //Allegro
  must_init(al_init_image_addon(), "image");              //Imagens
  must_init(al_init_primitives_addon(), "primitives");    //Primitivas

  //Variaveis de allegro engine
  ALLEGRO_TIMER* timer = al_create_timer(1.0 / FRAMES_N); //Variavel de tempo
  must_init(timer, "timer");
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();   //Variavel de eventos
  must_init(queue, "queue");
  ALLEGRO_BITMAP *buffer; ALLEGRO_DISPLAY *display;       //Variaveis de display
  display_init(&buffer, &display);
  unsigned char key[ALLEGRO_KEY_MAX];                     //Variavel de teclado
  keyboard_init(key);
  MOUSE *mouse;                                           //Variavel de mouse
  mouse = mouse_init();
  ALLEGRO_FONT *font;                                     //Variaveis de fonte
  font_init(&font);
  ALLEGRO_AUDIO_STREAM *bg_music;                         //Variaveis de audio
  audio_init(&bg_music);

  //Variaveis de funcionamento
  ALLEGRO_BITMAP *background;                             //Variavel de background
  background_init(&background);
  STAR stars[STARS_N];                                    //Variavel de estrelas de fundo
  stars_init(stars);
  SCORE *game_score;                                      //Variavel de score
  game_score = score_init();
  JEWEL **board; ALLEGRO_BITMAP *candy_sprite[5];         //Variavel do tabuleiro e vetor de sprites
  board = board_init(candy_sprite);
  STATE_BOARD board_state;                                //Variave de estado do tabuleiro
  board_state = BOARD_NEW_PLAY;

  //Registradores de evento
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_mouse_event_source());
  al_register_event_source(queue, al_get_timer_event_source(timer));
  al_register_event_source(queue, al_get_display_event_source(display));

  bool done = false;    //Fim de jogo
  bool redraw = true;   //Renderizar
  ALLEGRO_EVENT event;

  al_start_timer(timer);
  while(1){

    al_wait_for_event(queue, &event);

    switch ( event.type ){
      case ALLEGRO_EVENT_TIMER:
        //Update functions
        stars_update(stars);
        board_update(board, &board_state, mouse);

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
      background_draw(background);
      stars_draw(stars);
      score_draw(game_score, font);
      board_draw(board, candy_sprite);

      display_post_draw(&buffer, &display);
      redraw = false;
    } //If (done)
  } //While (1)

  //Destroi temas
  score_deinit(game_score);
  background_deinit(&background);
  board_deinit(&board, candy_sprite);
  
  //Destroi resto
  audio_deinit(&bg_music);
  font_deinit(&font);
  mouse_deinit(mouse);
  display_deinit(&buffer, &display);
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);

  return 0;
}

