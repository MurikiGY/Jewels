#ifndef _ALLEGROENGINE_
#define _ALLEGROENGINE_

//Struct com as variaveis necessárias ao allegro
typedef struct engine_allegro {
  ALLEGRO_TIMER         *timer;                 //Variavel de tempo
  ALLEGRO_EVENT_QUEUE   *queue;                 //Variavel de eventos
  ALLEGRO_BITMAP        *buffer;                //Variavel de display em memoria
  ALLEGRO_DISPLAY       *display;               //Variavel de display
  unsigned char         key[ALLEGRO_KEY_MAX];   //Variavel do teclado
  MOUSE                 *mouse;                 //Variavel do mouse
} ALLEGRO_ENGINE;



  



#endif
