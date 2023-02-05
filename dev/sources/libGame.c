#include "libGame.h"
#include "utils.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/bitmap.h>

// --- AUDIO ---
//Inicia variaveis de audio
void audio_init(AUDIO_T **audio){
  //Inicia ponteiro de audios
  *audio = malloc( sizeof(AUDIO_T));
  must_init(*audio, "Game Audio");

  (*audio)->bg_music = al_load_audio_stream("../resources/sound/Haggstrom.opus", 2, 2048);
  must_init((*audio)->bg_music, "Background music");

  (*audio)->fall_snd_effect = al_load_sample("../resources/sound/rock_fall.wav");
  must_init((*audio)->fall_snd_effect, "Fall effect");

  (*audio)->special1_snd_effect = al_load_sample("../resources/sound/special_explosion1.wav");
  must_init((*audio)->special1_snd_effect, "Special1 effect");

  (*audio)->special2_snd_effect = al_load_sample("../resources/sound/special_explosion2.wav");
  must_init((*audio)->special2_snd_effect, "Special2 effect");
}
//Destroi variaveis de audio
void audio_deinit(AUDIO_T **audio){
  al_destroy_audio_stream((*audio)->bg_music);
  al_destroy_sample((*audio)->fall_snd_effect);
  al_destroy_sample((*audio)->special1_snd_effect);
  al_destroy_sample((*audio)->special2_snd_effect);
  free(*audio);
}


// --- FONTE ---
//Inicializa variavel de fonte
void font_init(FONT_T **font){
  (*font) = malloc( sizeof(FONT_T) );
  must_init((*font), "Game Font");

  (*font)->score_font = al_load_font("../resources/fonts/MASLITE.otf", 36, 0);
  must_init((*font)->score_font, "Fonte MASLITE");
}
//Destroi variavel de fonte
void font_deinit(FONT_T **font){
  al_destroy_font((*font)->score_font);
  free(*font);
}


// --- BACKGROUND ---
//Inicia background
void background_init(ALLEGRO_BITMAP **bg){
  *bg = al_load_bitmap("../resources/background/rock_bg.png");
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
//Inicia score
void score_init(SCORE **score){
  *score = malloc( sizeof(SCORE) );
  must_init(*score, "Score");

  //Inicia
  (*score)->local_score = 0;
  (*score)->global_score = 0;

  //Inicia global score
  FILE *filename = fopen("../resources/score/score_history.txt", "a+");
  must_init(filename, "Global Score");

  //Pega maior score
  int score_max = 0;
  while ( fscanf(filename, "%d", &score_max) != EOF )
    if ( score_max >= (*score)->global_score )
      (*score)->global_score = score_max;
  fclose(filename);
}
//Destroi score
void score_deinit(SCORE **score){
  FILE *filename = fopen("../resources/score/score_history.txt", "a");
  must_init(filename, "Save game score");

  printf("score: %d\n", (*score)->local_score);
  printf("score: %d\n", (*score)->global_score);
  if ( (*score)->local_score > (*score)->global_score ){
    fprintf(filename, "%d\n",(*score)->local_score);
  }

  fclose(filename);
  free(*score);
}
//Renderiza score
void score_draw (SCORE *score, ALLEGRO_FONT *font){
  char str_score[20];
  char str_global_score[20];

  snprintf(str_score, 20, "%d", score->local_score);
  snprintf(str_global_score, 20, "%d", score->global_score);

  al_draw_text(font, al_map_rgb(255, 255, 255), DISP_W/4.0 + 40   , 30, ALLEGRO_ALIGN_CENTER, "SCORE");
  al_draw_text(font, al_map_rgb(255, 255, 255), 3*DISP_W/4.0 - 40 , 30, ALLEGRO_ALIGN_CENTER, "RECORD");
  al_draw_text(font, al_map_rgb(255, 255, 255), DISP_W/4.0 + 40   , 90, ALLEGRO_ALIGN_CENTER, str_score);
  al_draw_text(font, al_map_rgb(255, 255, 255), 3*DISP_W/4.0 - 40 , 90, ALLEGRO_ALIGN_CENTER, str_global_score);
}


  // --- BOARD ---
//Inicia board
JEWEL **board_init (ALLEGRO_BITMAP **candy_sprite){
  //Inicia vetor de sprites
  candy_sprite[0]  = al_load_bitmap("../resources/sprites/rocks/rock1.png");   
  candy_sprite[1]  = al_load_bitmap("../resources/sprites/rocks/rock2.png");   
  candy_sprite[2]  = al_load_bitmap("../resources/sprites/rocks/rock3.png");   
  candy_sprite[3]  = al_load_bitmap("../resources/sprites/rocks/rock4.png");   
  candy_sprite[4]  = al_load_bitmap("../resources/sprites/rocks/rock5.png");   
  candy_sprite[5]  = al_load_bitmap("../resources/sprites/rocks/rock6.png");   
  candy_sprite[6]  = al_load_bitmap("../resources/sprites/rocks/special11.png");
  candy_sprite[7]  = al_load_bitmap("../resources/sprites/rocks/special12.png");
  candy_sprite[8]  = al_load_bitmap("../resources/sprites/rocks/special13.png");
  candy_sprite[9]  = al_load_bitmap("../resources/sprites/rocks/special14.png");
  candy_sprite[10] = al_load_bitmap("../resources/sprites/rocks/special15.png");
  candy_sprite[11] = al_load_bitmap("../resources/sprites/rocks/special16.png");
  candy_sprite[12] = al_load_bitmap("../resources/sprites/rocks/special21.png");
  candy_sprite[13] = al_load_bitmap("../resources/sprites/rocks/special22.png");
  candy_sprite[14] = al_load_bitmap("../resources/sprites/rocks/special23.png");
  candy_sprite[15] = al_load_bitmap("../resources/sprites/rocks/special24.png");
  candy_sprite[16] = al_load_bitmap("../resources/sprites/rocks/special25.png");
  candy_sprite[17] = al_load_bitmap("../resources/sprites/rocks/special26.png");

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

//Renderiza board
void board_draw(JEWEL **board, ALLEGRO_BITMAP **piece_sprite){
  //Percore board
  for(int i=0; i<BOARD_N+1 ;i++)    //Ignora a primeira fileira de peças
    for(int j=0; j<BOARD_N ;j++)
      if ( board[i][j].draw ){
        al_draw_bitmap(piece_sprite[board[i][j].type], board[i][j].x, board[i][j].y, 0);
        board[i][j].special_gen_flag = 0;
      }
}


// --- Maquina de estados ---
//Inicia maquina de estados
void state_init(STATES *global_states){
  global_states->board_state  = BOARD_NEW_PLAY;
  global_states->jewel_state  = JEWEL_GO;
  global_states->fall_state   = TEST_FALL;
  global_states->x_jewel_clk  = -1; global_states->y_jewel_clk  = -1;     //Troca de joias   
  global_states->x_jewel_rls  = -1; global_states->y_jewel_rls  = -1;     //Troca de joias
  global_states->i_jewel_fall = -1;                                       //Queda das joias
  global_states->fall_flag    = -1;
}
