#ifndef _ALLEGROENGINE_
#define _ALLEGROENGINE_

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


// --- ALLEGRO SYSTEM ---

// --- DISPLAY ---
#define BUFFER_W 720
#define BUFFER_H 720
#define FRAMES_N 60.0

#define DISP_SCALE 1
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

// --- KEYBOARD ---
#define KEY_SEEN     1
#define KEY_RELEASED 2


// --- MOUSE ---
typedef struct mouse {
  int x, y;                   //Bit do movimento
  int x_clk, y_clk;           //Bit do click
  int i_clk, j_clk;           //Posição do click
  int x_rls, y_rls;           //Bit do release
  int i_rls, j_rls;           //Posição do release
} MOUSE;


//Struct com as variaveis necessárias ao allegro
typedef struct engine_allegro {
  ALLEGRO_TIMER         *timer;                 //Variavel de tempo
  ALLEGRO_EVENT_QUEUE   *queue;                 //Variavel de eventos
  ALLEGRO_BITMAP        *buffer;                //Variavel de display em memoria
  ALLEGRO_DISPLAY       *display;               //Variavel de display
  unsigned char         key[ALLEGRO_KEY_MAX];   //Variavel do teclado
  MOUSE                 *mouse;                 //Variavel do mouse
} ALLEGRO_ENGINE;


// --- DISPLAY ---
//Inicia variaveis de buffer e display
void display_init(ALLEGRO_BITMAP **buff, ALLEGRO_DISPLAY **disp);
//Destroy variaveis de buffer e display
void display_deinit(ALLEGRO_BITMAP **buff, ALLEGRO_DISPLAY **disp);
//Buffer target
void display_pre_draw(ALLEGRO_BITMAP **buff);
//Buffer commit
void display_post_draw(ALLEGRO_BITMAP **buff, ALLEGRO_DISPLAY **disp);


// --- KEYBOARD ---
//Inicia teclado
void keyboard_init(unsigned char key[]);
//Atualiza vetor de teclado
void keyboard_update(ALLEGRO_EVENT* event, unsigned char key[]);


// --- MOUSE ---
//Inicia estrutura mouse
MOUSE *mouse_init();
//Destroi estrutura mouse
void mouse_deinit(MOUSE *mouse);
//Atualiza mouse
void mouse_update(ALLEGRO_EVENT* event, MOUSE *mouse);


#endif
