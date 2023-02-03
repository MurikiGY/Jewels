#ifndef _LIBGAME_
#define _LIBGAME_

//Struct com as variaveis necessárias para o jogo
typedef struct engine_game {
  ALLEGRO_AUDIO_STREAM    *bg_music;              //Musica de fundo
  ALLEGRO_AUDIO_STREAM    *fall_snd_effect;       //Musica de fundo
  ALLEGRO_AUDIO_STREAM    *special1_snd_effect;   //Musica de fundo
  ALLEGRO_AUDIO_STREAM    *special2_snd_effect;   //Musica de fundo
  ALLEGRO_FONT            *font;                  //Fonte
  ALLEGRO_BITMAP          *background;            //imagem de fundo
  STAR                    stars[STARS_N];         //Estrelas
  SCORE                   *game_score;            //Pontuacao
  JEWEL                   **board;                //Tabuleiro 
  ALLEGRO_BITMAP          *piece_sprite[18];      //Sprites de peças
  STATES                  global_state;           //Variaveis de estado
} GAME_ENGINE;




#endif
