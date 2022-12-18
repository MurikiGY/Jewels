#include <allegro5/display.h>
#include <allegro5/events.h>
#include <allegro5/mouse.h>
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

#define DISPLAY_W 1200
#define DISPLAY_H 650
#define FPS 60

//Função de inicialização
void must_init(bool test, const char *description)
{
  if(test) return;

  printf("couldn't initialize %s\n", description);
  exit(1);
}

int main()
{
    //Inicializa allegro, teclado, mouse e Imagens
  must_init(al_init(), "allegro");
  must_init(al_install_keyboard(), "keyboard");
  must_init(al_install_mouse(), "mouse");
  must_init(al_init_image_addon(), "image addon");
  must_init(al_init_font_addon(), "font addon");
  must_init(al_init_ttf_addon(), "ttf_addon");

  al_set_new_display_flags(ALLEGRO_NOFRAME);

  //Inicializa temporizador, eventos, display e fonte
  ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
  must_init(timer, "timer");
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  must_init(queue, "queue");
  ALLEGRO_DISPLAY* disp = al_create_display(DISPLAY_W, DISPLAY_H);
  must_init(disp, "display");
  al_set_window_title(disp, "JEWELS");
  ALLEGRO_FONT* font = al_load_font("resources/Unique.ttf", 36, NULL);
  must_init(font, "font");

  //Sinalizadores de eventos
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_mouse_event_source());
  al_register_event_source(queue, al_get_display_event_source(disp));
  al_register_event_source(queue, al_get_timer_event_source(timer));

  bool done = false;    //Booleano de execução
  bool redraw = true;   //flush do display
  ALLEGRO_EVENT event;  //Sinalizador de evento

  al_start_timer(timer);
  while(1)
  {
    al_wait_for_event(queue, &event);   //Espera por um evento
    switch(event.type)
    {
      case ALLEGRO_EVENT_TIMER:
        // game logic goes here.
        redraw = true;
        break;

      case ALLEGRO_EVENT_KEY_DOWN:
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        done = true;
        break;
    }

    //Termina o jogo
    if(done)
      break;

    //Renderiza
    if(redraw && al_is_event_queue_empty(queue))
    {
      al_clear_to_color(al_map_rgb(0, 0, 0));
      al_draw_text(font, al_map_rgb(255, 255, 255), 500, 25, 0, "!! JEWELS !!");

      al_flip_display();

      redraw = false;
    }
  }

  //Destroi o jogo
  al_destroy_font(font);
  al_destroy_display(disp);
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);

  return 0;
}
