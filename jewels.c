//Jewels by Muriki Gusmão Yamanaka

#include <allegro5/bitmap.h>
#include <allegro5/events.h>
#include <allegro5/mouse.h>
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
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

#define BUFFER_W 960
#define BUFFER_H 720

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

// --- audio ---

//ALLEGRO_SAMPLE* sample_shot;
//ALLEGRO_SAMPLE* sample_explode[2];

void audio_init()
{
  al_install_audio();
  al_init_acodec_addon();
  al_reserve_samples(128);

  //Carregar variaveis de audio
}

//void audio_deinit()
//{
//    al_destroy_sample(sample_shot);
//    al_destroy_sample(sample_explode[0]);
//    al_destroy_sample(sample_explode[1]);
//}

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
} mouse_p;

ALLEGRO_BITMAP *pntr_img;
mouse_p mouse_pointer;

#define mouse_size 10

void mouse_pointer_init(){
  mouse_pointer.x = -mouse_size;
  mouse_pointer.y = -mouse_size;

  pntr_img = al_load_bitmap("resources/pointer/pointer.png");
  must_init(pntr_img, "Mouse pointer");
}

void mouse_pointer_deinit()
{
  al_destroy_bitmap(pntr_img);
}

void mouse_pointer_update(){
  mouse_pointer.x = mouse.x;
  mouse_pointer.y = mouse.y;
}

void mouse_pointer_draw()
{
  al_draw_bitmap(pntr_img, mouse_pointer.x, mouse_pointer.y, 0);
}


// --- main ---

int main()
{
  //Instala allegro e teclado
  must_init(al_init(), "allegro");
  must_init(al_install_keyboard(), "keyboard");
  must_init(al_install_mouse(), "mouse");

  //Variavel de tempo
  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);   //Inicia variavel de tempo
  must_init(timer, "timer");
  //Variavel de eventos
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  must_init(queue, "queue");

  disp_init();        //Inicia Display
  audio_init();       //Inicia Audio
  keyboard_init();    //Zera vetor de teclado
  mouse_init();

  //Permite adição de imagens
  must_init(al_init_image_addon(), "image");

  //Permite adição de primitivas
  must_init(al_init_primitives_addon(), "primitives");

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
      mouse_pointer_draw();

      disp_post_draw();
      redraw = false;
    }
  }

  // --- Destroy themes ---
  bg_deinit();
  mouse_pointer_deinit();

  //audio_deinit();
  disp_deinit();
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);

  return 0;
}
