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

// --- Maquina de estados ---

typedef enum state_board {
  BOARD_NEW_PLAY = 0,
  BOARD_SWITCH_JEWEL,
  BOARD_JEWEL_FALL
} STATE_BOARD;

typedef enum state_jewel {
  JEWEL_GO = 0,
  JEWEL_BACK
} STATE_JEWEL;

typedef enum state_fall {
  TEST_FALL = 0,
  L_FALL,
  HORIZONTAL_FALL,
  VERTICAL_FALL
} STATE_FALL;

typedef struct states {
  STATE_BOARD board_state;          //Maquina de estado do tabuleiro
  STATE_JEWEL jewel_state;          //Maquina de estado da joia
  STATE_FALL  fall_state;           //Maquina de estado da queda do tabuleiro
  int x_jewel_clk, y_jewel_clk;     //Coordenadas da joia clicada
  int x_jewel_rls, y_jewel_rls;     //Coordenadas da joia solta
  int i_jewel_fall, j_jewel_fall;   //coordenadas da queda
  int k_jewel_fall, l_jewel_fall;
} STATES;

void state_init(STATES *global_states) {

  global_states->board_state  = BOARD_NEW_PLAY;
  global_states->jewel_state  = JEWEL_GO;
  global_states->fall_state   = TEST_FALL;
  global_states->x_jewel_clk  = -1; global_states->y_jewel_clk  = -1;     //Troca de joias   
  global_states->x_jewel_rls  = -1; global_states->y_jewel_rls  = -1;     //Troca de joias
  global_states->i_jewel_fall = -1; global_states->j_jewel_fall = -1;   //Queda das joias
  global_states->k_jewel_fall = -1; global_states->l_jewel_fall = -1;   //Queda das joias
}

// --- BOARD ---

typedef struct jewel {
  int x, y;                         //Coordenadas
  int type;                         //Tipo do doce
  int draw;                         //Booleano de renderizar
} JEWEL;

//Board_x [-- 80 (offset) -- : -- 560 (board) -- : -- 80 (offset) --]
//Board_y [-- 140 (score) -- : -- 560 (board) -- : -- 20 (offset) --]
//        [       70 (jewel) :                                      ]
#define X_OFFSET      80             //Limites do tabuleiro
#define Y_OFFSET      140            //Espaçamento do score
#define JEWEL_SIZE    70             //Tamanho ocupado pelo doce
#define BOARD_N       8              //Tamanho da matriz
#define JEWEL_TYPE_N  5              //Tipos diferentes de doces
#define FALL_SPEED 5

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
  for (int i=0; i<5 ;i++)
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
      if (board[i][j+1].type == tipo && board[i][j+2].type == tipo)
        return 1;
    }

  //Verifica vertical
  for (int i=1; i<BOARD_N-1 ;i++)
    for (int j=0; j<BOARD_N ;j++){
      int tipo = board[i][j].type;
      if (board[i+1][j].type == tipo && board[i+2][j].type == tipo)
        return 1;
    }

  return 0; //Não foi marcado ponto
}

//Troca joias do board de posicao
void switch_jewel_position(JEWEL **board, int x, int y, int z, int w){

  int tipo = board[x][y].type;
  board[x][y].type = board[z][w].type;
  board[z][w].type = tipo;
}

//Movimenta troca de doces
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

//Troca joias de posição, renderiza e testa se deve desfazer
void switch_jewels(JEWEL **board, STATES *global_state, MOUSE *mouse){
  int *i_clk = &(mouse->i_clk), *j_clk = &(mouse->j_clk);   //Coordenadas board click
  int *i_rls = &(mouse->i_rls), *j_rls = &(mouse->j_rls);   //Coordenadas board release
  int *x_jewel_clk = &(global_state->x_jewel_clk), *y_jewel_clk = &(global_state->y_jewel_clk);
  int *x_jewel_rls = &(global_state->x_jewel_rls), *y_jewel_rls = &(global_state->y_jewel_rls);

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

//Verifica matchpoint na horizontal
int horizontal_test(JEWEL **board, STATES *global_state){
  int *i_fall = &(global_state->i_jewel_fall), *j_fall = &(global_state->j_jewel_fall);
  int *k_fall = &(global_state->k_jewel_fall), *l_fall = &(global_state->l_jewel_fall);

  for (int i=1; i<BOARD_N+1 ;i++)
    for (int j=0; j<BOARD_N-2 ;j++){
      int tipo = board[i][j].type;
      if ( board[i][j+1].type == tipo && board[i][j+2].type == tipo ){  //Matchpoint horizontal
        int k = j+3;
        while ( k < BOARD_N && board[i][k].type == tipo )
          k++;
        *i_fall = i; *j_fall = j;
        *k_fall = k; *l_fall = -1;
        for (; j<k ;j++)    //Esconde doces sequenciados
          board[i][j].draw = 0;
        return 1; } }

  return 0;
}

//Verifica matchpoint na vertical
int vertical_test(JEWEL **board, STATES *global_state){
  int *i_fall = &(global_state->i_jewel_fall), *j_fall = &(global_state->j_jewel_fall);
  int *k_fall = &(global_state->k_jewel_fall), *l_fall = &(global_state->l_jewel_fall);

  for (int i=1; i<BOARD_N-1 ;i++)
    for (int j=0; j<BOARD_N ;j++){
      int tipo = board[i][j].type;
      if ( board[i+1][j].type == tipo && board[i+2][j].type == tipo ){ //Triple candy
        int k = i+3;
        while ( k < BOARD_N+1 && board[k][j].type == tipo)
          k++;
        *i_fall = i;  *j_fall = j;
        *k_fall = -1; *l_fall = k;
        for (; i<k ;i++)    //Esconde doces sequenciados
          board[i][j].draw = 0;
        return 1; } }
 
  return 0;
}

//Verifica matchpoint em l
int l_test(JEWEL **board, STATES *global_state){
  int *i_fall = &(global_state->i_jewel_fall), *j_fall = &(global_state->j_jewel_fall);
  int *k_fall = &(global_state->k_jewel_fall), *l_fall = &(global_state->l_jewel_fall);

  //Verifica l normal
  for (int i=3; i<BOARD_N+1 ;i++)                                         //Vai de 3 a 8
    for (int j=0; j<BOARD_N-2 ;j++){                                      //Vai de 0 a 5
      int tipo = board[i][j].type;
      if ( board[i-1][j].type == tipo && board[i-2][j].type == tipo &&    //Verifica para cima
           board[i][j+1].type == tipo && board[i][j+2].type == tipo ){    //Verifica para direita
        *i_fall = i; *j_fall = j;
        *l_fall = i-3; *k_fall = j+3;
        printf("i=%d, l=%d, j=%d, k=%d\n", i, i-3, j, j+3);
        for (int k=i; k>*l_fall ;k--)                                     //Apaga joias
          board[k][j].draw = 0;
        board[i][j+1].draw = 0;
        board[i][j+2].draw = 0;
        return 1; } }

  //Verifica l invertido
  for (int i=3; i<BOARD_N+1 ;i++)       //Vai de 3 a 8
    for (int j=2; j<BOARD_N ;j++){      //Vai de 2 a 7
      int tipo = board[i][j].type;
      if ( board[i-1][j].type == tipo && board[i-2][j].type == tipo &&    //Verifica para cima
           board[i][j-1].type == tipo && board[i][j-2].type == tipo ){    //Verifica para esquerda
        *i_fall = i; *j_fall = j;
        *l_fall = i-3; *k_fall = j-3;
        printf("i=%d, l=%d, j=%d, k=%d\n", i, i-3, j, j-3);
        for (int k=i; k>*l_fall ;k--)                                     //Apaga joias
          board[k][j].draw = 0;
        board[i][j-1].draw = 0;
        board[i][j-2].draw = 0;
        return 1; } }

  //Verifica l de ponta-cabeça
  for (int i=1; i<BOARD_N-1 ;i++)       //Vai de 1 a 6
    for (int j=0; j<BOARD_N-2 ;j++){    //Vai de 0 a 5
      int tipo = board[i][j].type;
      if ( board[i+1][j].type == tipo && board[i+2][j].type == tipo &&    //Verifica para baixo
           board[i][j+1].type == tipo && board[i][j+2].type == tipo ){    //Verifica para Direita
        *i_fall = i; *j_fall = j;
        *l_fall = i+3; *k_fall = j+3;
        printf("i=%d, l=%d, j=%d, k=%d\n", i, i+3, j, j+3);
        for (int k=i; k<*l_fall ;k++)                                     //Apaga joias
          board[k][j].draw = 0;
        board[i][j+1].draw = 0;
        board[i][j+2].draw = 0;
        return 1; } }

  //Verifica l invertido e de ponta-cabeça
  for (int i=1; i<BOARD_N-1 ;i++)       //Vai de 1 a 6
    for (int j=2; j<BOARD_N ;j++){      //Vai de 0 a 5
      int tipo = board[i][j].type;
      if ( board[i+1][j].type == tipo && board[i+2][j].type == tipo &&    //Verifica para baixo
           board[i][j-1].type == tipo && board[i][j-2].type == tipo ){    //Verifica para esquerda
        *i_fall = i; *j_fall = j;
        *l_fall = i+3; *k_fall = j-3;
        printf("i=%d, l=%d, j=%d, k=%d\n", i, i+3, j, j-3);
        for (int k=i; k<*l_fall ;k++)                                     //Apaga joias
          board[k][j].draw = 0;
        board[i][j-1].draw = 0;
        board[i][j-2].draw = 0;
        return 1; } }

  return 0;
}

//Cascata de joias
int jewel_fall(JEWEL **board, STATES *global_state){
  int *i_fall = &(global_state->i_jewel_fall), *j_fall = &(global_state->j_jewel_fall);
  int *k_fall = &(global_state->k_jewel_fall), *l_fall = &(global_state->l_jewel_fall);

  switch ( global_state->fall_state ){
    case TEST_FALL:

      if ( l_test(board, global_state) ){
        global_state->fall_state = L_FALL;
        return 1;
      } else if ( horizontal_test(board, global_state) ){
        global_state->fall_state = HORIZONTAL_FALL;
        return 1;
      } else if ( vertical_test(board, global_state) ){
        global_state->fall_state = VERTICAL_FALL;
        return 1;
      }

     //Não achou inconsistencia
      global_state->board_state = BOARD_NEW_PLAY;
      return 0;
      break;

    case L_FALL:
      if ( *l_fall < *i_fall ){ //Se L em pé
        if ( *l_fall >= *i_fall-2 ){ //Caso das ultimas 2 peças a descer
          if ( board[*l_fall][*j_fall].y == board[*l_fall+1][*j_fall].y ) { //Se terminou de descer
            //Restaura y
            for (int i=*l_fall; i>-1 ;i--)
              board[i][*j_fall].y -= JEWEL_SIZE;
            //Permite renderizar
            board[*l_fall+1][*j_fall].draw = 1;
            //Atualiza tipos
            for (int i=*l_fall+1; i>0 ;i--)
              board[i][*j_fall].type = board[i-1][*j_fall].type;
            board[0][*j_fall].type = between(0, JEWEL_TYPE_N);
            //Desrenderiza linha 0
            board[0][*j_fall].draw = 0;
            //Atualiza l
            (*l_fall)++;
            //Se terminou de descer, faz outro teste
            if ( *l_fall == *i_fall )
              global_state->fall_state = TEST_FALL;

          } else {
            //Renderiza joia caindo
            for (int i=*l_fall; i>-1 ;i--)
              board[i][*j_fall].y += 1;
            //Testa render da linha 0
            if ( board[0][*j_fall].y == Y_OFFSET - 40 )
              board[0][*j_fall].draw = 1;
          }
        } else { //Caso das 3 primeiras peças a descer
          //Se terminou de descer as 3 primeiras peças
          if ( board[*l_fall][*j_fall].y == board[*l_fall+1][*j_fall].y ){
            //Restaura y
            for (int i=*l_fall; i>-1 ;i--)                //Sobe peças da coluna
              board[i][*j_fall].y -= JEWEL_SIZE;
            for (int i=*i_fall-1; i>-1 ;i--)              //Sobe peças da linha
              if ( *j_fall < *k_fall ) {
                board[i][*j_fall+1].y -= JEWEL_SIZE;
                board[i][*j_fall+2].y -= JEWEL_SIZE;
              } else {
                board[i][*j_fall-1].y -= JEWEL_SIZE;
                board[i][*j_fall-2].y -= JEWEL_SIZE; }
            //Permite renderizar
            board[*l_fall+1][*j_fall].draw = 1;
            if ( *j_fall < *k_fall ) {
              board[*i_fall][*j_fall+1].draw = 1;
              board[*i_fall][*j_fall+2].draw = 1;
            } else {
              board[*i_fall][*j_fall-1].draw = 1;
              board[*i_fall][*j_fall-2].draw = 1; }
            //Troca tipos
            for (int i=*l_fall+1; i>0 ;i--)
              board[i][*j_fall].type = board[i-1][*j_fall].type;
            board[0][*j_fall].type = between(0, JEWEL_TYPE_N);
            for (int i=*i_fall; i>0 ;i--)
              if ( *j_fall < *k_fall ) {
                board[i][*j_fall+1].type = board[i-1][*j_fall+1].type;
                board[i][*j_fall+2].type = board[i-1][*j_fall+2].type;
              } else {
                board[i][*j_fall-1].type = board[i-1][*j_fall-1].type;
                board[i][*j_fall-2].type = board[i-1][*j_fall-2].type; }
            if ( *j_fall < *k_fall ) {
              board[0][*j_fall+1].type = between(0, JEWEL_TYPE_N);
              board[0][*j_fall+2].type = between(0, JEWEL_TYPE_N);
            } else {
              board[0][*j_fall-1].type = between(0, JEWEL_TYPE_N);
              board[0][*j_fall-2].type = between(0, JEWEL_TYPE_N); }
            //Esconde linha 0
            board[0][*j_fall].draw = 0;
            if ( *j_fall < *k_fall ) {
              board[0][*j_fall+1].draw = 0;
              board[0][*j_fall+2].draw = 0;
            } else {
              board[0][*j_fall-1].draw = 0;
              board[0][*j_fall-2].draw = 0; }

            //Atualiza l
            (*l_fall)++;
          } else {  
            //Renderiza joia caindo
            for (int i=*l_fall; i>-1 ;i--)          //Desce uma joia da coluna
              board[i][*j_fall].y += 1;
            for (int i=*i_fall-1; i>-1 ;i--)        //Desce as joias da linha
              if ( *j_fall < *k_fall ) {
                board[i][*j_fall+1].y += 1;
                board[i][*j_fall+2].y += 1;
              } else {
                board[i][*j_fall-1].y += 1;
                board[i][*j_fall-2].y += 1;
              }
            //Testa render da linha 0
            if ( board[0][*j_fall].y == Y_OFFSET - 40 ){
              board[0][*j_fall].draw = 1;
              if ( *j_fall < *k_fall ) {
                board[0][*j_fall+1].draw = 1;
                board[0][*j_fall+2].draw = 1;
              } else {
                board[0][*j_fall-1].draw = 1;
                board[0][*j_fall-2].draw = 1;
              }
            }
          } //Se terminou de descer
        }

      } else { //L de ponta-cabeça
        if ( *i_fall >= *l_fall-2 ) {  //Caso das 2 peças
          //Testa se desceu a peça
          if ( board[*i_fall-1][*j_fall].y == board[*i_fall][*j_fall].y ){
            //Restaura y
            for (int i=*i_fall-1; i>-1 ;i--)
              board[i][*j_fall].y -= JEWEL_SIZE;
            //Permite renderizar
            board[*i_fall][*j_fall].draw = 1;
            //Atualiza tipos
            for (int i=*i_fall; i>0 ;i--)
              board[i][*j_fall].type = board[i-1][*j_fall].type;
            board[0][*j_fall].type = between(0, JEWEL_TYPE_N);
            //Apaga linha 0
            board[0][*j_fall].draw = 0;
            //Atualiza i
            (*i_fall)++;
            //Se terminou de descer. faz outro teste
            if ( *i_fall == *l_fall )
              global_state->fall_state = TEST_FALL;

          } else {
            //Renderiza peças
            for (int i=*i_fall-1; i>-1 ;i--)
              board[i][*j_fall].y += 1;
            //Testa render da linha 0
            if ( board[0][*j_fall].y == Y_OFFSET-40 )
              board[0][*j_fall].draw = 1;

          }
        } else {  //Caso das 3 peças
          //Testa se desceu as 3 peças
          if ( board[*i_fall-1][*j_fall].y == board[*i_fall][*j_fall].y ) {
            //Restaura y
            for (int i=*i_fall-1; i>-1 ;i--)
              if ( *j_fall < *k_fall )
                for (int j=*j_fall; j<*k_fall ;j++)
                  board[i][j].y -= JEWEL_SIZE;
              else
                for (int j=*k_fall+1; j<=*j_fall ;j++)
                  board[i][j].y -= JEWEL_SIZE;
            //Permite renderizar
            if ( *j_fall < *k_fall )
              for (int j=*j_fall; j<*k_fall ;j++)
                board[*i_fall][j].draw = 1;
            else
              for (int j=*k_fall+1; j<=*j_fall ;j++)
                board[*i_fall][j].draw = 1;
            //Troca tipos
            for (int i=*i_fall; i>0 ;i--)
              if ( *j_fall < *k_fall )
                for (int j=*j_fall; j<*k_fall ;j++)
                  board[i][j].type = board[i-1][j].type;
              else
                for (int j=*k_fall+1; j<=*j_fall ;j++)
                  board[i][j].type = board[i-1][j].type;
            if ( *j_fall < *k_fall )
              for (int j=*j_fall; j<*k_fall ;j++)
                board[0][j].type = between(0, JEWEL_TYPE_N);
            else
              for (int j=*k_fall+1; j<=*j_fall ;j++)
                board[0][j].type = between(0, JEWEL_TYPE_N);
            //Apaga linha 0
            if ( *j_fall < *k_fall )
              for (int j=*j_fall; j<*k_fall ;j++)
                board[0][j].draw = 0;
            else
              for (int j=*k_fall+1; j<=*j_fall ;j++)
                board[0][j].draw = 0;

            //Muda pra caso de 2 peças
            (*i_fall)++;
          } else {
            //Renderiza joia caindo
            for (int i=*i_fall-1; i>-1 ;i--)
              if ( *j_fall < *k_fall )
                for (int j=*j_fall; j<*k_fall ;j++)
                  board[i][j].y += 1;
              else
                for (int j=*k_fall+1; j<=*j_fall ;j++)
                  board[i][j].y += 1;
            //Testa render da linha 0
            if ( board[0][*j_fall].y == Y_OFFSET - 40){
              if ( *j_fall < *k_fall ){
                for (int j=*j_fall; j<*k_fall ;j++)
                  board[0][j].draw = 1;
              } else {
                for (int j=*k_fall+1; j<=*j_fall ;j++)
                  board[0][j].draw = 1;
              } }

          }
        } //Caso de 2 ou 3 peças
      } //L em pé ou de ponta-cabeça
      break;

    case HORIZONTAL_FALL:
      //Se joia terminou de descer
      if ( board[*i_fall-1][*j_fall].y == board[*i_fall][*j_fall].y ){
        //Restaura posição do y horizontal
        for (int i=*i_fall-1; i>-1 ;i--)
          for (int j=*j_fall; j<*k_fall ;j++)
            board[i][j].y -= JEWEL_SIZE;
        //Permite renderizar horizontal
        for (int j=*j_fall; j<*k_fall ;j++)
          board[*i_fall][j].draw = 1;
        //Atualiza tipo de doce horizontal
        for (int i=*i_fall; i>0 ;i--)
          for (int j=*j_fall; j<*k_fall ;j++)
            board[i][j].type = board[i-1][j].type;
        for (int j=*j_fall; j<*k_fall ;j++)
          board[0][j].type = between(0, JEWEL_TYPE_N);
        //Apaga linha zero
        for (int j=*j_fall; j<*k_fall ;j++)
          board[0][j].draw = 0;

        //terminou de descer, faz outro teste
        global_state->fall_state = TEST_FALL;
      } else {
        //Movimenta joias
        for (int i=*i_fall-1; i>-1 ;i--)
          for (int j=*j_fall; j<*k_fall ;j++)
            board[i][j].y += FALL_SPEED;
        //Teste de renderização da linha 0
        if ( board[0][*j_fall].y == Y_OFFSET - 40 )
          for (int j=*j_fall; j<*k_fall ;j++)
            board[0][j].draw = 1;
      }
      break;

    case VERTICAL_FALL:
      //Se terminou de descer tudo
      if ( *i_fall == *l_fall){
        board[0][*j_fall].draw = 0;             //Apaga doce da linha zero
        global_state->fall_state = TEST_FALL;   //Manda testar matchpoint
      } else
        //Se terminou de descer uma posição
        if ( board[*i_fall-1][*j_fall].y == board[*i_fall][*j_fall].y){
          //Restaura posição do y vertical
          for ( int i=*i_fall-1; i>-1 ;i--)
            board[i][*j_fall].y -= JEWEL_SIZE;
          //Permite joia em i renderizar e apaga primeira joia
          board[*i_fall][*j_fall].draw = 1; board[0][*j_fall].draw = 0;
          //Atualiza tipos
          for (int i=*i_fall; i>0 ;i--)
            board[i][*j_fall].type = board[i-1][*j_fall].type;
          board[0][*j_fall].type = between(0, JEWEL_TYPE_N);

          //Desce ponteiro do i_jewel_fall
          (*i_fall)++;
        } else {
          //Movimenta joias
          for (int i=*i_fall-1; i>-1 ;i--)
            board[i][*j_fall].y += FALL_SPEED;
          //Se peça da linha zero deve renderizar
          if ( board[0][*j_fall].y == Y_OFFSET - 40 )
              board[0][*j_fall].draw = 1;
        }
      break; }

  return 1;
}

void board_update(JEWEL **board, STATES *global_state, MOUSE *mouse){
  int *i_clk = &(mouse->i_clk), *j_clk = &(mouse->j_clk);   //Coordenadas board click
  int *i_rls = &(mouse->i_rls), *j_rls = &(mouse->j_rls);   //Coordenadas board release
  int *x_jewel_clk = &(global_state->x_jewel_clk), *y_jewel_clk = &(global_state->y_jewel_clk);
  int *x_jewel_rls = &(global_state->x_jewel_rls), *y_jewel_rls = &(global_state->y_jewel_rls);

  switch ( global_state->board_state ){
    case BOARD_NEW_PLAY:    //Carrega nova jogada
      //Calcula coordenadas do doce clicado na matriz
      *i_clk = (mouse->y_clk - Y_OFFSET)/JEWEL_SIZE;
      *j_clk = (mouse->x_clk - X_OFFSET)/JEWEL_SIZE;
      *i_rls = (mouse->y_rls - Y_OFFSET)/JEWEL_SIZE;
      *j_rls = (mouse->x_rls - X_OFFSET)/JEWEL_SIZE;

      //Se o click foi no tabuleiro
      if ((*i_clk > -1 && *i_clk < BOARD_N) && (*j_clk > -1 && *j_clk < BOARD_N))
        //Se o release foi no tabuleiro
        if ((*i_rls > -1 && *i_rls < BOARD_N) && (*j_rls > -1 && *j_rls < BOARD_N))
          //Se o release foi ao lado do click
          if ((*i_rls==*i_clk   && *j_rls==*j_clk-1) || (*i_rls==*i_clk   && *j_rls==*j_clk+1) ||
              (*i_rls==*i_clk-1 && *j_rls==*j_clk)   || (*i_rls==*i_clk+1 && *j_rls==*j_clk)){
            (mouse->i_clk)++; (mouse->i_rls)++;               //Correção da linha 0 invisível
            global_state->board_state = BOARD_SWITCH_JEWEL;   //Manda animar troca de peças
            *x_jewel_clk = board[*i_clk][*j_clk].x;           //Salva as coordenadas da troca
            *y_jewel_clk = board[*i_clk][*j_clk].y;           //Salva as coordenadas da troca
            *x_jewel_rls = board[*i_rls][*j_rls].x;           //Salva as coordenadas da troca
            *y_jewel_rls = board[*i_rls][*j_rls].y;           //Salva as coordenadas da troca
          }
      break;

    case BOARD_SWITCH_JEWEL:                                  //Troca joias de posição
      switch_jewels(board, global_state, mouse);
      break;

    case BOARD_JEWEL_FALL:                                    //Desce joias do matchpoint
      jewel_fall(board, global_state);
      *i_clk = -1; *j_clk = -1;                               //Zera mouse
      *i_rls = -1; *j_rls = -1;                               //Zera mouse
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
  STATES global_state;                                    //Variavel de maquina de estados
  state_init(&global_state);

  int ok = jewel_fall(board, &global_state);
  while ( ok )
    ok = jewel_fall(board, &global_state);

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
        board_update(board, &global_state, mouse);

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

