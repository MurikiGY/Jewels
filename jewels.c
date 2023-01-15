//Jewels by Muriki Gusmão Yamanaka

#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/bitmap_io.h>
#include <allegro5/events.h>
#include <allegro5/mouse.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <unistd.h>

//Teste de inicialização
void must_init(bool test, const char *description)
{
  if(test) return;

  printf("couldn't initialize %s\n", description);
  exit(1);
}

//Inteiros aleatorios
int between(int lo, int hi)
{
  return lo + (rand() % (hi - lo));
}

//Float aleatorios
float between_f(float lo, float hi)
{
  return lo + ((float)rand() / (float)RAND_MAX) * (hi - lo);
}

// --- display ---

#define BUFFER_W 720
#define BUFFER_H 720
#define FRAMES_N 60.0

#define DISP_SCALE 1
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;

long frames;

void disp_init()
{
  al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

  //Cria display ampliado
  disp = al_create_display(DISP_W, DISP_H);
  must_init(disp, "display");

  //Cria buffer de display
  buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
  must_init(buffer, "bitmap buffer");
}

void disp_deinit()
{
  al_destroy_bitmap(buffer);
  al_destroy_display(disp);
}

//Draw on buffer
void disp_pre_draw()
{
  al_set_target_bitmap(buffer);
}

//Commit buffer
void disp_post_draw()
{
  al_set_target_backbuffer(disp);
  al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);

  al_flip_display();
}

// --- Fonte ---

ALLEGRO_FONT* font;

void font_init()
{
  al_init_font_addon();
  al_init_ttf_addon();
  font = al_load_font("resources/fonts/MASLITE.otf", 36, 0);
  must_init(font, "Font");
}

void font_deinit()
{
  al_destroy_font(font);
}

// --- audio ---

ALLEGRO_AUDIO_STREAM *bg_music;

void audio_init()
{
  al_install_audio();
  al_init_acodec_addon();
  al_reserve_samples(128);

  //Variaveis de audio
  bg_music = al_load_audio_stream("resources/sound/music.opus", 2, 2048);
  must_init(bg_music, "bg_music");
  al_set_audio_stream_playmode(bg_music, ALLEGRO_PLAYMODE_LOOP);
  al_attach_audio_stream_to_mixer(bg_music, al_get_default_mixer());
}

void audio_deinit()
{
  al_destroy_audio_stream(bg_music);
}

// --- keyboard ---

#define KEY_SEEN     1
#define KEY_RELEASED 2
unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init()
{
  memset(key, 0, sizeof(key));
}

void keyboard_update(ALLEGRO_EVENT* event)
{
  switch(event->type)
  {
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

// --- mouse ---

typedef struct mouse
{
  int x;
  int y;
  int x_click;
  int y_click;
  int x_release;
  int y_release;
} mouse_t;

mouse_t mouse;

void mouse_init()
{
  //al_hide_mouse_cursor(disp);
}

void mouse_update(ALLEGRO_EVENT* event)
{
  switch(event->type)
  {
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

// --- Configurações de Tema ---

// --- background ---

ALLEGRO_BITMAP *bg;

void bg_init()
{
  bg = al_load_bitmap("resources/background/bg.jpg");
  must_init(bg, "background");
}

void bg_deinit()
{
  al_destroy_bitmap(bg);
}

void bg_draw()
{
  al_draw_bitmap(bg, 0, 0, 0);
}

// --- stars ---

typedef struct STAR
{
  float y;
  float speed;
} STAR;

#define STARS_N ((BUFFER_W / 2) - 1)  //Numero de estrelas
STAR stars[STARS_N];

void stars_init()
{
  for(int i = 0; i < STARS_N; i++)
  {
    stars[i].y = between_f(0, BUFFER_H);
    stars[i].speed = between_f(0.1, 1);
  }
}

void stars_update()
{
  for(int i = 0; i < STARS_N; i++)
  {
    stars[i].y += stars[i].speed;

    //Se estrela fora o buffer de display
    if(stars[i].y >= BUFFER_H)
    {
      stars[i].y = 0;
      stars[i].speed = between_f(0.1, 1);
    }
  }
}

void stars_draw()
{
  int star_x = 1;
  for(int i = 0; i < STARS_N; i++)
  {
    al_draw_pixel(star_x, stars[i].y, al_map_rgb_f(255,255,255));
    star_x += 2;
  }
}

// --- mouse pointer ---

typedef struct pointer
{
  int x, y;
  ALLEGRO_BITMAP *pntr_img;
} mouse_p;

mouse_p mouse_pointer;

#define MOUSE_SIZE 12

void mouse_pointer_init(){
  mouse_pointer.x = -MOUSE_SIZE;
  mouse_pointer.y = -MOUSE_SIZE;

  mouse_pointer.pntr_img = al_load_bitmap("resources/pointer/pointer.png");
  must_init(mouse_pointer.pntr_img, "Mouse pointer");
}

void mouse_pointer_deinit()
{
  al_destroy_bitmap(mouse_pointer.pntr_img);
}

void mouse_pointer_update()
{
  mouse_pointer.x = mouse.x;
  mouse_pointer.y = mouse.y;
}

void mouse_pointer_draw()
{
  al_draw_bitmap(mouse_pointer.pntr_img, mouse_pointer.x, mouse_pointer.y, 0);
}

// --- Score ---

typedef struct score{
  int   score;
  char  str_score[20];
  int   global_score;
  char  str_global_score[20];
  int   x1, y1, x2, y2;
  ALLEGRO_BITMAP *score_map;
} score;

score game_score;

void score_init()
{
  game_score.score = 0;
  strcpy(game_score.str_score, "0");
  game_score.global_score = 0;
  strcpy(game_score.str_global_score, "0");
  game_score.x1 = 160; game_score.y1 = 70;
  game_score.x2 = 370; game_score.y2 = 70;
  game_score.score_map = al_load_bitmap("resources/score/score.png");
}

void score_deinit()
{
  al_destroy_bitmap(game_score.score_map);
}

void score_draw ()
{
  al_draw_text(font, al_map_rgb(255, 255, 255), 190, 30, 0, "SCORE       RECORD");
  al_draw_text(font, al_map_rgb(255, 255, 255), 190, 90, 0, game_score.str_score);
}

// --- candy board ---

typedef struct candy
{
  int x, y;
  int type;
  int draw;
} candy;

//Board_x [-- 80 (offset) -- : -- 560 (board) -- : -- 80 (offset) --]
//Board_y [-- 140 (score) -- : -- 560 (board) -- : -- 20 (offset) --]
#define X_OFFSET 80                 //Limites do tabuleiro
#define Y_OFFSET 140                //Espaçamento do score
#define CANDY_SIZE 70               //Tamanho ocupado pelo doce
#define BOARD_N 8                   //Tamanho da matriz
#define CANDY_TYPE_N 5              //Tipos diferentes de doces

candy board[BOARD_N][BOARD_N];      //Matriz de doces
ALLEGRO_BITMAP *candy_sprite[5];    //Vetor de sprites
int board_switch_candy;             //Testa se deve movimentar troca de doces
int board_candy_fall;               //Testa se deve movimentar queda de doces
int i_clk, j_clk;                   //click candy
int i_rls, j_rls;                   //release candy
int x_clk, y_clk;                   //Coordenadas do click
int x_rls, y_rls;                   //Coordenadas do release
int doce_indo;                      //Doce indo ou vindo
int i_candy_fall, j_candy_fall;     //Coordenadas da queda do doce
int k_candy_fall, l_candy_fall;     //Coordenadas da queda do doce
int candy_fall_done;                //Deve descer doces

void board_init()
{
  //Inicia vetor de sprites
  candy_sprite[0] = al_load_bitmap("resources/candy/candy1.png");
  candy_sprite[1] = al_load_bitmap("resources/candy/candy2.png");
  candy_sprite[2] = al_load_bitmap("resources/candy/candy3.png");
  candy_sprite[3] = al_load_bitmap("resources/candy/candy4.png");
  candy_sprite[4] = al_load_bitmap("resources/candy/candy5.png");

  //Inicia board
  int x_aux = 0, y_aux = 0;
  for(int i=0; i<BOARD_N ;i++){
    for(int j=0; j<BOARD_N ;j++){
      board[i][j].x = X_OFFSET + x_aux;
      board[i][j].y = Y_OFFSET + y_aux;
      board[i][j].type = between(0, 5);
      board[i][j].draw = 1;
      x_aux += CANDY_SIZE;   //Muda x para proximo doce da direita
    }
    x_aux = 0;
    y_aux += CANDY_SIZE;
  }

  board_switch_candy = 0;   //Não troca doces
  board_candy_fall = 0;     //Não desce doces
  i_clk = -1; j_clk = -1;
  i_rls = -1; j_rls = -1;
  doce_indo = 0;
  candy_fall_done = 1;
  i_candy_fall = -1;
  j_candy_fall = -1;
  k_candy_fall = -1;
  l_candy_fall = -1;
}

void board_deinit()
{
  al_destroy_bitmap(candy_sprite[0]);
  al_destroy_bitmap(candy_sprite[1]);
  al_destroy_bitmap(candy_sprite[2]);
  al_destroy_bitmap(candy_sprite[3]);
  al_destroy_bitmap(candy_sprite[4]);
}

//Verifica integridade do tabuleiro, alterando as peças
int board_flush()
{ 
  int point = 0;
  //Integridade horizontal
  for (int i=0; i<BOARD_N ;i++){
    for (int j=0; j<BOARD_N-2 ;j++){
      int tipo = board[i][j].type;
      if ((board[i][j+1].type == tipo) && (board[i][j+2].type == tipo)) //Triple candy
      {
        point = 1;
        int k = j+3;
        while ((k < BOARD_N) && (board[i][k].type == tipo)) //Pega outros doces alinhados
          k++;

        //Calcula score
        game_score.score += 100 * (k - j);  //100 pts p/ cada doce repetido
        sprintf(game_score.str_score, "%d", game_score.score);

        //De j até k-1 os doces são repetidos
        int l = i, m = j;
        for (; l>0 ;l--){
          for (; m<k ;m++)
            board[l][m].type = board[l-1][m].type;
          m = j;
        }
        //Gera doces da linha zero
        for (; m<k; m++)
          board[l][m].type = between(0, 5);
      }
    }
  } //Integridade horizontal

  //Integridade vertical
  for (int i=0; i<BOARD_N-2 ;i++){
    for (int j=0; j<BOARD_N ;j++){
      int tipo = board[i][j].type;
      if (board[i+1][j].type == tipo && board[i+2][j].type == tipo) //Triple candy
      {
        point = 1;
        int k = i+3;
        while((k < BOARD_N) && (board[k][j].type == tipo))  //Pega outros doces alinhados
          k++;

        //Calcula score
        game_score.score += 100 * (k - i);  //100 pts p/ cada doce repetido
        sprintf(game_score.str_score, "%d", game_score.score);

        //Na coluna j, de i até k-1 os doces são iguais
        while (i > 0){
          board[k-1][j].type = board[i-1][j].type;
          k--; i--;
        }
        //Gera doces restantes
        while (k > 0){
          board[k-1][j].type = between(0, 5);
          k--;
        }
      }
    }
  } //Integridade vertical
  
  return point;
}

//Retorna se tem sequencia de três
int board_check()
{
  //Verifica horizontal
  for (int i=0; i<BOARD_N ;i++)
    for (int j=0; j<BOARD_N-2 ;j++)
    {
      int tipo = board[i][j].type;
      if (board[i][j+1].type == tipo && board[i][j+2].type == tipo)
        return 1;
    }

  //Verifica vertical
  for (int i=0; i<BOARD_N-2 ;i++)
    for (int j=0; j<BOARD_N ;j++)
    {
      int tipo = board[i][j].type;
      if (board[i+1][j].type == tipo && board[i+2][j].type == tipo)
        return 1;
    }

  return 0; //Não foi marcado ponto
}

//Troca doces do board de posicao
void switch_candy(int x, int y, int z, int w)
{
  int tipo = board[x][y].type;
  board[x][y].type = board[z][w].type;
  board[z][w].type = tipo;
}

//Movimenta troca de doces
void switch_movement(int direction)
{
  int horizontal, vertical;

  if ( direction > 0 ){             //Movimenta indo
    horizontal = j_rls - j_clk;
    vertical = i_rls - i_clk;
  } else {                          //Movimenta voltando
    horizontal = j_clk - j_rls;
    vertical = i_clk - i_rls;
  }

  if ( horizontal > 0 ){            
    board[i_clk][j_clk].x+=5;       //Movimenta click pra direita
    board[i_rls][j_rls].x-=5;       //Movimenta release para esquerda
  } else if ( horizontal < 0 ){
      board[i_clk][j_clk].x-=5;     //Movimenta click para esquerda
      board[i_rls][j_rls].x+=5;     //Movimenta release para direita
  } else if ( vertical > 0 ){
      board[i_clk][j_clk].y+=5;     //Movimenta click para baixo
      board[i_rls][j_rls].y-=5;     //Movimenta release para cima
  } else if ( vertical < 0 ){
      board[i_clk][j_clk].y-=5;     //Movimenta click para cima
      board[i_rls][j_rls].y+=5;     //Movimenta release para baixo
  }
}

//Apaga lixo de variaveis
void clean_variables()
{
  i_clk = -1; j_clk = -1;
  x_clk = -1; y_clk = -1;
  i_rls = -1; j_rls = -1;
  x_rls = -1; y_rls = -1;
}

void candy_drop()
{
  //Se ja desceu todos os doces
  if ( candy_fall_done ){

    //Busca por inconsistencia horizontal
    for (int i=0; i<BOARD_N ;i++)
      for (int j=0; j<BOARD_N-2 ;j++){
        int tipo = board[i][j].type;
        if ( board[i][j+1].type == tipo && board[i][j+2].type == tipo ){  //Triple candy
          int k = j+3;
          while ( board[i][k].type == tipo && k < BOARD_N )
            k++;
          i_candy_fall = i; l_candy_fall = -1;        //Coordenadas de queda
          j_candy_fall = j; k_candy_fall = k;         //Coordenadas de queda
          candy_fall_done = 0;                        //Não terminou de cair
          for (; j<k ;j++)                            //Esconde doces sequenciados
            board[i][j].draw = 0;
          return;
        }}

    //Busca por inconsistencia na vertical
    for (int i=0; i<BOARD_N-2 ;i++)
      for (int j=0; j<BOARD_N ;j++){
        int tipo = board[i][j].type;
        if ( board[i+1][j].type == tipo && board[i+2][j].type == tipo ){ //Triple candy
          int k = i+3;
          while ( board[k][j].type == tipo && k < BOARD_N )
            k++;
          i_candy_fall = i; l_candy_fall = k;         //Coordenadas de queda
          j_candy_fall = j; k_candy_fall = -1;        //Coordenadas de queda
          candy_fall_done = 0;                        //Nao terminou de cair
          for (; i<k; i++)                            //Esconde os doces sequenciados
            board[i][j].draw = 0;
          return;
        }}
    
    //Desceu todos os doces e sem mais irregularidades
    board_candy_fall = 0;

  } else {  //Testa e Desenha queda

    if ( l_candy_fall == -1 ){  //Se matchpoint horizontal
      //Se matchpoint na primeira linha, o que da bosta
      if ( i_candy_fall == 0 ){
        for (int j=j_candy_fall; j<k_candy_fall ;j++){
          board[i_candy_fall][j].type = between(0, 5);
          board[i_candy_fall][j].draw = 1;
        }
        i_candy_fall = -1; j_candy_fall = -1;
        l_candy_fall = -1; k_candy_fall = -1;
        candy_fall_done = 1;
      }
      
      if ( board[i_candy_fall-1][j_candy_fall].y == board[i_candy_fall][j_candy_fall].y ){  //Se terminou de descer
        
        //Restaura posição y horizontal
        for (int i=i_candy_fall-1; i>-1 ;i--)
          for (int j=j_candy_fall; j<k_candy_fall ;j++)
            board[i][j].y-=CANDY_SIZE;

        //Permite renderizar horizontal
        for (int j=j_candy_fall; j<k_candy_fall ;j++)
            board[i_candy_fall][j].draw = 1;

        //Atualiza tipo do doce horizontal
        for (int i=i_candy_fall; i>0 ;i--)
          for (int j=j_candy_fall; j<k_candy_fall ;j++)
            board[i][j].type = board[i-1][j].type;
        for (int j=j_candy_fall; j<k_candy_fall ;j++)
          board[0][j].type = between(0, 5);
        
        i_candy_fall = -1; j_candy_fall = -1;
        l_candy_fall = -1; k_candy_fall = -1;
        candy_fall_done = 1;  //Termina de cair doces, checa integridade

      } else {  //Desce doces horizontal

        for (int i=i_candy_fall-1; i>-1 ;i--)
          for (int j=j_candy_fall; j<k_candy_fall ;j++)
            board[i][j].y+=5;

      }

    } else if ( k_candy_fall == -1 ){ //Se matchpoint vertical

      //Testando a bosta do matchpoint ser na linha zero
      if ( i_candy_fall == 0 ){
        for (int i=i_candy_fall; i<l_candy_fall ;i++){
          board[i][j_candy_fall].draw = 1;
          board[i][j_candy_fall].type = between(0, 5);
        }
        i_candy_fall = -1; j_candy_fall = -1;
        l_candy_fall = -1; k_candy_fall = -1;
        candy_fall_done = 1;
      }
      
      if ( board[i_candy_fall-1][j_candy_fall].y == board[l_candy_fall-1][j_candy_fall].y ){  //Se terminou de descer

        //Restaura posição do y vertical
        for (int i=i_candy_fall-1; i>-1 ;i--)
          board[i][j_candy_fall].y -= CANDY_SIZE * (l_candy_fall - i_candy_fall); //Restaura y
        
        //Permite renderizar vertical
        for (int i=i_candy_fall; i<l_candy_fall ;i++)
          board[i][j_candy_fall].draw = 1;

        //Atualiza tipo de doce vertical
        int n_candy = l_candy_fall - i_candy_fall;
        for (int i=l_candy_fall-1; i>-1 ;i--)
          if ( i > n_candy-1 )
            board[i][j_candy_fall].type = board[--i_candy_fall][j_candy_fall].type;
          else
            board[i][j_candy_fall].type = between(0, 5);

        i_candy_fall = -1; j_candy_fall = -1;
        l_candy_fall = -1; k_candy_fall = -1;
        candy_fall_done = 1;  //Termina de cair doces, checa integridade

        return;
      } else {  //Desce doces vertical
        for (int i=i_candy_fall-1; i>-1 ;i--)
          board[i][j_candy_fall].y+=5;
      }
    }
  }
}

void board_update()
{
  //Se troca de doces
  if ( board_switch_candy ){
    //Se doce indo
    if ( doce_indo ){
      //Testa se terminou de movimentar
      if ( board[i_clk][j_clk].x == x_rls && board[i_clk][j_clk].y == y_rls &&
           board[i_rls][j_rls].x == x_clk && board[i_rls][j_rls].y == y_clk ){
        switch_candy(i_clk, j_clk, i_rls, j_rls); //Troca tipo dos doces
        //Se marcou ponto
        if ( board_check() ){
          doce_indo = 0;                    //Doce não esta mais indo
          board_switch_candy = 0;           //Desativa troca de doces
          board_candy_fall = 1;             //Ativa descida dos doces
          board[i_clk][j_clk].x = x_clk;    //Restaura posições originais    
          board[i_clk][j_clk].y = y_clk;    //Restaura posições originais
          board[i_rls][j_rls].x = x_rls;    //Restaura posições originais
          board[i_rls][j_rls].y = y_rls;    //Restaura posições originais
          clean_variables();
        } else {  //Não marcou ponto
          switch_candy(i_clk, j_clk, i_rls, j_rls); //Troca tipo dos doces
          doce_indo = 0;
        }
      }
      //Movimenta doce indo
      switch_movement(1);
    } else {  //Doce voltando
      //Se terminou de voltar a posição original
      if ( board[i_clk][j_clk].x == x_clk && board[i_clk][j_clk].y == y_clk &&
           board[i_rls][j_rls].x == x_rls && board[i_rls][j_rls].y == y_rls ){
        board_switch_candy = 0;         //Terminou de trocar, pegar mouse de novo
        clean_variables();
      }
      //Movimenta doce voltando
      switch_movement(-1);
    }

  } else if ( board_candy_fall ){   //Queda dos doces

    candy_drop();
    //while ( board_check() )
    //  board_flush();

    //board_candy_fall = 0;           //Terminou de descer doces

  } else {  //Pega informações do mouse
    //Calcula coordenadas do doce clicado na matriz
    i_clk = (mouse.y_click - Y_OFFSET)/CANDY_SIZE;
    j_clk = (mouse.x_click - X_OFFSET)/CANDY_SIZE;
    i_rls = (mouse.y_release - Y_OFFSET)/CANDY_SIZE;
    j_rls = (mouse.x_release - X_OFFSET)/CANDY_SIZE;

    //Se o click foi no tabuleiro
    if ((i_clk > -1 && i_clk < BOARD_N) && (j_clk > -1 && j_clk < BOARD_N))
      //Se o release foi no tabuleiro
      if ((i_rls > -1 && i_rls < BOARD_N) && (j_rls > -1 && j_rls < BOARD_N))
        //Se o release foi ao lado do click
        if ((i_rls==i_clk && j_rls==j_clk-1) || (i_rls==i_clk && j_rls==j_clk+1) ||
            (i_rls==i_clk-1 && j_rls==j_clk) || (i_rls==i_clk+1 && j_rls==j_clk)){
          board_switch_candy = 1;               //Troca doces
          x_clk = board[i_clk][j_clk].x;        //Salva posição do click
          y_clk = board[i_clk][j_clk].y;        //Salva posição do click
          x_rls = board[i_rls][j_rls].x;        //Salva Posição do release
          y_rls = board[i_rls][j_rls].y;        //Salva Posição do release
          doce_indo = 1;
        }

  }
}

void board_draw()
{
  for(int i=0; i<BOARD_N ;i++)
    for(int j=0; j<BOARD_N ;j++)
      if ( board[i][j].draw )
        al_draw_bitmap(candy_sprite[board[i][j].type], board[i][j].x, board[i][j].y, 0);

}


// --- main ---

int main()
{
  srand(time(NULL));    //Raiz aleatoria

  //Instala allegro e teclado
  must_init(al_init(), "allegro");
  must_init(al_install_keyboard(), "keyboard");
  must_init(al_install_mouse(), "mouse");

  //Variavel de tempo
  ALLEGRO_TIMER* timer = al_create_timer(1.0 / FRAMES_N);   //Inicia variavel de tempo
  must_init(timer, "timer");
  //Variavel de eventos
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  must_init(queue, "queue");

  disp_init();        //Inicia Display
  audio_init();       //Inicia Audio
  font_init();        //Inicia fonte
  keyboard_init();    //Zera vetor de teclado
  //mouse_init();

  must_init(al_init_image_addon(), "image");    //Permite adição de imagens
  must_init(al_init_primitives_addon(), "primitives");    //Permite adição de primitivas

  //Instala audio e acodecs
  must_init(al_install_audio(), "audio");
  must_init(al_init_acodec_addon(), "audio codecs");
  must_init(al_reserve_samples(16), "reserve samples");

  //Marcadores de evento
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_mouse_event_source());
  al_register_event_source(queue, al_get_display_event_source(disp));
  al_register_event_source(queue, al_get_timer_event_source(timer));

  // --- Inicia estruturas ---
  bg_init();
  stars_init();
  board_init();
  while ( board_check() )
    board_flush();
  score_init();
  //mouse_pointer_init();

  frames = 0;

  bool done = false;
  bool redraw = true;
  ALLEGRO_EVENT event;

  al_start_timer(timer);
  while(1)
  {
    al_wait_for_event(queue, &event);  //Lista de eventos

    switch(event.type)
    {
      case ALLEGRO_EVENT_TIMER:
        //Update functions
        stars_update();
        board_update();
        //mouse_pointer_update();

        if(key[ALLEGRO_KEY_ESCAPE])
          done = true;

        redraw = true;
        frames++;
        break;

      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        done = true;
        break;
    }

    if(done)
      break;

    keyboard_update(&event);
    mouse_update(&event);

    if(redraw && al_is_event_queue_empty(queue))
    {
      disp_pre_draw();

      //Draw functions
      bg_draw();
      stars_draw();
      board_draw();
      score_draw();
      //mouse_pointer_draw();

      disp_post_draw();
      redraw = false;
    }
  }

  // --- Destroy themes ---
  score_deinit();
  board_deinit();
  bg_deinit();
  //mouse_pointer_deinit();

  audio_deinit();
  font_deinit();
  disp_deinit();
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);

  return 0;
}
