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
} mouse_t;

mouse_t mouse;

void mouse_init()
{
  al_hide_mouse_cursor(disp);
}

void mouse_update(ALLEGRO_EVENT* event)
{
  switch(event->type)
  {
    case ALLEGRO_EVENT_MOUSE_AXES:
      mouse.x = event->mouse.x;
      mouse.y = event->mouse.y;
      break;

    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
      mouse.x_click = event->mouse.x;
      mouse.y_click = event->mouse.y;
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

#define mouse_size 12

void mouse_pointer_init(){
  mouse_pointer.x = -mouse_size;
  mouse_pointer.y = -mouse_size;

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

// --- candy board ---

typedef struct candy
{
  int x, y;
  int type;
  ALLEGRO_BITMAP *candy_map;
} candy;

//Board_x [-- 80 (offset) -- : -- 560 (board) -- : -- 80 (offset) --]
//Board_y [-- 140 (score) -- : -- 560 (board) -- : -- 20 (offset) --]
#define x_offset 80     //Limites do tabuleiro
#define y_offset 140    //Espaçamento do score
#define candy_size 70   //Tamanho ocupado pelo doce
#define BOARD_N 8       //Tamanho da matriz
candy board[BOARD_N][BOARD_N];

ALLEGRO_BITMAP *aleat_bitmap(int type)
{
  ALLEGRO_BITMAP *bitmap;
  switch (type)
  {
    case 1:
      bitmap = al_load_bitmap("resources/candy/candy1.png");
      break;
    case 2:
      bitmap = al_load_bitmap("resources/candy/candy2.png");
      break;
    case 3:
      bitmap = al_load_bitmap("resources/candy/candy3.png");
      break;
    case 4:
      bitmap = al_load_bitmap("resources/candy/candy4.png");
      break;
    case 5:
      bitmap = al_load_bitmap("resources/candy/candy5.png");
      break;
    default:
      printf("Candy type not recognized\n");
      exit(2);
  }

  must_init(bitmap, "board_bitmap");
  return bitmap;
}

void board_init()
{
  int x_aux = 0;
  int y_aux = 0;    //140 por conta do score
  for(int i=0; i<BOARD_N ;i++){
    for(int j=0; j<BOARD_N ;j++){
      board[i][j].x = x_offset + x_aux;
      board[i][j].y = y_offset + y_aux;
      board[i][j].type = between(1, 6);
      x_aux += candy_size;   //Muda x para proximo doce da direita
    }
    x_aux = 0;
    y_aux += candy_size;
  }
}

void board_deinit()
{
  for(int i=0; i<BOARD_N ;i++)
    for(int j=0; j<BOARD_N ;j++)
      al_destroy_bitmap(board[i][j].candy_map);
}

//Verifica integridade do tabuleiro
void board_update()
{
  int i, j;
  
  i = (mouse.y_click - y_offset)/candy_size;
  j = (mouse.x_click - x_offset)/candy_size;

  mouse.x_click = -1;
  mouse.y_click = -1;


  if ((i > -1 && i < BOARD_N) && (j > -1 && j < BOARD_N))
    board[i][j].type = between(1, 6);
  //int tipo;
  ////Integridade horizontal
  //for (int i=0; i<BOARD_N ;i++)
  //  for (int j=0; j<BOARD_N-2 ;j++)
  //  {
  //    tipo = board[i][j].type;

  //    //Busca doces iguais na linha
  //    int k = j+1;
  //    int size = 1;
  //    while (board[i][k].type == tipo || k < BOARD_N)
  //    {
  //      size++;
  //      k++;
  //    }

  //    //Se houver mais de 3 peças repetidas
  //    if (size > 2){
  //      //De j até k-1 na linha i os doces são iguais
  //      int l = i;
  //      int m = j;

  //      //Desce doces
  //      for (; m<k ;m++){
  //        for (; l>0 ;l--)
  //          board[l][m].type = board[l-1][m].type;

  //        board[l][m].type = between(1, 6);
  //      }
  //    }
  //  }

  ////Integridade vertical
}

void board_draw()
{
  for(int i=0; i<BOARD_N ;i++)
    for(int j=0; j<BOARD_N ;j++){
      board[i][j].candy_map = aleat_bitmap(board[i][j].type);
      al_draw_bitmap(board[i][j].candy_map, board[i][j].x, board[i][j].y, 0);
    }
}

// --- Score ---

typedef struct score{
  int global_score;
  int score;
  int x1, y1, x2, y2;
  ALLEGRO_BITMAP *score_map;
} score;

score game_score;

void score_init()
{
  game_score.global_score = 0;
  game_score.score = 0;
  game_score.x1 = 160; game_score.y1 = 70;
  game_score.x2 = 370; game_score.y2 = 70;
  game_score.score_map = al_load_bitmap("resources/score/score.png");
}

void score_deinit()
{
  al_destroy_bitmap(game_score.score_map);
}

//void score_update()
//{
//
//}

void score_draw ()
{
  al_draw_text(font, al_map_rgb(255, 255, 255), 190, 30, 0, "SCORE       RECORD");
  //al_draw_text(font, al_map_rgb(255, 255, 255), 190, 90, 0, "GAME OVER");
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
  mouse_init();

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
  mouse_pointer_init();

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
        //board_update();
        mouse_pointer_update();

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
      mouse_pointer_draw();

      disp_post_draw();
      redraw = false;
    }
  }

  // --- Destroy themes ---
  score_deinit();
  //board_deinit();
  bg_deinit();
  mouse_pointer_deinit();

  audio_deinit();
  font_deinit();
  disp_deinit();
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);

  return 0;
}
