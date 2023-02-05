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

//Inicializa variavel de fonte
void font_init(ALLEGRO_FONT **font){

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
  *bg_music = al_load_audio_stream("resources/sound/Haggstrom.opus", 2, 2048);
  must_init(*bg_music, "Background music");
  al_set_audio_stream_playmode(*bg_music, ALLEGRO_PLAYMODE_LOOP);     //Toca musica
  al_attach_audio_stream_to_mixer(*bg_music, al_get_default_mixer());
}

//Destroi variaveis de audio
void audio_deinit(ALLEGRO_AUDIO_STREAM **bg_music){

  al_destroy_audio_stream(*bg_music);
}

// --- KEYBOARD ---

#define KEY_SEEN     1
#define KEY_RELEASED 2

void keyboard_init(unsigned char key[]){

  must_init(al_install_keyboard(), "keyboard");
  memset(key, 0, sizeof(unsigned char) * ALLEGRO_KEY_MAX);
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

//Inicia estrutura de mouse
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

//destroi mouse
void mouse_deinit(MOUSE *mouse){

  free(mouse);
}

//Atualiza mouse
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
//Inicia background
void background_init(ALLEGRO_BITMAP **bg){

  *bg = al_load_bitmap("resources/background/rock_bg.png");
  must_init(*bg, "Background");
}

//Destroi background
void background_deinit(ALLEGRO_BITMAP **bg){

  al_destroy_bitmap(*bg);
}

//Desenha background
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

//Renderiza estrelas
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
  int   global_score;
} SCORE;

//Inicia score
SCORE *score_init(){
  SCORE *game_score = malloc( sizeof(SCORE) );
  must_init(game_score, "Score");

  //Inicia score normal
  game_score->score = 0;
  game_score->global_score = 0;

  //Inicia global score
  FILE *filename = fopen("resources/score/score_history.txt", "a+");
  must_init(filename, "Global Score");

  int score_max = 0;
  while ( fscanf(filename, "%d", &score_max) != EOF )
    if ( score_max >= game_score->global_score )
      game_score->global_score = score_max;

  fclose(filename);
  return game_score;
}

//Destroi score
void score_deinit(SCORE *game_score){
  FILE *filename = fopen("resources/score/score_history.txt", "a");
  must_init(filename, "Save game score");

  if ( game_score->score > game_score->global_score )
    fprintf(filename, "%d\n", game_score->score);

  fclose(filename);
  free(game_score);
}

//Renderiza score
void score_draw (SCORE *game_score, ALLEGRO_FONT *font){
  char str_score[20];
  char str_global_score[20];

  snprintf(str_score, 20, "%d", game_score->score);
  snprintf(str_global_score, 20, "%d", game_score->global_score);

  al_draw_text(font, al_map_rgb(255, 255, 255), DISP_W/4.0 + 40   , 30, ALLEGRO_ALIGN_CENTER, "SCORE");
  al_draw_text(font, al_map_rgb(255, 255, 255), 3*DISP_W/4.0 - 40 , 30, ALLEGRO_ALIGN_CENTER, "RECORD");
  al_draw_text(font, al_map_rgb(255, 255, 255), DISP_W/4.0 + 40   , 90, ALLEGRO_ALIGN_CENTER, str_score);
  al_draw_text(font, al_map_rgb(255, 255, 255), 3*DISP_W/4.0 - 40 , 90, ALLEGRO_ALIGN_CENTER, str_global_score);
}

// --- Maquina de estados ---

typedef enum state_board {        //Estados do board
  BOARD_NEW_PLAY = 0,
  BOARD_SWITCH_JEWEL,
  BOARD_JEWEL_FALL
} STATE_BOARD;

typedef enum state_jewel {        //Estados da troca de peças
  JEWEL_GO = 0,
  JEWEL_BACK
} STATE_JEWEL;

typedef enum state_fall {         //Estado da queda de peças
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

//Inicia maquina de estados
void state_init(STATES *global_states) {

  global_states->board_state  = BOARD_NEW_PLAY;
  global_states->jewel_state  = JEWEL_GO;
  global_states->fall_state   = TEST_FALL;
  global_states->x_jewel_clk  = -1; global_states->y_jewel_clk  = -1;     //Troca de joias   
  global_states->x_jewel_rls  = -1; global_states->y_jewel_rls  = -1;     //Troca de joias
  global_states->i_jewel_fall = -1;                                       //Queda das joias
  global_states->fall_flag    = -1;
}

// --- BOARD ---

typedef struct jewel {
  int x, y;                         //Coordenadas
  int type;                         //Tipo do doce
  int draw;                         //Booleano de renderizar
  int special_gen_flag;             //Flag para saber se acabou de virar um especial
} JEWEL;

//Board_x [-- 80 (offset) -- : -- 560 (board) -- : -- 80 (offset) --]
//Board_y [-- 140 (score) -- : -- 560 (board) -- : -- 20 (offset) --]
//        [       70 (jewel) :                                      ]
#define X_OFFSET      80             //Limites do tabuleiro
#define Y_OFFSET      140            //Espaçamento do score
#define JEWEL_SIZE    70             //Tamanho ocupado pelo doce
#define BOARD_N       8              //Tamanho da matriz
#define JEWEL_TYPE_N  6              //Tipos diferentes de doces
#define FALL_SPEED 5


//Inicia board
JEWEL **board_init (ALLEGRO_BITMAP **candy_sprite){

  //Inicia vetor de sprites
  candy_sprite[0]  = al_load_bitmap("resources/sprites/rocks/rock1.png");   
  candy_sprite[1]  = al_load_bitmap("resources/sprites/rocks/rock2.png");   
  candy_sprite[2]  = al_load_bitmap("resources/sprites/rocks/rock3.png");   
  candy_sprite[3]  = al_load_bitmap("resources/sprites/rocks/rock4.png");   
  candy_sprite[4]  = al_load_bitmap("resources/sprites/rocks/rock5.png");   
  candy_sprite[5]  = al_load_bitmap("resources/sprites/rocks/rock6.png");   
  candy_sprite[6]  = al_load_bitmap("resources/sprites/rocks/special11.png");
  candy_sprite[7]  = al_load_bitmap("resources/sprites/rocks/special12.png");
  candy_sprite[8]  = al_load_bitmap("resources/sprites/rocks/special13.png");
  candy_sprite[9]  = al_load_bitmap("resources/sprites/rocks/special14.png");
  candy_sprite[10] = al_load_bitmap("resources/sprites/rocks/special15.png");
  candy_sprite[11] = al_load_bitmap("resources/sprites/rocks/special16.png");
  candy_sprite[12] = al_load_bitmap("resources/sprites/rocks/special21.png");
  candy_sprite[13] = al_load_bitmap("resources/sprites/rocks/special22.png");
  candy_sprite[14] = al_load_bitmap("resources/sprites/rocks/special23.png");
  candy_sprite[15] = al_load_bitmap("resources/sprites/rocks/special24.png");
  candy_sprite[16] = al_load_bitmap("resources/sprites/rocks/special25.png");
  candy_sprite[17] = al_load_bitmap("resources/sprites/rocks/special26.png");

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
      board[i][j].special_gen_flag = 0;
      if ( i > 0 )  board[i][j].draw = 1;
      else          board[i][j].draw = 0;
      x_aux += JEWEL_SIZE;
    }
    x_aux = 0;
    y_aux += JEWEL_SIZE;
  }

  return board;
}

//Destroi board
void board_deinit(JEWEL ***board, ALLEGRO_BITMAP **candy_sprite){

  //Destroi vetor de sprites
  for (int i=0; i<3*JEWEL_TYPE_N ;i++)
    al_destroy_bitmap(candy_sprite[i]);

  //Desaloca matriz
  free(*board[0]);
  free(*board);
}

//Retorna se tem sequencia de três
int board_check(JEWEL **board){

  //Verifica horizontal
  for (int i=1; i<BOARD_N+1 ;i++)
    for (int j=0; j<BOARD_N-2 ;j++){
      int tipo = board[i][j].type;
      if ( (board[i][j+1].type == tipo                     || 
            abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N   ||
            abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N ) && //Testa joias a direita
          (board[i][j+2].type == tipo                     ||
           abs(tipo-board[i][j+2].type) == JEWEL_TYPE_N   ||
           abs(tipo-board[i][j+2].type) == 2*JEWEL_TYPE_N ) )
        return 1;
    }

  //Verifica vertical
  for (int i=1; i<BOARD_N-1 ;i++)
    for (int j=0; j<BOARD_N ;j++){
      int tipo = board[i][j].type;
      if ( (board[i+1][j].type == tipo                     || 
            abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N   ||
            abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N   ) && //Testa joias a baixo
          (board[i+2][j].type == tipo                     || 
           abs(tipo-board[i+2][j].type) == JEWEL_TYPE_N   ||
           abs(tipo-board[i+2][j].type) == 2*JEWEL_TYPE_N ) )
        return 1;
    }

  return 0; //Não foi marcado ponto
}

//Troca joias de posicao logicamente
void switch_jewel_position(JEWEL **board, int x, int y, int z, int w){

  int tipo = board[x][y].type;
  board[x][y].type = board[z][w].type;
  board[z][w].type = tipo;
}

//Renderiza troca de joias
void switch_movement(JEWEL **board, int i_clk, int j_clk, int i_rls, int j_rls, int direction){

  int horizontal, vertical;
  if ( direction > 0 ){             //Movimenta indo
    horizontal = j_rls - j_clk;
    vertical = i_rls - i_clk;
  } else {                          //Movimenta voltando
    horizontal = j_clk - j_rls;
    vertical = i_clk - i_rls;
  }

  if ( horizontal > 0 ){            
    board[i_clk][j_clk].x += FALL_SPEED;       //Movimenta click pra direita
    board[i_rls][j_rls].x -= FALL_SPEED;       //Movimenta release para esquerda
  } else if ( horizontal < 0 ){
    board[i_clk][j_clk].x -= FALL_SPEED;     //Movimenta click para esquerda
    board[i_rls][j_rls].x += FALL_SPEED;     //Movimenta release para direita
  } else if ( vertical > 0 ){
    board[i_clk][j_clk].y += FALL_SPEED;     //Movimenta click para baixo
    board[i_rls][j_rls].y -= FALL_SPEED;     //Movimenta release para cima
  } else if ( vertical < 0 ){
    board[i_clk][j_clk].y -= FALL_SPEED;     //Movimenta click para cima
    board[i_rls][j_rls].y += FALL_SPEED;     //Movimenta release para baixo
  }
}

//Renderiza troca de joias e testa se deve desfazer
void switch_jewels(JEWEL **board, STATES *global_state, MOUSE *mouse){
  int *i_clk = &(mouse->i_clk), *j_clk = &(mouse->j_clk);   //Coordenadas board click
  int *i_rls = &(mouse->i_rls), *j_rls = &(mouse->j_rls);   //Coordenadas board release
  int *x_jewel_clk = &(global_state->x_jewel_clk), *y_jewel_clk = &(global_state->y_jewel_clk);   //Coordenada de trocas 
  int *x_jewel_rls = &(global_state->x_jewel_rls), *y_jewel_rls = &(global_state->y_jewel_rls);   //Coordenada de trocas

  switch ( global_state->jewel_state ){
    case JEWEL_GO:
      //Testa de terminou de movimentar
      if ( board[*i_clk][*j_clk].x == *x_jewel_rls && board[*i_clk][*j_clk].y == *y_jewel_rls &&
          board[*i_rls][*j_rls].x == *x_jewel_clk && board[*i_rls][*j_rls].y == *y_jewel_clk ){
        switch_jewel_position(board, *i_clk, *j_clk, *i_rls, *j_rls);                       //Troca peças na logica
        if ( board_check(board) ){                                                          //Se marcou ponto
          global_state->board_state = BOARD_JEWEL_FALL;                                     //Busca nova jogada
          board[*i_clk][*j_clk].x = *x_jewel_clk; board[*i_clk][*j_clk].y = *y_jewel_clk;   //Restaura coordenadas
          board[*i_rls][*j_rls].x = *x_jewel_rls; board[*i_rls][*j_rls].y = *y_jewel_rls;   //Restaura coordenadas
          *x_jewel_clk = -1; *y_jewel_clk = -1; *x_jewel_rls = -1; *y_jewel_rls = -1;
        } else {                                                                            //Se não marcou ponto
          switch_jewel_position(board, *i_clk, *j_clk, *i_rls, *j_rls);                     //Destroca na logica
          global_state->jewel_state = JEWEL_BACK;                                           //Retrocede doce
        }
      } else
        //Renderiza joia indo
        switch_movement(board, *i_clk, *j_clk, *i_rls, *j_rls, 1);
      break;

    case JEWEL_BACK:
      //Se terminou de voltar a posição original, busca nova jogada
      if ( board[*i_clk][*j_clk].x == *x_jewel_clk && board[*i_clk][*j_clk].y == *y_jewel_clk &&
          board[*i_rls][*j_rls].x == *x_jewel_rls && board[*i_rls][*j_rls].y == *y_jewel_rls ){
        global_state->jewel_state = JEWEL_GO;                                               //Estado da joia indo
        global_state->board_state = BOARD_NEW_PLAY;                                         //Carrega nova jogada
      }
      else
        switch_movement(board, *i_clk, *j_clk, *i_rls, *j_rls, -1);                         //Renderiza movimento
      break;
  }
}

//Verifica mathpoint
int matchpoint_verify (JEWEL **board, int tipo, int i, int j, int match_type){

  switch ( match_type ){
    case 1:                                                                         //Caso simples
      if (board[i][j].type   ==  tipo || abs(tipo-board[i][j].type)  == JEWEL_TYPE_N  || abs(tipo-board[i][j].type)  == 2*JEWEL_TYPE_N)
        return 1;
      break;

    case 2:                                                                         //Caso horizontal
      if ((board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+2].type == tipo || abs(tipo-board[i][j+2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+2].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 3:                                                                         //Caso vertical
      if ((board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+2][j].type == tipo || abs(tipo-board[i+2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+2][j].type) == 2*JEWEL_TYPE_N)) 
        return 1;
      break;

    case 4:   //Caso L
      if ((board[i-1][j].type == tipo || abs(tipo-board[i-1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i-2][j].type == tipo || abs(tipo-board[i-2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-2][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+2].type == tipo || abs(tipo-board[i][j+2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+2].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 5:   //Caso L contrario
      if ((board[i-1][j].type == tipo || abs(tipo-board[i-1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i-2][j].type == tipo || abs(tipo-board[i-2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-2][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j-1].type == tipo || abs(tipo-board[i][j-1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j-2].type == tipo || abs(tipo-board[i][j-2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-2].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 6:   //Caso L de ponta-cabeca
      if ((board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+2][j].type == tipo || abs(tipo-board[i+2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+2][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+2].type == tipo || abs(tipo-board[i][j+2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+2].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 7:   //Caso L contrario de ponta-cabeca
      if ((board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+2][j].type == tipo || abs(tipo-board[i+2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+2][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j-1].type == tipo || abs(tipo-board[i][j-1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j-2].type == tipo || abs(tipo-board[i][j-2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-2].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 8:   //Caso T
      if ((board[i][j-1].type == tipo || abs(tipo-board[i][j-1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+2][j].type == tipo || abs(tipo-board[i+2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+2][j].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 9:   //Caso T de ponta-cabeca
      if ((board[i][j-1].type == tipo || abs(tipo-board[i][j-1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i-1][j].type == tipo || abs(tipo-board[i-1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i-2][j].type == tipo || abs(tipo-board[i-2][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-2][j].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 10:  //Caso T deitado esquerda
      if ((board[i][j+1].type == tipo || abs(tipo-board[i][j+1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j+2].type == tipo || abs(tipo-board[i][j+2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j+2].type) == 2*JEWEL_TYPE_N) &&
          (board[i-1][j].type == tipo || abs(tipo-board[i-1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;

    case 11:  //Caso T deitado Direita
      if ((board[i][j-1].type == tipo || abs(tipo-board[i][j-1].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-1].type) == 2*JEWEL_TYPE_N) &&
          (board[i][j-2].type == tipo || abs(tipo-board[i][j-2].type) == JEWEL_TYPE_N || abs(tipo-board[i][j-2].type) == 2*JEWEL_TYPE_N) &&
          (board[i-1][j].type == tipo || abs(tipo-board[i-1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i-1][j].type) == 2*JEWEL_TYPE_N) &&
          (board[i+1][j].type == tipo || abs(tipo-board[i+1][j].type) == JEWEL_TYPE_N || abs(tipo-board[i+1][j].type) == 2*JEWEL_TYPE_N))
        return 1;
      break;
  }

  return 0;
}


//Seta draw para 0 a posicao de uma peça especial
void hide_special_explosion(JEWEL **board, int i, int j){

  //Se explosao circular
  if ( board[i][j].type >= JEWEL_TYPE_N && board[i][j].type < 2*JEWEL_TYPE_N ){	//Se tipo > 5 e < 12
                                                                                //Remove tipo especial para não backtraking infinito
    board[i][j].type -= JEWEL_TYPE_N;

    int a_end = i+2,  b_start = j-1,   b_end = j+2;
    if ( j == 0 )               b_start = j;
    else if ( j == BOARD_N-1 )  b_end = j+1;
    if ( i == BOARD_N )         a_end = i+1;

    for (int a=i-1; a<a_end ;a++)
      for (int b=b_start; b<b_end  ;b++){
        board[a][b].draw = 0;
        if ( board[a][b].type >= JEWEL_TYPE_N )  //Se tipo especial
          hide_special_explosion(board, a, b);}

  } else if ( board[i][j].type >= 2*JEWEL_TYPE_N ) {  //Explosao em cruz
                                                      //remove tipo especial
    board[i][j].type -= 2*JEWEL_TYPE_N;

    //Esconde horizontal
    for (int aux=0; aux<BOARD_N ;aux++){
      board[i][aux].draw = 0;
      if ( board[i][aux].type >= JEWEL_TYPE_N )
        hide_special_explosion(board, i, aux); }

    //Esconde vertical
    for (int aux=1; aux<BOARD_N+1 ;aux++){
      board[aux][j].draw = 0;
      if ( board[aux][j].type >= JEWEL_TYPE_N )
        hide_special_explosion(board, aux, j); }
  }
}

//Seta draw para 0
//Testa se há peças especiais e se não foram recem geradas
void hide_pieces(JEWEL **board, int tipo, int i, int j, int match_type){

  //Se for do tipo especial e estiver com a flag setada em zero
  switch ( match_type ){
    case 1:   //Caso em L
      board[i-1][j].draw = 0; board[i][j+1].draw = 0;
      board[i-2][j].draw = 0; board[i][j+2].draw = 0;
      if      ( board[i-1][j].type >= JEWEL_TYPE_N && board[i-1][j].special_gen_flag == 0 ) hide_special_explosion(board, i-1, j);      
      else if ( board[i-2][j].type >= JEWEL_TYPE_N && board[i-2][j].special_gen_flag == 0 ) hide_special_explosion(board, i-2, j);
      else if ( board[i][j+1].type >= JEWEL_TYPE_N && board[i][j+1].special_gen_flag == 0 ) hide_special_explosion(board, i, j+1);
      else if ( board[i][j+2].type >= JEWEL_TYPE_N && board[i][j+2].special_gen_flag == 0 ) hide_special_explosion(board, i, j+2);
      break;

    case 2:   //Caso de L invertido
      board[i-1][j].draw = 0; board[i][j-1].draw = 0;
      board[i-2][j].draw = 0; board[i][j-2].draw = 0;
      if      ( board[i-1][j].type >= JEWEL_TYPE_N && board[i-1][j].special_gen_flag == 0 ) hide_special_explosion(board, i-1, j);      
      else if ( board[i-2][j].type >= JEWEL_TYPE_N && board[i-2][j].special_gen_flag == 0 ) hide_special_explosion(board, i-2, j);
      else if ( board[i][j-1].type >= JEWEL_TYPE_N && board[i][j-1].special_gen_flag == 0 ) hide_special_explosion(board, i, j-1);
      else if ( board[i][j-2].type >= JEWEL_TYPE_N && board[i][j-2].special_gen_flag == 0 ) hide_special_explosion(board, i, j-2);
      break;

    case 3:   //Caso de L de ponta-cabeca 
      board[i+1][j].draw = 0; board[i][j+1].draw = 0;
      board[i+2][j].draw = 0; board[i][j+2].draw = 0;
      if      ( board[i+1][j].type >= JEWEL_TYPE_N && board[i+1][j].special_gen_flag == 0 ) hide_special_explosion(board, i+1, j);
      else if ( board[i+2][j].type >= JEWEL_TYPE_N && board[i+2][j].special_gen_flag == 0 ) hide_special_explosion(board, i+2, j);
      else if ( board[i][j+1].type >= JEWEL_TYPE_N && board[i][j+1].special_gen_flag == 0 ) hide_special_explosion(board, i, j+1);
      else if ( board[i][j+2].type >= JEWEL_TYPE_N && board[i][j+2].special_gen_flag == 0 ) hide_special_explosion(board, i, j+2);
      break;

    case 4:   //Caso de L invertido de ponta-cabeca
      board[i+1][j].draw = 0; board[i][j-1].draw = 0;
      board[i+2][j].draw = 0; board[i][j-2].draw = 0;
      if      ( board[i+1][j].type >= JEWEL_TYPE_N && board[i+1][j].special_gen_flag == 0 ) hide_special_explosion(board, i+1, j);      
      else if ( board[i+2][j].type >= JEWEL_TYPE_N && board[i+2][j].special_gen_flag == 0 ) hide_special_explosion(board, i+2, j);
      else if ( board[i][j-1].type >= JEWEL_TYPE_N && board[i][j-1].special_gen_flag == 0 ) hide_special_explosion(board, i, j-1);
      else if ( board[i][j-2].type >= JEWEL_TYPE_N && board[i][j-2].special_gen_flag == 0 ) hide_special_explosion(board, i, j-2);
      break;

    case 5:   //Caso de T em pé
      board[i][j-1].draw = 0; board[i+1][j].draw = 0;
      board[i][j+1].draw = 0; board[i+2][j].draw = 0;
      if      ( board[i][j-1].type >= JEWEL_TYPE_N && board[i][j-1].special_gen_flag == 0 ) hide_special_explosion(board, i, j-1);      
      else if ( board[i][j+1].type >= JEWEL_TYPE_N && board[i][j+1].special_gen_flag == 0 ) hide_special_explosion(board, i, j+1);
      else if ( board[i+1][j].type >= JEWEL_TYPE_N && board[i+1][j].special_gen_flag == 0 ) hide_special_explosion(board, i+1, j);
      else if ( board[i+2][j].type >= JEWEL_TYPE_N && board[i+2][j].special_gen_flag == 0 ) hide_special_explosion(board, i+2, j);
      break;

    case 6:   //Caso de T de ponta-cabeca
      board[i][j-1].draw = 0; board[i-1][j].draw = 0;
      board[i][j+1].draw = 0; board[i-2][j].draw = 0;
      if      ( board[i][j-1].type >= JEWEL_TYPE_N && board[i][j-1].special_gen_flag == 0 ) hide_special_explosion(board, i, j-1);      
      else if ( board[i][j+1].type >= JEWEL_TYPE_N && board[i][j+1].special_gen_flag == 0 ) hide_special_explosion(board, i, j+1);
      else if ( board[i-1][j].type >= JEWEL_TYPE_N && board[i-1][j].special_gen_flag == 0 ) hide_special_explosion(board, i-1, j);
      else if ( board[i-2][j].type >= JEWEL_TYPE_N && board[i-2][j].special_gen_flag == 0 ) hide_special_explosion(board, i-2, j);
      break;

    case 7:   //Caso de T deitado para esquerda
      board[i][j+1].draw = 0; board[i-1][j].draw = 0;
      board[i][j+2].draw = 0; board[i+1][j].draw = 0;
      if      ( board[i][j+1].type >= JEWEL_TYPE_N && board[i][j+1].special_gen_flag == 0 ) hide_special_explosion(board, i, j+1);      
      else if ( board[i][j+2].type >= JEWEL_TYPE_N && board[i][j+2].special_gen_flag == 0 ) hide_special_explosion(board, i, j+2);
      else if ( board[i-1][j].type >= JEWEL_TYPE_N && board[i-1][j].special_gen_flag == 0 ) hide_special_explosion(board, i-1, j);
      else if ( board[i+1][j].type >= JEWEL_TYPE_N && board[i+1][j].special_gen_flag == 0 ) hide_special_explosion(board, i+1, j);
      break;

    case 8:   //Caso de T deitado para direita
      board[i][j-1].draw = 0; board[i-1][j].draw = 0;
      board[i][j-2].draw = 0; board[i+1][j].draw = 0;
      if      ( board[i][j-1].type >= JEWEL_TYPE_N && board[i][j-1].special_gen_flag == 0 ) hide_special_explosion(board, i, j-1);      
      else if ( board[i][j-2].type >= JEWEL_TYPE_N && board[i][j-2].special_gen_flag == 0 ) hide_special_explosion(board, i, j-2);
      else if ( board[i-1][j].type >= JEWEL_TYPE_N && board[i-1][j].special_gen_flag == 0 ) hide_special_explosion(board, i-1, j);
      else if ( board[i+1][j].type >= JEWEL_TYPE_N && board[i+1][j].special_gen_flag == 0 ) hide_special_explosion(board, i+1, j);
      break;
  }
  return;
}

//Verifica matchpoint na horizontal
//Retorna numero de peças destruidas
int horizontal_test(JEWEL **board){
  int quant = 0;

  for (int i=1; i<BOARD_N+1 ;i++)
    for (int j=0; j<BOARD_N-2 ;j++){
      if ( board[i][j].draw ){

        int tipo = board[i][j].type;
        if ( matchpoint_verify(board, tipo, i, j, 2) ){                                 //Se marcou ponto
          int k = j+3;
          while ( k < BOARD_N && matchpoint_verify(board, tipo, i, k, 1) ) k++;         //Pega joias sequenciadas
          for (int aux=j; aux<k ;aux++)                                                 //Esconde doces sequenciados
                                                                                        //Se for comum
            if ( board[i][aux].type < JEWEL_TYPE_N )
              board[i][aux].draw = 0;
            else
              if ( board[i][aux].special_gen_flag == 0 )
                hide_special_explosion(board, i, aux);

          //Gera joia especial
          if ( k-j == 5 ){
            board[i][j+2].type += 2*JEWEL_TYPE_N;
            board[i][j+2].draw = 1;
            board[i][j+2].special_gen_flag = 1;
          }
          quant += k-j;
        }}}

  return quant;
}

//Verifica matchpoint na vertical
//Retorna numero de peças destruidas
int vertical_test(JEWEL **board){
  int quant = 0;

  for (int i=1; i<BOARD_N-1 ;i++)
    for (int j=0; j<BOARD_N ;j++){
      if ( board[i][j].draw ){

        int tipo = board[i][j].type;
        if ( matchpoint_verify(board, tipo, i, j, 3) ){
          int k = i+3;
          while ( k < BOARD_N+1 && matchpoint_verify(board, tipo, k, j, 1) ) k++;       //Pega joias sequenciadas
          for (int aux=i; aux<k ;aux++)                                                 //Esconde doces sequenciados
                                                                                        //Se for especial nao gerado recentemente
            if ( board[aux][j].type < JEWEL_TYPE_N )
              board[aux][j].draw = 0;
            else
              if ( board[aux][j].special_gen_flag == 0 )
                hide_special_explosion(board, aux, j);      //Se especial, explode

          //Gera joia especial
          if ( k-i == 5 ){
            board[i+2][j].type += 2*JEWEL_TYPE_N;
            board[i+2][j].draw = 1;
            board[i+2][j].special_gen_flag = 1;
          }
          quant += k-i;
        }}}

  return quant;
}

//Verifica matchpoint de L em pe
//Retorna quantas joias foram destruidas
int L_test(JEWEL **board){
  int quant = 0;

  //Verifica l normal
  for (int i=3; i<BOARD_N+1 ;i++)                                     //Vai de 3 a 8
    for (int j=0; j<BOARD_N-2 ;j++){                                  //Vai de 0 a 5
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 4) ){                 //Se marcou ponto
        if ( board[i][j].type < JEWEL_TYPE_N){                        //Se não for especial
          board[i][j].type += JEWEL_TYPE_N;                           //Transforma em especial
          board[i][j].special_gen_flag = 1;                           //Seta flag de recem gerado
        }
        hide_pieces(board, tipo, i, j, 1);                            //Escode joias a descer
        quant += 5; } }

  //Verifica l invertido
  for (int i=3; i<BOARD_N+1 ;i++)                                     //Vai de 3 a 8
    for (int j=2; j<BOARD_N ;j++){                                    //Vai de 2 a 7
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 5) ){                 //Se marcou ponto
        if ( board[i][j].type < JEWEL_TYPE_N){                        //Se não for especial
          board[i][j].type += JEWEL_TYPE_N;                           //Transforma em especial
          board[i][j].special_gen_flag = 1;                           //Seta flag de recem gerado
        }
        hide_pieces(board, tipo, i, j, 2);                            //Esconde joias a descer
        quant += 5; } }

  //Verifica l de ponta-cabeça
  for (int i=1; i<BOARD_N-1 ;i++)                                     //Vai de 1 a 6
    for (int j=0; j<BOARD_N-2 ;j++){                                  //Vai de 0 a 5
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 6) ){                 //Se marcou ponto
        if ( board[i][j].type < JEWEL_TYPE_N){                        //Se não for especial
          board[i][j].type += JEWEL_TYPE_N;                           //Transforma em especial
          board[i][j].special_gen_flag = 1;                           //Seta flag de recem gerado
        }
        hide_pieces(board, tipo, i, j, 3);                            //Esconde joias a descer
        quant += 5; } }

  //Verifica l invertido e de ponta-cabeça
  for (int i=1; i<BOARD_N-1 ;i++)                                     //Vai de 1 a 6
    for (int j=2; j<BOARD_N ;j++){                                    //Vai de 2 a 7
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 7) ){                 //Se marcou ponto
        if ( board[i][j].type < JEWEL_TYPE_N){                        //Se não for especial
          board[i][j].type += JEWEL_TYPE_N;                           //Transforma em especial
          board[i][j].special_gen_flag = 1;                           //Seta flag de recem gerado
        }
        hide_pieces(board, tipo, i, j, 4);                            //Esconde joias a descer
        quant += 5; } }

  return quant;
}

//Verifica matchpoint em T
//Retorna numero de joias marcadas
int T_test(JEWEL **board){
  int quant = 0;

  //Verifica T em pe
  for (int i=1; i<BOARD_N-1 ;i++)
    for (int j=1; j<BOARD_N-1; j++){
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 8) ){                 //Se marcou ponto
        if ( board[i][j].type < JEWEL_TYPE_N ){                       //Se nao for especial  
          board[i][j].type += JEWEL_TYPE_N;                           //Transforma em especial
          board[i][j].special_gen_flag = 1;                           //Seta flag de recem gerado
        }
        hide_pieces(board, tipo, i, j, 5);                            //Esconde joias
        quant += 5; } }

  //Verifica T de ponta cabeca
  for (int i=3; i<BOARD_N+1 ;i++)
    for (int j=1; j<BOARD_N-1; j++){
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 9) ){
        if ( board[i][j].type < JEWEL_TYPE_N ){
          board[i][j].type += JEWEL_TYPE_N;
          board[i][j].special_gen_flag = 1;
        }
        hide_pieces(board, tipo, i, j, 6);
        quant += 5; } }

  //Verifica T deitado para esquerda
  for (int i=2; i<BOARD_N ;i++)
    for (int j=0; j<BOARD_N-2; j++){
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 10) ){
        if ( board[i][j].type < JEWEL_TYPE_N ){
          board[i][j].type += JEWEL_TYPE_N;
          board[i][j].special_gen_flag = 1;
        }
        hide_pieces(board, tipo, i, j, 7);
        quant += 5; } }

  //Verifica T deitado para direita
  for (int i=2; i<BOARD_N ;i++)
    for (int j=2; j<BOARD_N; j++){
      int tipo = board[i][j].type;
      if ( matchpoint_verify(board, tipo, i, j, 11) ){
        if ( board[i][j].type < JEWEL_TYPE_N ){
          board[i][j].type += JEWEL_TYPE_N;
          board[i][j].special_gen_flag = 1;
        }
        hide_pieces(board, tipo, i, j, 8);
        quant += 5; } }

  return quant;
}

//Busca nova jogada do mouse
void get_new_play(JEWEL **board, STATES *global_state, MOUSE *mouse){
  int *i_clk = &(mouse->i_clk), *j_clk = &(mouse->j_clk);   //Coordenadas board click
  int *i_rls = &(mouse->i_rls), *j_rls = &(mouse->j_rls);   //Coordenadas board release
  int *x_jewel_clk = &(global_state->x_jewel_clk), *y_jewel_clk = &(global_state->y_jewel_clk);
  int *x_jewel_rls = &(global_state->x_jewel_rls), *y_jewel_rls = &(global_state->y_jewel_rls);

  //Se o click foi no tabuleiro
  if ( (mouse->x_clk > X_OFFSET && mouse->x_clk < DISP_W-X_OFFSET) &&
      (mouse->y_clk > Y_OFFSET && mouse->y_clk < DISP_H-20) )
    //Se o release foi no tabuleiro
    if ( (mouse->x_rls > X_OFFSET && mouse->x_rls < DISP_W-X_OFFSET) &&
        (mouse->y_rls > Y_OFFSET && mouse->y_rls < DISP_H-20) )
      //testa se houve movimento
      if ( mouse->x_clk != mouse->x_rls || mouse->y_clk != mouse->y_rls ) {
        int h_delta     = abs(mouse->x_clk-mouse->x_rls);
        int v_delta     = abs(mouse->y_clk-mouse->y_rls);
        int horizontal  = mouse->x_rls - mouse->x_clk;
        int vertical    = mouse->y_rls - mouse->y_clk;

        //Testa movimentacao minima
        if ( h_delta >= JEWEL_SIZE-18 || v_delta >= JEWEL_SIZE-18 ) {
          //Calcula coordenadas do doce clicado e solto na matriz
          *i_clk = (mouse->y_clk - Y_OFFSET + JEWEL_SIZE)/JEWEL_SIZE;  //Linha da joia clicada
          *j_clk = (mouse->x_clk - X_OFFSET)/JEWEL_SIZE;               //Coluna da joia clicada
          *i_rls = (mouse->y_rls - Y_OFFSET + JEWEL_SIZE)/JEWEL_SIZE;  //Linha da joia solta
          *j_rls = (mouse->x_rls - X_OFFSET)/JEWEL_SIZE;               //Coluna da joia solta

          //Se variação horizontal maior que vertical
          if ( h_delta > v_delta ){  *i_rls = *i_clk;
            if ( horizontal > 0 )    *j_rls = *j_clk + 1;              //Movimenta direita
            else                     *j_rls = *j_clk - 1;              //Movimenta esquerda
          } else {                   *j_rls = *j_clk;
            if ( vertical > 0 )      *i_rls = *i_clk + 1;              //Movimenta baixo
            else                     *i_rls = *i_clk - 1;              //Movimenta cima
          }

          //Correção de bug
          if ( *i_rls < BOARD_N+1 && *j_rls < BOARD_N ) {
            *x_jewel_clk = board[*i_clk][*j_clk].x;                    //Salva as coordenadas da troca
            *y_jewel_clk = board[*i_clk][*j_clk].y;                    //Salva as coordenadas da troca
            *x_jewel_rls = board[*i_rls][*j_rls].x;                    //Salva as coordenadas da troca
            *y_jewel_rls = board[*i_rls][*j_rls].y;                    //Salva as coordenadas da troca
            global_state->board_state = BOARD_SWITCH_JEWEL;
          } } }

  return;
}

void imprime_board(JEWEL **board){

  for (int i=0; i<BOARD_N+1 ;i++){
    for (int j=0; j<BOARD_N ;j++)
      printf("[%d][%d] ", i, j);

    printf("\n");
    for (int j=0; j<BOARD_N ;j++)
      printf(" %d  %d  ", board[i][j].type, board[i][j].draw);
    printf("\n");
  }
  printf("\n");
  printf("\n");
  return;
}

//testa se ainda existem possiveis jogadas
//Retorna 1 em caso de game over
int game_over(JEWEL **board){

  //Testa submatriz do board
  for (int i=1; i<BOARD_N; i++)        //i vai de 1 a 7
    for (int j=0; j<BOARD_N-1 ;j++){   //j vai de 0 a 6
                                       //Troca horizontal
      switch_jewel_position(board, i, j, i, j+1);
      if ( board_check(board) ){ switch_jewel_position(board, i, j, i, j+1); return 0; }
      switch_jewel_position(board, i, j, i, j+1);

      //Troca vertical
      switch_jewel_position(board, i, j, i+1, j);
      if ( board_check(board) ){ switch_jewel_position(board, i, j, i+1, j); return 0; }
      switch_jewel_position(board, i, j, i+1, j);
    }

  //Testa coluna final
  for (int i=1; i<BOARD_N ;i++){
    switch_jewel_position(board, i, 7, i+1, 7);
    if ( board_check(board) ){ switch_jewel_position(board, i, 7, i+1, 7); return 0; }
    switch_jewel_position(board, i, 7, i+1, 7);
  }

  //Testa linha final
  for (int j=0; j<BOARD_N-1 ;j++){
    switch_jewel_position(board, 8, j, 8, j+1);
    if ( board_check(board) ){ switch_jewel_position(board, 8, j, 8, j+1); return 0; }
    switch_jewel_position(board, 8, j, 8, j+1);
  }

  return 1;
}


// Renderiza joias caindo
// Retorna 1 se tiver joia para cair
// Retorna 0 do contrario
int jewel_fall(JEWEL **board, STATES *global_state, SCORE *game_score){
  int *i_fall = &(global_state->i_jewel_fall);
  int *fall_flag = &(global_state->fall_flag);
  int jewel_quant = 0;

  switch ( global_state->fall_state ){
    case TEST_FALL:
      //Bateria de testes
      jewel_quant += T_test(board);
      jewel_quant += L_test(board);
      jewel_quant += horizontal_test(board);
      jewel_quant += vertical_test(board);

      //Atualiza pontuacao
      game_score->score += 100 * jewel_quant;

      //Se marcou pontuacao, muda pra fall_board
      if ( jewel_quant ){
        global_state->fall_state = RENDER_FALL;
        *fall_flag = 1;
        *i_fall = 1;
        return 1;
      } else
        if ( game_over(board) ){
          printf("Game Over seu bosta\n");
          exit(1);
        } else 
          global_state->board_state = BOARD_NEW_PLAY;
      return 0;

      //Renderiza joias caindo na linha *i_fall
    case RENDER_FALL:
      for (int j=0; j<BOARD_N ;j++){
        //Se encontrou joia para renderizar
        if ( board[*i_fall][j].draw == 0 ){
          //Marca como achou e marca que não terminou de renderizar
          *fall_flag = 0;
          if ( board[*i_fall-1][j].y == board[*i_fall][j].y ){     //Se joia tiver terminado de cair

            //restaura y
            for (int i=*i_fall-1; i>-1 ;i--)
              board[i][j].y -= JEWEL_SIZE;
            //Troca tipos
            for (int i=*i_fall; i>0 ;i--)
              board[i][j].type = board[i-1][j].type;
            board[0][j].type = between(0, JEWEL_TYPE_N);
            //Permite renderizar e apaga linha zero
            board[*i_fall][j].draw = 1;
            board[0][j].draw = 0;
            //Passa para proxima linha se tiver renderizado tudo
            *fall_flag = 1;

          } else {
            //Renderiza
            for (int i=*i_fall-1; i>-1 ;i--){
              board[i][j].y += FALL_SPEED; if ( board[0][j].y > 100 ) board[0][j].draw = 1;
            } } } }

      //Se não houve joia para renderizar, ou terminou de renderizar
      if ( *fall_flag == 1 ){
        (*i_fall)++;
        if ( *i_fall > BOARD_N )
          global_state->fall_state = TEST_FALL; }
      break;
  }
  return 1;
}


//Atualiza board
void board_update(JEWEL **board, STATES *global_state, MOUSE *mouse, SCORE *game_score){
  int *i_clk = &(mouse->i_clk), *j_clk = &(mouse->j_clk);   //Coordenadas board click
  int *i_rls = &(mouse->i_rls), *j_rls = &(mouse->j_rls);   //Coordenadas board release

  switch ( global_state->board_state ){
    case BOARD_NEW_PLAY:    //Carrega nova jogada
      get_new_play(board, global_state, mouse);
      break;

    case BOARD_SWITCH_JEWEL:                                  //Troca joias de posição
      switch_jewels(board, global_state, mouse);
      break;

    case BOARD_JEWEL_FALL:                                    //Desce joias do matchpoint
      jewel_fall(board, global_state, game_score);
      *i_clk = -1; *j_clk = -1;                               //Zera mouse
      *i_rls = -1; *j_rls = -1;                               //Zera mouse
      break;
  }
}

//Renderiza board
void board_draw(JEWEL **board, ALLEGRO_BITMAP **candy_sprite){

  for(int i=0; i<BOARD_N+1 ;i++)    //Ignora a primeira fileira de peças
    for(int j=0; j<BOARD_N ;j++)
      if ( board[i][j].draw ){
        al_draw_bitmap(candy_sprite[board[i][j].type], board[i][j].x, board[i][j].y, 0);
        board[i][j].special_gen_flag = 0;
      }
}


// --- Main ---

int main(){
  srand(time(NULL));

  //Instaladores
  must_init(al_init(), "allegro");                        //Allegro
  must_init(al_init_primitives_addon(), "primitives");    //Primitivas
  must_init(al_init_image_addon(), "image");              //Imagens
  al_install_audio();
  al_init_acodec_addon();
  al_reserve_samples(128);
  al_init_font_addon();
  al_init_ttf_addon();

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

  //Variaveis de tema do jogo
  ALLEGRO_AUDIO_STREAM *bg_music;                         //Variaveis de audio
  audio_init(&bg_music);

  ALLEGRO_FONT *font;                                     //Variaveis de fonte
  font_init(&font);

  ALLEGRO_BITMAP *background;                             //Variavel de background
  background_init(&background);

  STAR stars[STARS_N];                                    //Variavel de estrelas de fundo
  stars_init(stars);

  SCORE *game_score;                                      //Variavel de score
  game_score = score_init();

  JEWEL **board; ALLEGRO_BITMAP *candy_sprite[18];        //Variavel do tabuleiro e vetor de sprites
  board = board_init(candy_sprite);

  STATES global_state;                                    //Variavel de maquina de estados
  state_init(&global_state);

  int ok = jewel_fall(board, &global_state, game_score);
  while ( ok )
    ok = jewel_fall(board, &global_state, game_score);

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
        board_update(board, &global_state, mouse, game_score);

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

