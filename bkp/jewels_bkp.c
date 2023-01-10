#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/bitmap.h>
#include <allegro5/keycodes.h>
#include <allegro5/color.h>
#include <allegro5/mouse_cursor.h>
#include <allegro5/events.h>
#include <allegro5/mouse.h>
#include <allegro5/display.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define DISPLAY_W 1100
#define DISPLAY_H 600
#define FPS 60

//Função de inicialização
void must_init(bool test, const char *description)
{
  if(test) return;

  printf("couldn't initialize %s\n", description);
  exit(1);
}

/* Retorna um valor aleatorio entre ini e fim */
int aleat (int ini, int fim){
  return ini + (rand() % (fim - ini + 1));
}


int main()
{
  srand(time(NULL));    //Raiz aleatoria

  //--------------------------- Inicia Allegro -------------------------//
  //Inicializa allegro, teclado, mouse e Imagens
  must_init(al_init(), "allegro");
  must_init(al_install_keyboard(), "keyboard");
  must_init(al_install_mouse(), "mouse");
  must_init(al_init_image_addon(), "image addon");
  must_init(al_init_font_addon(), "font addon");
  must_init(al_init_ttf_addon(), "ttf_addon");
  must_init(al_init_primitives_addon(), "prititives");
  must_init(al_install_audio(), "audio");
  must_init(al_init_acodec_addon(), "audio codecs");
  must_init(al_reserve_samples(16), "reserve samples");

  //Inicializa variaveis temporizador, eventos, display e fonte
  ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
  must_init(timer, "timer");
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  must_init(queue, "queue");
  ALLEGRO_DISPLAY* disp = al_create_display(DISPLAY_W, DISPLAY_H);
  must_init(disp, "display");
  ALLEGRO_FONT* font = al_load_font("resources/fonts/Unique.ttf", 36, ALLEGRO_TTF_MONOCHROME);
  must_init(font, "font");
  ALLEGRO_BITMAP *background = al_load_bitmap("resources/background/background.jpg");
  must_init(background, "background");
  ALLEGRO_SAMPLE *elephant = al_load_sample("resources/sound/audio_elephant.wav");
  must_init(elephant, "elephant");
  ALLEGRO_AUDIO_STREAM* music = al_load_audio_stream("resources/sound/music.opus", 2, 2048);
  must_init(music, "music");

  //Cores
  ALLEGRO_COLOR electricBlue = al_map_rgb(44, 117, 255);

  //Registradores de eventos
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_mouse_event_source());
  al_register_event_source(queue, al_get_display_event_source(disp));
  al_register_event_source(queue, al_get_timer_event_source(timer));

  //Funções de configuração
  al_set_window_title(disp, "JEWELS");              //Titulo da janela do jogo
  al_set_new_display_flags(ALLEGRO_NOFRAME);
  al_grab_mouse(disp);                              //Limita mouse dentro da janela
  al_hide_mouse_cursor(disp);                       //Esconde ponteiro do mouse
  al_set_audio_stream_playmode(music, ALLEGRO_PLAYMODE_LOOP);
  al_attach_audio_stream_to_mixer(music, al_get_default_mixer());

  //--------------------------- Inicia Allegro -------------------------//

  bool done = false;    //Fim de jogo?
  bool redraw = true;   //flush do display
  ALLEGRO_EVENT event;  //Sinalizador de evento

  //Variaveis de estado
  int x = 10, y = 10;   //Retangulo

  #define KEY_SEEN     1
  #define KEY_RELEASED 2

  unsigned char key[ALLEGRO_KEY_MAX];
  memset(key, 0, sizeof(key));

  al_start_timer(timer);
  while(1)
  {
    al_wait_for_event(queue, &event);   //Espera por um evento

    switch(event.type)
    {
      case ALLEGRO_EVENT_TIMER:   //Frame time
        if(key[ALLEGRO_KEY_ESCAPE])   //Esc precionado
          done = true;

        for(int i = 0; i < ALLEGRO_KEY_MAX; i++)    //Mapeamento das teclas
          key[i] &= KEY_SEEN;

        redraw = true;    //Atualizar display
        break;

      //TECLADO
      case ALLEGRO_EVENT_KEY_DOWN:    //Tecla precionada
        key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
        if(event.keyboard.keycode == ALLEGRO_KEY_E)
          al_play_sample(elephant, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        if(event.keyboard.keycode != ALLEGRO_KEY_ESCAPE)
        break;
      case ALLEGRO_EVENT_KEY_UP:    //Tecla solta
        key[event.keyboard.keycode] &= KEY_RELEASED;
        break;

      //MOUSE
      case ALLEGRO_EVENT_MOUSE_AXES:    //Mouse movimentado
        x = event.mouse.x;
        y = event.mouse.y;
        break;

      case ALLEGRO_EVENT_DISPLAY_CLOSE:   //Botao de fechar janela
        done = true;
        break;
    }


    //Termina o jogo
    if(done)
      break;


    //Renderiza
    if(redraw && al_is_event_queue_empty(queue))
    {
      //al_clear_to_color(al_map_rgb(0, 0, 0));   //Fundo preto
      al_draw_bitmap(background, 0, 0, 0);
      //al_draw_text(font, al_map_rgb(255, 255, 255), 25, 12, 0, "!! JEWELS !!");
      al_draw_text(font, al_map_rgb(255, 255, 255), 640/2, 480/2, ALLEGRO_ALIGN_CENTER, "Knock knock, it's Nelly");

      //Desenha retangulo
      al_draw_filled_rectangle(x, y, x+10, y+10, electricBlue);

      //Commita imagem
      al_flip_display();

      redraw = false;
    }
  }

  //Destroi o jogo
  al_destroy_font(font);
  al_destroy_display(disp);
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);
  al_destroy_bitmap(background);
  al_destroy_sample(elephant);
  al_destroy_audio_stream(music);

  return 0;
}
