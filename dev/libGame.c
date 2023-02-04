#include "libGame.h"
#include "utils.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>

// --- AUDIO ---
//Inicia variaveis de audio
void audio_init(AUDIO_T *audio){
  audio->bg_music = al_load_audio_stream("resources/sound/Haggstrom.opus", 2, 2048);
  must_init(audio->bg_music, "Background music");

  audio->fall_snd_effect = al_load_sample("resources/sound/rock_fall.wav");
  must_init(audio->fall_snd_effect, "Fall effect");

  audio->special1_snd_effect = al_load_sample("resources/sound/special_explosion1.wav");
  must_init(audio->special1_snd_effect, "Special1 effect");

  audio->special2_snd_effect = al_load_sample("resources/sound/special_explosion2.wav");
  must_init(audio->special2_snd_effect, "Special2 effect");
}

//Destroi variaveis de audio
void audio_deinit(AUDIO_T *audio){
  al_destroy_audio_stream(audio->bg_music);
  al_destroy_sample(audio->fall_snd_effect);
  al_destroy_sample(audio->special1_snd_effect);
  al_destroy_sample(audio->special2_snd_effect);
}


// --- FONTE ---
//Inicializa variavel de fonte
void font_init(FONT_T *font){
  font->score_font = al_load_font("resources/fonts/MASLITE.otf", 36, 0);
  must_init(font->score_font, "Fonte MASLITE");
}

//Destroi variavel de fonte
void font_deinit(FONT_T *font){
  al_destroy_font(font->score_font);
}

