#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/events.h>
#include <allegro5/mouse.h>
#include <allegro5/display.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

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

  //-------------------- Inicia allegro ---------------------//
  //Inicializa allegro, teclado, mouse e Imagens
  must_init(al_init(), "allegro");
  must_init(al_install_keyboard(), "keyboard");
  must_init(al_install_mouse(), "mouse");
  must_init(al_init_image_addon(), "image addon");
  must_init(al_init_font_addon(), "font addon");
  must_init(al_init_ttf_addon(), "ttf_addon");
  must_init(al_init_primitives_addon(), "prititives");

  //Inicializa variaveis temporizador, eventos, display e fonte
  ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
  must_init(timer, "timer");
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  must_init(queue, "queue");
  ALLEGRO_DISPLAY* disp = al_create_display(DISPLAY_W, DISPLAY_H);
  must_init(disp, "display");
  ALLEGRO_FONT* font = al_load_font("resources/Unique.ttf", 36, ALLEGRO_TTF_MONOCHROME);
  must_init(font, "font");

  //Funções de configuração
  //al_set_new_display_flags(ALLEGRO_NOFRAME);
  al_set_window_title(disp, "JEWELS");

  //Sinalizadores de eventos
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_mouse_event_source());
  al_register_event_source(queue, al_get_display_event_source(disp));
  al_register_event_source(queue, al_get_timer_event_source(timer));

  //--------------------------- Inicia Allegro -------------------------//

  bool done = false;    //Fim de jogo?
  bool redraw = true;   //flush do display
  ALLEGRO_EVENT event;  //Sinalizador de evento

  //Variaveis de estado
  int x = 10, y = 10;   //Retangulo

  al_start_timer(timer);
  while(1)
  {
    al_wait_for_event(queue, &event);   //Espera por um evento

    switch(event.type)
    {
      //Tempo de um frame
      case ALLEGRO_EVENT_TIMER:
        if (y < DISPLAY_H - 10)
          y++;
        redraw = true;
        break;

      //O mouse foi movimentado
      case ALLEGRO_EVENT_MOUSE_AXES:
        break;

      case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        x = event.mouse.x;
        y = event.mouse.y;
        break;

      case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
        break;

      //Uma tecla do teclado foi pressionada
      case ALLEGRO_EVENT_KEY_DOWN:
      
      //Botão de fechar foi pressionado
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
      al_draw_text(font, al_map_rgb(255, 255, 255), 25, 12, 0, "!! JEWELS !!");

      al_draw_filled_rectangle(x, y, x+50, y+10, al_map_rgba_f(0, 0, 0.5, 0.5));

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

  return 0;
}
