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
  int score;
  int global_score;
  int x1, y1, x2, y2;
  ALLEGRO_BITMAP *score_map;
} score;

score game_score;

void score_init()
{
  game_score.score = 0;
  game_score.global_score = 0;
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
  //al_draw_text(font, al_map_rgb(255, 255, 255), 190, 90, 0, "GAME OVER");
}

// --- candy board ---

typedef struct candy
{
  int x, y;
  int type;
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
  for(int i=0; i<BOARD_N ;i++)
  {
    for(int j=0; j<BOARD_N ;j++)
    {
      board[i][j].x = X_OFFSET + x_aux;
      board[i][j].y = Y_OFFSET + y_aux;
      board[i][j].type = between(0, 5);
      x_aux += CANDY_SIZE;   //Muda x para proximo doce da direita
    }
    x_aux = 0;
    y_aux += CANDY_SIZE;
  }
}

void board_deinit()
{
  al_destroy_bitmap(candy_sprite[0]);
  al_destroy_bitmap(candy_sprite[1]);
  al_destroy_bitmap(candy_sprite[2]);
  al_destroy_bitmap(candy_sprite[3]);
  al_destroy_bitmap(candy_sprite[4]);
}

//Verifica integridade do tabuleiro
int board_check()
{ 
  int point = 0;
  //Integridade horizontal
  for (int i=0; i<BOARD_N ;i++)
  {
    for (int j=0; j<BOARD_N-2 ;j++)
    {
      int tipo = board[i][j].type;
      if ((board[i][j+1].type == tipo) && (board[i][j+2].type == tipo)) //Triple candy
      {
        point = 1;
        int k = j+3;
        while ((k < BOARD_N) && (board[i][k].type == tipo)) //Pega outros doces alinhados
          k++;

        //Calcula score
        game_score.score += 100 * (k - j);  //100 pts p/ cada doce repetido

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
  for (int i=0; i<BOARD_N-2 ;i++)
  {
    for (int j=0; j<BOARD_N ;j++)
    {
      int tipo = board[i][j].type;
      if (board[i+1][j].type == tipo && board[i+2][j].type == tipo) //Triple candy
      {
        point = 1;
        int k = i+3;
        while((k < BOARD_N) && (board[k][j].type == tipo))  //Pega outros doces alinhados
          k++;

        //Na coluna j, de i até k-1 os doces são iguais
        while (i > 0)
        {
          board[k-1][j].type = board[i-1][j].type;
          k--;
          i--;
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

//Troca doces do board de posicao
void switch_candy(int x, int y, int z, int w)
{
  int tipo = board[x][y].type;
  board[x][y].type = board[z][w].type;
  board[z][w].type = tipo;
}

void board_update()
{
  int i_click, j_click;
  int i_release, j_release;
  
  //Calcula coordenadas do doce clicado na matriz
  i_click = (mouse.y_click - Y_OFFSET)/CANDY_SIZE;
  j_click = (mouse.x_click - X_OFFSET)/CANDY_SIZE;
  i_release = (mouse.y_release - Y_OFFSET)/CANDY_SIZE;
  j_release = (mouse.x_release - X_OFFSET)/CANDY_SIZE;

  int make_point = 0;
  //Se o click foi no tabuleiro
  if ((i_click > -1 && i_click < BOARD_N) && (j_click > -1 && j_click < BOARD_N))
  {
    //Se o release foi no tabuleiro
    if ((i_release > -1 && i_release < BOARD_N) && (j_release > -1 && j_release < BOARD_N))
    {
      //Se o release foi ao lado do click
      if ((i_release==i_click && j_release==j_click-1) || (i_release==i_click && j_release==j_click+1) ||
          (i_release==i_click-1 && j_release==j_click) || (i_release==i_click+1 && j_release==j_click))
      {
        //Troca doces
        switch_candy(i_click, j_click, i_release, j_release);

        //Testa se foi marcado ponto
        make_point = board_check();

        //Se não foi marcado ponto, desfaz troca
        if (!make_point)
          switch_candy(i_click, j_click, i_release, j_release);
      }
    }
  }

  board_check();
}

void board_draw()
{
  for(int i=0; i<BOARD_N ;i++)
    for(int j=0; j<BOARD_N ;j++)
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
        //printf("Score: %d\n", game_score.score);

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
