#ifndef _UTILS_
#define _UTILS_

#include <stdio.h>
#include <stdbool.h>
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

//Teste de inicialização
void must_init(bool test, const char *description);

//Inteiros aleatorios
int between(int lo, int hi);

//Float aleatorios
float between_f(float lo, float hi);

#endif
